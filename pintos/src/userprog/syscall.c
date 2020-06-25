#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include <devices/shutdown.h>
#include <threads/vaddr.h>
#include <threads/synch.h>
#include <lib/kernel/stdio.h>
#include "threads/malloc.h"
#include "filesys/file.h"
#include "filesys/filesys.h"
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "pagedir.h"
#include "process.h"
#include <string.h>

static void syscall_handler(struct intr_frame *);

int get_syscall_type();

static void get_syscall_arg(struct intr_frame *f, uint32_t *buffer, int argc);


static struct lock filesystem_lock;

void
syscall_init(void) {
    intr_register_int(0x30, 3, INTR_ON, syscall_handler, "syscall");
    lock_init(&filesystem_lock);
}

struct lock * get_fs_lock() {
    return &filesystem_lock;
}




void check_stack(void * esp) {
/*
 * The second method is to check only that a user pointer points below PHYS_BASE, then dereference it.
 * An invalid user pointer will cause a "page fault" that you can handle by modifying the code for page_fault()
 * in userprog/exception.c. This technique is normally faster because it takes advantage of the processor's
 * MMU, so it tends to be used in real kernels (including Linux).
 */
    if (!is_valid_user_vaddr(esp)) {
        syscall_exit(-1);
    } else {
        if (pagedir_get_page(thread_current()->pagedir, pg_round_down(esp + 1)) == NULL)
            syscall_exit(-1);
    }

}

void check_file_addr(const char * file) {
    const char *p = file;
    if (file == NULL) syscall_exit(-1);
    for (; validate_user_addr(p, 1), *p != '\0'; p++) {
        ;
    }
    validate_user_addr(p, 1);
}

static void
syscall_handler(struct intr_frame *f UNUSED) {
    // uint32_t *esp = f->esp;
    uint32_t syscall_args[4];

    check_stack(f->esp);
    int type = get_syscall_type(f);
    switch (type) {
        case SYS_HALT : {
            syscall_halt();
            break;
        }/* Halt the operating system. */
        case SYS_EXIT: {
            // printf("system call [exit] !\n");
            get_syscall_arg(f, syscall_args, 1);
            syscall_exit(syscall_args[0]);
            break;
        }/* Terminate this process. */
        case SYS_EXEC: {
            // printf("system call [exec] !\n");
            get_syscall_arg(f, syscall_args, 1);
            f->eax = syscall_exec((char *) syscall_args[0]);
            break;
        }/* Start another process. */
        case SYS_WAIT: {
            // printf("system call [wait] !\n");
            get_syscall_arg(f, syscall_args, 1);
            f->eax = syscall_wait(syscall_args[0]);
            // printf("wait now return %d\n", f->eax);
            break;
        }                   /* Wait for a child process to die. */
        case SYS_CREATE: {
            // printf("system call [create] !\n");
            get_syscall_arg(f, syscall_args, 2);
            const char * file = (char *)syscall_args[0];
            unsigned int size = syscall_args[1];
            f->eax = syscall_create(file, size);
            break;
        }/* Create a file. */
        case SYS_REMOVE: {
            // printf("system call [remove]!\n");
            get_syscall_arg(f, syscall_args, 1);
            const char * file = (char *)syscall_args[0];
            syscall_remove(file);
            break;
        }/* Delete a file. */
        case SYS_OPEN: {
            // printf("system call [open]!\n");
            get_syscall_arg(f, syscall_args, 1);
            const char * file = (char *)syscall_args[0];
            f->eax = syscall_open(file);
            break;
        }/* Open a file. */
        case SYS_FILESIZE: {
            // printf("system call [filesize]!\n");
            get_syscall_arg(f, syscall_args, 1);
            int fd = syscall_args[0];
            f->eax = syscall_filesize(fd);
            break;
        }               /* Obtain a file's size. */
        case SYS_READ : {
            // printf("system call [read]!\n");
            get_syscall_arg(f, syscall_args, 3);
            int fd = syscall_args[0];
            void * buffer = (void *) syscall_args[1];
            uint32_t size = syscall_args[2];
            f->eax = syscall_read(fd, buffer, size);
            break;
        }                   /* Read from a file. */
        case SYS_WRITE : {
            // (int fd, const void *buffer, unsigned size)
            // printf("system call [write]!\n");
            get_syscall_arg(f, syscall_args, 3);
            int fd = syscall_args[0];
            void * buffer = (void *) syscall_args[1];
            uint32_t size = syscall_args[2];
            f->eax = syscall_write(fd, buffer, size);
            break;
        }
        case SYS_SEEK: {
            // printf("system call [seek]!\n");
            get_syscall_arg(f, syscall_args, 2);
            int fd = syscall_args[0];
            int pos = syscall_args[1];
            syscall_seek(fd, pos);
            break;
        }                   /* Change position in a file. */
        case SYS_TELL : {
            // printf("system call [tell]!\n");
            get_syscall_arg(f, syscall_args, 1);
            int fd = syscall_args[0];
            f->eax = syscall_tell(fd);
            break;
        }                   /* Report current position in a file. */
        case SYS_CLOSE: {
            // printf("system call [close]!\n");
            get_syscall_arg(f, syscall_args, 1);
            int fd = syscall_args[0];
            syscall_close(fd);
            break;
        }
        case SYS_MMAP: {
            get_syscall_arg(f, syscall_args, 2);
            int fd = syscall_args[0];
            void * addr = syscall_args[1];
            f->eax = syscall_mmap(fd, addr);
            break;
        }
        case SYS_MUNMAP: {
            get_syscall_arg(f, syscall_args, 1);
            syscall_munmap((mapid_t) syscall_args[0]);
            break;
        }
        default: {
            printf("other system call...\n");
            syscall_exit(-1);
            break;
        }
    }
}

static void
validate_user_addr(const void *uaddr, unsigned int len) {
    for (const void *addr = uaddr; addr < uaddr + len; ++addr) {
        if ((!addr) || !(is_valid_user_vaddr(addr))) {
            syscall_exit(-1);
        }
        if (pagedir_get_page(thread_current()->pagedir, pg_round_down(addr)) == NULL) {
            syscall_exit(-1);
        }
    }
}

static void
get_syscall_arg(struct intr_frame *f, uint32_t *buffer, int argc) {
    for (int i = 0; i < argc; i++) {
        validate_user_addr((uint32_t *) f->esp + i + 1, 4);
        *buffer = *((uint32_t *) f->esp + i + 1);
        buffer++;
    }
}

int get_syscall_type(struct intr_frame *f) {
    // printf("stack pointer: %x\n", f->esp);
    validate_user_addr(f->esp, sizeof(uint32_t));
    return *((uint32_t *) f->esp);
}

int syscall_write(int fd, const void *buffer, unsigned size) {
    lock_acquire(&filesystem_lock);
    validate_user_addr(buffer, size);
    switch (fd) {
        case 0: //wirte to stdin, meaningless
            lock_release(&filesystem_lock);
            syscall_exit(-1);
            return -1;
        case 1: // write to stdout, which is console
            // printf("write to console now!!\n");
            putbuf((const char *)buffer, size);
            lock_release(&filesystem_lock);
            return size;
        default: {
            // printf("write to file descriptor %d\n", fd);
            struct file_descriptor * fd_ptr = get_fd_ptr(thread_current(), fd);
            if (fd_ptr == NULL) {
                lock_release(&filesystem_lock);
                syscall_exit(-1);
            }

            int write_size = file_write(fd_ptr->opened_file, buffer, size);
            lock_release(&filesystem_lock);
            return write_size;
        }
    }
}

int syscall_read (int fd, void *buffer, unsigned size) {
    lock_acquire(&filesystem_lock);
    validate_user_addr(buffer, size);
    switch (fd) {
        case 0: //wirte to stdin, meaningless
            lock_release(&filesystem_lock);
            syscall_exit(-1);
            return -1;
        case 1: // write to stdout, which is console
            // printf("write to console now!!\n");
            lock_release(&filesystem_lock);
            syscall_exit(-1);
            return size;
        default: {
            // printf("write to file descriptor %d\n", fd);
            struct file_descriptor * fd_ptr = get_fd_ptr(thread_current(), fd);
            if (fd_ptr == NULL) {
                lock_release(&filesystem_lock);
                syscall_exit(-1);
            }
            int read_size = file_read(fd_ptr->opened_file, buffer, size);
            lock_release(&filesystem_lock);
            // printf("read on %d done normally without sys_exit\n ", fd);
            return read_size;
        }
    }
}

void syscall_exit (int status) {
    struct thread * t = thread_current();
    t->exit_status = status;
    while (!list_empty(&t->file_descriptor_list)) {
        struct file_descriptor * fd_ptr = list_entry(list_pop_front(&t->file_descriptor_list),
                struct file_descriptor, elem);
        file_close(fd_ptr->opened_file);
        t->fd_used[fd_ptr->fd] = false;
        free(fd_ptr);
    }
    // debug_backtrace();
    // printf("stupid child exit %d\n", t->tid);
    printf("%s: exit(%d)\n", thread_current()->name, status);
    thread_exit();
}

void syscall_halt(void) {
    shutdown_power_off();
    NOT_REACHED ();
}

bool syscall_remove (const char *file) {
    lock_acquire(&filesystem_lock);
    check_file_addr(file);
    bool success = filesys_remove(file);
    lock_release(&filesystem_lock);
    return success;
}

bool syscall_create (const char *file, unsigned initial_size) {
    lock_acquire(&filesystem_lock);
    check_file_addr(file);
    bool success = filesys_create(file, initial_size);
    lock_release(&filesystem_lock);
    return success;
}

int select_unused_fd(struct thread * t) {
    for (int i = 3; i < 140; i++) {
        if (!t->fd_used[i]) {
            return i;
        }
    }
}

int syscall_open (const char *file) {
    /*
     * Opens the file called file. Returns a non-negative integer handle called a
     * "file descriptor" (fd), or -1 if the file could not be opened.
     */
    lock_acquire(&filesystem_lock);
    check_file_addr(file);
    struct file * fp = filesys_open(file);
    if (fp == NULL) {
        lock_release(&filesystem_lock);
        return -1;
    }

    struct thread * t = thread_current();
    int fd = select_unused_fd(t);

    struct file_descriptor * fileDescriptor = malloc (sizeof(struct file_descriptor));
    // printf("open new file, fd is %d\n", fd);
    fileDescriptor->fd = fd;
    fileDescriptor->holder = thread_current();
    fileDescriptor->name = (char *) file;
    fileDescriptor->opened_file = fp;

    t->fd_used[fd] = true;
    list_push_back(&t->file_descriptor_list, &fileDescriptor->elem );
    lock_release(&filesystem_lock);
    return fd;

}

struct file_descriptor * get_fd_ptr(struct thread * t, int fd) {
    struct list_elem * e;
    for (e = list_begin(&t->file_descriptor_list); e != list_end(&t->file_descriptor_list);
         e = list_next(e)) {
        struct file_descriptor * fileDescriptor = list_entry(e, struct file_descriptor, elem);
        if (fileDescriptor->fd == fd) {
            return fileDescriptor;
        }
    }
    return NULL;
}

void syscall_close(int fd) {
    struct thread * t = thread_current();
    lock_acquire(&filesystem_lock);

    if (fd <= 1 || list_empty(&t->file_descriptor_list)) {
        // printf("This thread (%s) has no opened file\n", t->name);
        lock_release(&filesystem_lock);
        syscall_exit(-1);
    }
    struct file_descriptor * fd_ptr = get_fd_ptr(t, fd);
    if (fd_ptr == NULL) {
        // printf("No such fd opened: %d\n", fd);
        lock_release(&filesystem_lock);
        syscall_exit(-1);
    }
    file_close(fd_ptr->opened_file);
    t->fd_used[fd] = false;
    list_remove(&fd_ptr->elem);
    free(fd_ptr);
    lock_release(&filesystem_lock);
}

int syscall_filesize(int fd) {
    // printf("call sys_filesize\n");
    lock_acquire(&filesystem_lock);
    struct file_descriptor * fileDescriptor = get_fd_ptr(thread_current(), fd);
    if (fileDescriptor == NULL) {
        lock_release(&filesystem_lock);
        syscall_exit(-1);
    }
    struct file * fp = fileDescriptor->opened_file;
    int ans = file_length(fp);
    lock_release(&filesystem_lock);
    return ans;
}

pid_t syscall_exec (const char *file) {
    check_file_addr(file);
    /*
     * Thus, the parent process cannot return from the exec until it knows whether the child
     * process successfully loaded its executable. You must use appropriate
     * synchronization to ensure this.
     */
    int tid = process_execute(file);
    // printf("exec %s \n", file);
    return tid;
}

int syscall_wait (pid_t pid) {
    return process_wait(pid);
}

void syscall_seek (int fd, unsigned position) {
    lock_acquire(&filesystem_lock);
    struct file_descriptor * fileDescriptor = get_fd_ptr(thread_current(), fd);
    if (fileDescriptor == NULL) {
        lock_release(&filesystem_lock);
        syscall_exit(-1);
    }
    struct file * fp = fileDescriptor->opened_file;
    file_seek(fp, position);
    lock_release(&filesystem_lock);
}

int syscall_tell (int fd) {
    lock_acquire(&filesystem_lock);
    struct file_descriptor * fileDescriptor = get_fd_ptr(thread_current(), fd);
    if (fileDescriptor == NULL) {
        lock_release(&filesystem_lock);
        syscall_exit(-1);
    }
    struct file * fp = fileDescriptor->opened_file;
    int pos = file_tell(fp);
    lock_release(&filesystem_lock);
    return pos;
}


void syscall_munmap (mapid_t mapping) {

}

mapid_t syscall_mmap (int fd, void *addr) {
    return 0;
}

