#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include <devices/shutdown.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

static void syscall_handler(struct intr_frame *);

void
syscall_init(void) {
    intr_register_int(0x30, 3, INTR_ON, syscall_handler, "syscall");
}

void syscall_halt (void) {
    shutdown_power_off();
    NOT_REACHED ();
}

static void
syscall_handler(struct intr_frame *f UNUSED) {
    printf("system call!\n");
    uint32_t *esp = f->esp;

    thread_exit();
}
