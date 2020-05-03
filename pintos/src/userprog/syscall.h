#include <lib/stdbool.h>

#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

void syscall_init (void);
void syscall_halt (void);
int syscall_write(int fd, const void *buffer, unsigned size);
int syscall_read (int fd, void *buffer, unsigned size);
void syscall_exit (int status);
bool syscall_remove (const char *file);
bool syscall_create (const char *file, unsigned initial_size);
int syscall_open (const char *file);

#endif /* userprog/syscall.h */
