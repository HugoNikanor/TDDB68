#include <syscall.h>

int main(int argc, char *argv[]){
  create("open_test", 1028);

  open("open_test");

  halt();

  return 0;
}
