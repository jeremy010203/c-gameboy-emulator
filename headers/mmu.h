#ifndef MMU_H
# define MMU_H

#include <stdlib.h>
#include <stdio.h>
#include "registers.h"
#include "utils.h"

typedef struct Mmu
{
  uint8_t memory[0x10000];
  uint8_t ram[0x8000 + 1];
  uint8_t game[0x200000];
  uint8_t MBC1;
  uint8_t MBC2;
  uint8_t CUR_ROM;
  uint8_t CUR_RAM;
  uint8_t ENABLE_RAM;
  uint8_t ROM_BANKING;
  uint8_t HALT;
  char *path_rom;
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
