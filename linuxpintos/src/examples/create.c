#include <syscall.h>

int main (void) 
{
  create("test1", 1024);

  create("test2", 1024);

  halt();

  return 0;
}
