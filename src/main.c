#include <stdlib.h>
#include <stdio.h>
#include "utils.h"

int main(void)
{
  init();

  printf("Load bios successfully\n");

  while (1)
  {
    printf("%x -> ", r.PC.val);
    uint8_t op = read_byte();
    printf("%x\n", op);
    execute(op);
    //print_r();
  }

  return 0;
}
