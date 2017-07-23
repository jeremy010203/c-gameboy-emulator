#include <stdlib.h>
#include <stdio.h>
#include "utils.h"

int main(void)
{
  init();

  printf("Load bios successfully\n");
  int debug = 0;

  while (1)
  {
    printf("%x -> ", r.PC.val);
    uint8_t op = read_byte();
    printf("%x\n", op);
    if (debug && op == 0xCD)
    {
      print_memory(r.PC.val, r.PC.val + 10);
      exit(1);
    }
    execute(op);
    //print_r();
  }

  return 0;
}
