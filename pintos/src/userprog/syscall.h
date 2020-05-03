#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

void syscall_init (void);
void syscall_halt (void);
int syscall_write(int fd, const void *buffer, unsigned size);
int syscall_read (int fd, void *buffer, unsigned size);
void syscall_exit (int status);

#endif /* userprog/syscall.h */
