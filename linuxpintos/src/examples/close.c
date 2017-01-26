#include <syscall.h>

int main(){

  create("close_test.txt", 1024);
  int fd = open("close_test.txt");
  close(fd);

  halt();

  return 0;
}
