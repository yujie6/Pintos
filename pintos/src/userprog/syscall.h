#include <lib/stdbool.h>
#include <threads/thread.h>
#include <lib/user/syscall.h>

#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

static void
validate_user_addr(const void *uaddr, unsigned int len);
struct file_descriptor * get_fd_ptr(struct thread * t, int fd);
void check_file_addr(const char * file);
void syscall_init (void);
void syscall_halt (void);
int syscall_write(int fd, const void *buffer, unsigned size);
int syscall_read (int fd, void *buffer, unsigned size);
void syscall_exit (int status);
bool syscall_remove (const char *file);
bool syscall_create (const char *file, unsigned initial_size);
int syscall_open (const char *file);
void syscall_close(int fd);
int syscall_filesize(int fd);
pid_t syscall_exec (const char *file);
int syscall_wait (pid_t pid);
struct lock * get_fs_lock();

#endif /* userprog/syscall.h */
