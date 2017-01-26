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

  printf("ID: %i \n", id);

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
    case SYS_OPEN: ;
      char *f_name = *((char**)(sp));
      f->eax = open(f_name);
    case SYS_CLOSE: ;
      int fd_close = *((int*)(sp));
      close(fd_close);
  }
}

void
halt (void) 
{
  power_off(); 
}

bool
create (const char *file, unsigned initial_size)
{
  return filesys_create(file, initial_size);
}

int
open (const char *file_name){
  struct file *f = filesys_open(file_name);

  if(f == NULL){
    return -1;
  }
  else{
    struct thread *current_thread = thread_current();
    int fd = -1;
    int i;

    for(i = 0; i < 128; i++){
      if(current_thread->file_list[i] == NULL){
	current_thread->file_list[i] = f;
	fd = i + 2; //File descriptor is 2 higher than index, since i = 0, 1 belongs to console
      }
    }
    printf("OPEN FD: %i \n", fd);
    return fd;
  }
}

void
close(int fd){
  struct thread *cur_thread = thread_current();
  struct file *close_file = cur_thread->file_list[fd-2];
  printf("FD: %i \n", fd);

  //file_close(close_file);
  //cur_thread->file_list[fd - 2] = NULL;
}
