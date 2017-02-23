#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/init.h"
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "devices/input.h"
#include "threads/vaddr.h"

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
  if (!is_valid_pointer(sp)) {
    exit(-1);
  }
  int id = *((int*)(sp));

  update_sp(&sp);

  switch(id) {
    case SYS_HALT: {
      halt(); 
      break;
    }
    case SYS_CREATE: {
      char *file_name = *((char**)(sp));
      update_sp(&sp);
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
      update_sp(&sp);
      void *buffer = *((void**)sp);
      update_sp(&sp);
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
      int exit_status = *((int*)(sp));
      exit(exit_status);
      break;
    }
    case SYS_EXEC: {
      char *cmd_line_exec = *((char**)sp);
      f->eax = exec(cmd_line_exec);
      break;
    }
    case SYS_WAIT:{
      tid_t pid = *((tid_t*)sp);
      f->eax = wait(pid);
      break;
    }
  }
}

void 
update_sp(void **sp) {
  (*sp) += 4;
  if(!is_valid_pointer(*sp)) {
    exit(-1); 
  }
}

bool
is_valid_pointer(void *ptr) {
  return (ptr < PHYS_BASE && pagedir_get_page(thread_current()->pagedir, ptr) != NULL);
}

bool
is_valid_string(char *string) {
  do{
    if(!is_valid_pointer(string)){
      return false;
    }
  }while(*(string++) != '\0');
  return true;
}

bool
is_valid_buffer(void *buffer, unsigned size) {
  unsigned i;
  for(i = 0; i < size; i++) {
    if(!is_valid_pointer(buffer)) {
      return false; 
    }
    buffer++; 
  }
  return true; 
}
 
void
halt (void) 
{
  power_off(); 
}

bool
create (const char *file, unsigned initial_size)
{
  if (!is_valid_string(file))
    {
      exit(-1);
    }
  return filesys_create(file, initial_size);
}

int
open (const char *file_name){
  if (!is_valid_string(file_name)) {
      exit(-1);
  }
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
  if (fd < 2 || fd > 129) {
     exit(-1);
  }

  struct thread *cur_thread = thread_current();
  struct file *close_file = cur_thread->file_list[fd-2];

  file_close(close_file);
  cur_thread->file_list[fd - 2] = NULL;
}

int 
read(int fd, void *buffer, unsigned size){
  if (!is_valid_buffer(buffer, size)) {
    exit(-1);
  }
  
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
    if(size == 0){
      return 0;
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
  if (!is_valid_buffer(buffer, size)) {
    exit(-1);
  }
  

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

    if(write_file == NULL){
      return -1;
    }

    if(size == 0) {
      return 0; 
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
  printf("%s: exit(%d)\n", thread_current()->name, status);
  
  struct thread *cur_thread = thread_current();
  cur_thread->parent_relation->exit_status = status;

  thread_exit();
}

tid_t 
exec(const char *cmd_line){
  if(!is_valid_string(cmd_line)) {
    exit(-1);  
  }
  return process_execute(cmd_line);
}

int wait(tid_t pid){
  if (pid == TID_ERROR) {
    exit(-1);
  }

  return process_wait(pid);
}

