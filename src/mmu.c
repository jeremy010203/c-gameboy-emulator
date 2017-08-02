#include "mmu.h"

void init_mmu(char *path)
{
  for (uint16_t i = 0; i < 0xFFFF; i++)
  {
    write_memory(i, 0);
  }
  load_rom("misc/Tetris.gb");
  load_bios(path);
}

void load_bios(char *path)
{
  FILE *file;
  unsigned long len;
  file = fopen(path, "rb");
  if (!file)
  {
    fprintf(stderr, "Error loading file %s\n", path);
    exit(1);
  }
  fseek(file, 0, SEEK_END);
  len = ftell(file);
  rewind(file);

  fread(&MMU.memory, len, 1, file);
  fclose(file);
}

void load_rom(char *path)
{
  FILE *file;
  unsigned long len;
  file = fopen(path, "rb");
  if (!file)
  {
    fprintf(stderr, "Error loading file %s\n", path);
    exit(1);
  }
  fseek(file, 0, SEEK_END);
  len = ftell(file);
  rewind(file);

  fread(&MMU.memory, len, 1, file);
  fclose(file);
}

void print_memory(uint16_t from, uint16_t to)
{
  for (uint16_t i = from; i <=to; i++)
  {
    printf("%x - %x\n", i, read_memory(i));
  }
}

uint8_t get_joypad(void)
{
  uint8_t mem = MMU.memory[0xFF00];
  mem ^= 0xFF;
  if (!test_bit(mem, 4))
  {
    uint8_t joy = (r.joypad >> 4) | 0xF0;
    mem &= joy;
  }
  else if (!test_bit(mem, 5))
  {
    uint8_t joy = (r.joypad & 0xF) | 0xF0;
    mem &= joy;
  }
  return mem;
}

uint8_t read_memory(uint16_t addr)
{
  // Catch joypad request
  if (addr == 0xFF00)
  {
    return get_joypad();
  }
  return MMU.memory[addr];
}

void write_memory(uint16_t addr, uint8_t val)
{
  // Detect disable boot rom
  if (addr == 0xFF50 && val == 1)
  {
    load_rom("misc/Tetris.gb");
  }
  MMU.memory[addr] = val;
}
