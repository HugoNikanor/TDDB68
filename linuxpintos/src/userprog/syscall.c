#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/init.h"
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "devices/input.h"

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
  int id = *((int*)(sp));

  sp += 4;

  switch(id) {
    case SYS_HALT: {
      halt(); 
      break;
    }
    case SYS_CREATE: {
      char *file_name = *((char**)(sp));
      sp += 4;
      unsigned new_size = *((unsigned*)(sp));
      f->eax = create(file_name, new_size);
      break;
    }
    case SYS_OPEN: {
      char *f_name = *((char**)(sp));
      f->eax = open(f_name);
      break;
    }
    case SYS_CLOSE: {
      int fd_close = *((int*)(sp));
      close(fd_close);
      break;
    }
    case SYS_READ:
    case SYS_WRITE: {
      int fd_rw = *((int*)(sp));
      sp += 4;
      void *buffer = *((void**)sp);
      sp += 4;
      unsigned buffer_size = *((unsigned*)(sp));
      if(id == SYS_WRITE){
	f->eax = write(fd_rw, buffer, buffer_size);
      }
      else{
	f->eax = read(fd_rw, buffer, buffer_size);
      }
      break;
    }
    case SYS_EXIT: {
      exit(0);
      break;
    }
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

    for(i = 0; i < 128 && fd == -1; i++){
      if(current_thread->file_list[i] == NULL){
	current_thread->file_list[i] = f;
	fd = i + 2; //File descriptor is 2 higher than index, since i = 0, 1 belongs to console
      }
    }
    return fd;
  }
}

void
close(int fd){
  struct thread *cur_thread = thread_current();
  struct file *close_file = cur_thread->file_list[fd-2];

  file_close(close_file);
  cur_thread->file_list[fd - 2] = NULL;
}

int 
read(int fd, void *buffer, unsigned size){
  if(fd == 0){
    //Read from console
    unsigned i = 0;
    for(i = 0; i < size; i++){
      *((uint8_t*)buffer) = input_getc();
      buffer++;
    }
    return size;
  }
  else if (fd >= 2 && fd <= 127){
    struct thread *cur_thread = thread_current();
    struct file *read_file = cur_thread->file_list[fd-2];

    if(read_file == NULL){
      return -1;
    }

    int read_bytes = file_read(read_file, buffer, (off_t)size);

    if(read_bytes == 0){
      return -1;
    }
    else{
      return read_bytes;
    }
  }
  else{
    //Not legitimate fd
    return -1;
  }
}

int 
write(int fd, void *buffer, unsigned size){
  if(fd == 1){
    //write to console
    int remaining_bytes = size;

    while(remaining_bytes != 0){
      if(remaining_bytes < 100){
	putbuf((char*) buffer, remaining_bytes);
	remaining_bytes = 0;
      }
      else{
        putbuf((char*) buffer,100);
	remaining_bytes -= 100;
      }
    }

    return size;
  }
  else if(fd >= 2 && fd <= 127){
    struct thread *cur_thread = thread_current();
    struct file *write_file = cur_thread->file_list[fd-2];

    printf("write file is %i\n", write_file);

    if(write_file == NULL){
      return -1;
    }
    
    int written_bytes = file_write(write_file, buffer, size);

   if(written_bytes == 0){
      return -1;
    }
    else{
      return written_bytes;
    } 
  }
  else{
    return -1;
  }
}

void
exit(int status) {
  thread_exit();
}
