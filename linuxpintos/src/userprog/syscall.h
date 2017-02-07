#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H
#include <stdbool.h>
#include <threads/thread.h>

void syscall_init (void);
void halt(void);
bool create(const char *file, unsigned initial_size);
int open(const char *file_name);
void close(int fd);
int read(int fd, void *buffer, unsigned size);
int write(int fd, void *buffer, unsigned size);
void exit(int status);
tid_t exec(const char *cmd_line);
int wait(tid_t pid);
#endif /* userprog/syscall.h */
