#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include <devices/shutdown.h>
#include <threads/vaddr.h>
#include <threads/synch.h>
#include <lib/kernel/stdio.h>
#include "filesys/file.h"
#include "filesys/filesys.h"
#include "threads/interrupt.h"
#include "threads/thread.h"

static void syscall_handler(struct intr_frame *);

static int get_syscall_type();

static void get_syscall_arg(struct intr_frame *f, uint32_t *buffer, int argc);



static struct lock filesystem_lock;

void
syscall_init(void) {
    intr_register_int(0x30, 3, INTR_ON, syscall_handler, "syscall");
    lock_init(&filesystem_lock);
}



void check_stack(void * esp) {
/*
 * The second method is to check only that a user pointer points below PHYS_BASE, then dereference it.
 * An invalid user pointer will cause a "page fault" that you can handle by modifying the code for page_fault()
 * in userprog/exception.c. This technique is normally faster because it takes advantage of the processor's
 * MMU, so it tends to be used in real kernels (including Linux).
 */
}

static void
syscall_handler(struct intr_frame *f UNUSED) {
    // uint32_t *esp = f->esp;
    uint32_t syscall_args[4];

    int type = get_syscall_type(f);
    check_stack(f->esp);
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
            break;
        }/* Start another process. */
        case SYS_WAIT: {
            // printf("system call [wait] !\n");
            break;
        }                   /* Wait for a child process to die. */
        case SYS_CREATE: {
            // printf("system call [create] !\n");
            break;
        }/* Create a file. */
        case SYS_REMOVE: {
            // printf("system call [remove]!\n");
            break;
        }/* Delete a file. */
        case SYS_OPEN: {
            // printf("system call [open]!\n");
            break;
        }/* Open a file. */
        case SYS_FILESIZE: {
            // printf("system call [filesize]!\n");
            break;
        }               /* Obtain a file's size. */
        case SYS_READ : {
            // printf("system call [read]!\n");
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
            break;
        }                   /* Change position in a file. */
        case SYS_TELL : {
            // printf("system call [tell]!\n");
            break;
        }                   /* Report current position in a file. */
        case SYS_CLOSE: {
            // printf("system call [close]!\n");
            break;
        }
        default: {
            // printf("other system call...\n");
            break;
        }
    }
}

static void
validate_user_addr(const void *uaddr, unsigned int len) {
    for (const void *addr = uaddr; addr < uaddr + len; ++addr) {
        if ((!addr) || !(is_user_vaddr(addr))) {
            thread_exit();
            return;
        }
    }
}

static void
get_syscall_arg(struct intr_frame *f, uint32_t *buffer, int argc) {
    for (int i = 0; i < argc; i++) {
        *buffer = *((uint32_t *) f->esp + i + 1);
        buffer++;
    }
}

int get_syscall_type(struct intr_frame *f) {
    validate_user_addr(f->esp, sizeof(uint32_t));
    return *((uint32_t *) f->esp);
}

int syscall_write(int fd, const void *buffer, unsigned size) {
    lock_acquire(&filesystem_lock);
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
            lock_release(&filesystem_lock);
            break;
        }
    }

}

int syscall_read (int fd, void *buffer, unsigned size) {
    return size;
}

void syscall_exit (int status) {
    thread_current()->exit_status = status;
    printf("%s: exit(%d)\n", thread_current()->name, status);
    thread_exit();
}

void syscall_halt(void) {
    shutdown_power_off();
    NOT_REACHED ();
}


