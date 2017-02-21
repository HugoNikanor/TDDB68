/* Tests the exit system call. */

#include "syscall.h"

void
main (void) 
{
  exit (57);
  printf("Should not be reached!");
}
