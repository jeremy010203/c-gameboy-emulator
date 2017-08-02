#ifndef MMU_H
# define MMU_H

#include <stdlib.h>
#include <stdio.h>
#include "registers.h"
#include "utils.h"

typedef struct Mmu
{
  uint8_t memory[0xFFFF];
} Mmu;

Mmu MMU;

void init_mmu(char *path);
void load_rom(char *path);
void load_bios(char *path);
void print_memory(uint16_t from, uint16_t to);
uint8_t read_memory(uint16_t addr);
void write_memory(uint16_t addr, uint8_t val);
void request_interupt(uint8_t val);
void do_interupt(void);
void execute_interupt(uint8_t i);

#endif /* MMU_H */
