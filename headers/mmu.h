#ifndef MMU_H
# define MMU_H

#include <stdlib.h>
#include <stdio.h>

typedef struct Mmu
{
  uint8_t memory[0xFFFF];
} Mmu;

Mmu MMU;

void init_mmu(char *path);
static void load_bios(char *path);

#endif /* MMU_H */
