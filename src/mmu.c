#include "mmu.h"

void init_mmu(char *path)
{
  memset(&MMU.memory, 0, sizeof(MMU.memory));
  memset(&MMU.ram, 0, sizeof(MMU.ram));

  load_rom(MMU.path_rom);
  load_bios(path);

  MMU.MBC1 = (MMU.memory[0x147] == 1 || MMU.memory[0x147] == 2 || MMU.memory[0x147] == 3);
  MMU.MBC2 = (MMU.memory[0x147] == 5 || MMU.memory[0x147] == 6);
  MMU.CUR_ROM = 1;
  MMU.CUR_RAM = 0;
  MMU.ROM_BANKING = 0;
  MMU.HALT = 0;
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

  fread(&MMU.game, len, 1, file);
  fclose(file);

  memcpy(&MMU.memory, &MMU.game, 0x8000);
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
  if ((addr >= 0x4000) && (addr <= 0x7FFF))
  {
    unsigned int new_addr = addr;
    addr += ((MMU.CUR_ROM - 1) * 0x4000);
    return MMU.game[new_addr];
  }
  else if ((addr >= 0xA000) && (addr <= 0xBFFF))
  {
    return MMU.ram[(addr - 0xA000) + (MMU.CUR_RAM * 0x2000)];
  }

  // Catch joypad request
  if (addr == 0xFF00)
  {
    return get_joypad();
  }
  return MMU.memory[addr];
}

void handle_bank_enable(uint16_t addr, uint8_t val)
{
  if (MMU.MBC2 && test_bit(addr, 4))
    return;

  if ((val & 0xF) == 0xA)
    MMU.ENABLE_RAM = 1;
  else if ((val & 0xF) == 0x0)
    MMU.ENABLE_RAM = 0;
}

void DoChangeLoROMBank(uint8_t val)
{
   if (MMU.MBC2)
   {
     MMU.CUR_ROM = val & 0xF;
     if (MMU.CUR_ROM == 0) MMU.CUR_ROM++;
     return;
   }

   uint8_t lower5 = val & 31;
   MMU.CUR_ROM &= 224;
   MMU.CUR_ROM |= lower5;
   if (MMU.CUR_ROM == 0) MMU.CUR_ROM++;
}

void DoChangeHiRomBank(uint8_t data)
{
   MMU.CUR_ROM &= 31;

   data &= 224;
   MMU.CUR_ROM |= data;
   if (MMU.CUR_ROM == 0) MMU.CUR_ROM++;
}

void DoRAMBankChange(uint8_t data)
{
   MMU.CUR_RAM = data & 0x3;
}

void DoChangeROMRAMMode(uint8_t data)
{
   uint8_t newData = data & 0x1;
   MMU.ROM_BANKING = (newData == 0);
   if (MMU.ROM_BANKING)
     MMU.CUR_RAM = 0;
}

void write_memory(uint16_t addr, uint8_t val)
{
  if (addr < 0x8000)
  {
    if (addr < 0x2000 && (MMU.MBC1 || MMU.MBC2))
    {
        handle_bank_enable(addr, val);
    }
   else if (((addr >= 0x2000) && (addr < 0x4000)) && (MMU.MBC1 || MMU.MBC2))
   {
       DoChangeLoROMBank(val);
   }
   else if (((addr >= 0x4000) && (addr < 0x6000)) && MMU.MBC1)
   {
       if(MMU.ROM_BANKING)
         DoChangeHiRomBank(val);
       else
         DoRAMBankChange(val);
   }
   else if (((addr >= 0x6000) && (addr < 0x8000)) && MMU.MBC1)
   {
       DoChangeROMRAMMode(val);
   }
  else if (((addr >= 0xA000) && (addr < 0xC000)) && MMU.ENABLE_RAM)
   {
       MMU.ram[(addr - 0xA000) + (MMU.CUR_RAM * 0x2000)] = val;
   }
   return;
 }

  // Read only
  if ((addr >= 0xFEA0 ) && (addr < 0xFEFF))
    return;

  // Detect disable boot rom
  if (addr == 0xFF50 && val == 1)
  {
    load_rom(MMU.path_rom);
  }
  else if (addr == 0xFF46)
  {
    uint16_t address = val << 8;
    for (int i = 0 ; i < 0xA0; i++)
    {
      write_memory(0xFE00 + i, read_memory(address + i)) ;
    }
    return;
  }
  else if (addr == 0xFF04) // Divider register
  {
    MMU.memory[addr] = 0;
    return;
  }

  MMU.memory[addr] = val;
  if ((addr >= 0xE000 ) && (addr < 0xFE00))
    write_memory(addr - 0x2000, val);
}

void request_interupt(uint8_t val)
{
  // printf("Request interupt: %u\n", val);
  uint8_t mem = read_memory(0xFF0F);
  mem |= (1 << val);
  write_memory(0xFF0F, mem);
}

void do_interupt(void)
{
  if (r.ime)
  {
    uint8_t mem = read_memory(0xFF0F);
    uint8_t ena = read_memory(0xFFFF);
    if (mem > 0)
    {
        for (uint8_t i = 0; i < 5; i++)
        {
          if (test_bit(mem, i) && test_bit(ena, i))
          {
            //printf("Execute interupt: %u\n", i);
            execute_interupt(i);
          }
        }
    }
  }
}

void execute_interupt(uint8_t i)
{
  MMU.HALT = 0;
  r.ime = 0;
  uint8_t mem = read_memory(0xFF0F);
  mem &= ~(1 << i);
  write_memory(0xFF0F, mem);
  push_stack(r.PC.val);
  switch (i)
  {
    case 0: r.PC.val = 0x40; break;
    case 1: r.PC.val = 0x48; break;
    case 2: r.PC.val = 0x50; break;
    case 4: r.PC.val = 0x60; break;
  }
}
