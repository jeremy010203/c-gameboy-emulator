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
void load_rom(char *path);
static void load_bios(char *path);
void print_memory(uint16_t from, uint16_t to);

#endif /* MMU_H */
