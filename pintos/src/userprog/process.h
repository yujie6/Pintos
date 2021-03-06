#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include <threads/synch.h>
#include "threads/thread.h"

struct start_process_arg {
    char * file_name;
    struct semaphore sema;
    bool success;
};

tid_t process_execute (const char *file_name);
int process_wait (tid_t);
void process_exit (void);
void process_activate (void);
struct process_info *get_child_process(tid_t child_tid, struct list child_list);
char * get_file_name(char * cmd_line);


#endif /* userprog/process.h */
