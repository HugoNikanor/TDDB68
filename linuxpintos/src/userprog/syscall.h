#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H
#include <stdbool.h>
#include <threads/thread.h>

void syscall_init (void);
void update_sp(void **sp);
bool is_valid_pointer(void *ptr);
bool is_valid_string(char *string);
bool is_valid_buffer(void *buffer, unsigned size);
void halt(void);
bool create(const char *file, unsigned initial_size);
int open(const char *file_name);
void close(int fd);
int read(int fd, void *buffer, unsigned size);
int write(int fd, void *buffer, unsigned size);
void exit(int status);
tid_t exec(const char *cmd_line);
int wait(tid_t pid);
void seek(int fd, unsigned position);
unsigned tell(int fd);
int filesize(int fd);
bool remove (const char *file_name);
#endif /* userprog/syscall.h */
