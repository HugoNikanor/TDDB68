/* Executes and waits for a single child process. */

#include <syscall.h>
//#include "tests/lib.h"
//#include "tests/main.h"

void
main (void) 
{
  wait (exec ("insult"));
}
