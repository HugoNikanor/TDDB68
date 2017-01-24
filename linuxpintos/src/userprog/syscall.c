#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/init.h"
#include "filesys/filesys.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  void* sp = f->esp;
  printf ("system call!\n");
  int id = *((int*)(sp));

  sp += 4;

  printf("ID: %i", id);

  switch(id) {
    case SYS_HALT:
      halt(); 
      break;
    case SYS_CREATE: ; //Fulhax  
      char *file_name = *((char**)(sp));
      sp += 4;
      unsigned size = *((unsigned*)(sp));
      f->eax = create(file_name, size);
      break;
  }
}

static void
halt (void) 
{
  power_off(); 
}

static bool
create (const char *file, unsigned initial_size)
{
  return filesys_create(file, initial_size);
}
