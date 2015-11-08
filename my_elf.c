/*
objcopy --info  (to look supported output tpe and arch) 
objcopy -I binary -O  elf64-x86-64 -B i386 test.c test.bin

gcc -o test  test.c test.bin

*/

#include <stdio.h>
#include <stdlib.h>

extern char _binary_test_c_start;

int main(){

  printf("%s", (char*)&_binary_test_c_start);

  return 0;

}
