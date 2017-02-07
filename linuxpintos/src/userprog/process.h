#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"
#include "threads/synch.h"

tid_t process_execute (const char *file_name);
int process_wait (tid_t);
void process_exit (void);
void process_activate (void);

struct process_starter{
  struct semaphore *sema;
  tid_t *tid;
  void *file_name;
  struct thread *child_thread;
};

#endif /* userprog/process.h */
