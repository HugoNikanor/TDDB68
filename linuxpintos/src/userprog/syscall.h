#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H
#include <stdbool.h>

void syscall_init (void);
static void halt(void);
static bool create(const char *file, unsigned initial_size);
#endif /* userprog/syscall.h */
