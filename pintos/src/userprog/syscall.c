#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include <devices/shutdown.h>
#include <threads/vaddr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

static void syscall_handler(struct intr_frame *);

static int get_syscall_type();

void sys_exit(int status);

void
syscall_init(void) {
    intr_register_int(0x30, 3, INTR_ON, syscall_handler, "syscall");
}

void syscall_halt(void) {
    shutdown_power_off();
    NOT_REACHED ();
}

static void
syscall_handler(struct intr_frame *f UNUSED) {
    // uint32_t *esp = f->esp;
    int type = get_syscall_type(f);
    switch (type) {
        case SYS_HALT : {
            printf("system call [halt] !\n");
            break;
        }/* Halt the operating system. */
        case SYS_EXIT: {
            printf("system call [exit] !\n");
            thread_exit();
            break;
        }/* Terminate this process. */
        case SYS_EXEC: {
            printf("system call [exec] !\n");
            break;
        }/* Start another process. */
        case SYS_WAIT: {
            printf("system call [wait] !\n");
            break;
        }                   /* Wait for a child process to die. */
        case SYS_CREATE: {
            printf("system call [create] !\n");
            break;
        }/* Create a file. */
        case SYS_REMOVE: {
            printf("system call [remove]!\n");
            break;
        }/* Delete a file. */
        case SYS_OPEN: {
            printf("system call [open]!\n");
            break;
        }/* Open a file. */
        case SYS_FILESIZE: {
            printf("system call [filesize]!\n");
            break;
        }               /* Obtain a file's size. */
        case SYS_READ : {
            printf("system call [read]!\n");
            break;
        }                   /* Read from a file. */
        case SYS_WRITE : {
            printf("system call [write]!\n");
            break;
        }                  /* Write to a file. */
        case SYS_SEEK: {
            printf("system call [seek]!\n");
            break;
        }                   /* Change position in a file. */
        case SYS_TELL : {
            printf("system call [tell]!\n");
            break;
        }                   /* Report current position in a file. */
        case SYS_CLOSE: {
            printf("system call [close]!\n");
            break;
        }
        default: {
            printf("other system call...\n");
            break;
        }
    }
}

static void
valid_uaddr(const void *uaddr, unsigned int len) {
    for (const void *addr = uaddr; addr < uaddr + len; ++addr) {
        if ((!addr) || !(is_user_vaddr(addr))) {
            thread_exit();
            return;
        }
    }
}

int get_syscall_type(struct intr_frame *f) {
    valid_uaddr(f->esp, sizeof(uint32_t));
    return *((uint32_t *) f->esp);
}

void sys_exit(int status) {
    thread_exit();
}


