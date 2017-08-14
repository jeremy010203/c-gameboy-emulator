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
  MMU.MEMORY_MODEL = 1;
  MMU.BIOS_MODE = 1;
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
    printf("%x - %x\n", i, MMU.memory[i]);
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
    new_addr += ((MMU.CUR_ROM - 1) * 0x4000);
    return MMU.game[new_addr];
  }
  else if ((addr >= 0xA000) && (addr <= 0xBFFF))
  {
    unsigned int addr2 = (addr - 0xA000) + (MMU.CUR_RAM * 0x2000);
    return MMU.ram[addr2];
  }

  // Catch joypad request
  if (addr == 0xFF00)
  {
    return get_joypad();
  }
  return MMU.memory[addr];
}

void write_memory(uint16_t addr, uint8_t val)
{
  if (MMU.BIOS_MODE)
  {
    if (addr == 0xFF50 && val == 1)
   {
     load_rom(MMU.path_rom);
     MMU.BIOS_MODE = 0;
   }
   MMU.memory[addr] = val;
   return;
  }

  if (addr < 0x2000)
  {
      if (MMU.MBC1)
	    {
            if ((val & 0xF) == 0xA)
                MMU.ENABLE_RAM = 1;
            else if (val == 0x0)
                MMU.ENABLE_RAM = 0;
	    }
	    else if (MMU.MBC2)
	    {
        //bit 0 of upper byte must be 0
	     if (0 == test_bit(addr >> 8, 0))
	     {
	         if ((val & 0xF) == 0xA)
                MMU.ENABLE_RAM = 1;
            else if (val == 0x0)
                MMU.ENABLE_RAM = 0;
	     }
	    }
    }
   else if ((addr >= 0x2000) && (addr < 0x4000))
   {
     if (MMU.MBC1)
    {
      if (val == 0x00)
        val++;

      val &= 31;

      // Turn off the lower 5-bits.
      MMU.CUR_ROM &= 224;

      // Combine the written data with the register.
      MMU.CUR_ROM |= val;
    }
    else if (MMU.MBC2)
    {
      val &= 0xF;
      MMU.CUR_ROM = val;
    }
   }
   else if (((addr >= 0x4000) && (addr < 0x6000)))
   {
     if (MMU.MBC1)
    {
      // are we using memory model 16/8
      if (MMU.MEMORY_MODEL)
      {
        // in this mode we can only use Ram Bank 0
        MMU.CUR_RAM = 0 ;

        val &= 3;
        val <<= 5;

        if ((MMU.CUR_ROM & 31) == 0)
        {
          val++;
        }

        // Turn off bits 5 and 6, and 7 if it somehow got turned on.
        MMU.CUR_ROM &= 31;

        // Combine the written data with the register.
        MMU.CUR_ROM |= val;

      }
      else
      {
        MMU.CUR_RAM = (val & 0x3);
      }
    }
   }
   else if (((addr >= 0x6000) && (addr < 0x8000)))
   {
     if (MMU.MBC1)
    {
      // we're only interested in the first bit
      val &= 1 ;
      if (val == 1)
      {
        MMU.CUR_RAM = 0 ;
        MMU.MEMORY_MODEL = 0;
      }
      else
      {
        MMU.MEMORY_MODEL = 1;
      }
    }
   }
 else if (((addr >= 0xA000) && (addr < 0xC000)))
  {
    if (MMU.ENABLE_RAM)
 		{
 		    if (MMU.MBC1)
 		    {
            uint16_t newAddress = (addr - 0xA000);
            MMU.ram[newAddress + (MMU.CUR_RAM * 0x2000)] = val;
 		    }
 		}
 		else if (MMU.MBC2 && (addr < 0xA200))
 		{
 		    uint16_t newAddress = (addr - 0xA000);
        MMU.ram[newAddress + (MMU.CUR_RAM * 0x2000)] = val;
 		}
  }
  // we're right to internal RAM, remember that it needs to echo it
	else if ((addr >= 0xC000) && (addr <= 0xDFFF))
	{
		MMU.memory[addr] = val;
	}
  else if ((addr >= 0xE000) && (addr < 0xFE00))
  {
    MMU.memory[addr] = val;
    MMU.memory[addr - 0x2000] = val;
  }

  // Read only
  else if ((addr >= 0xFEA0) && (addr <= 0xFEFF))
  {

  }
  else if (addr == 0xFF04) // Divider register
  {
    MMU.memory[addr] = 0;
    my_clock.divider = 0;
  }
  else if (addr == 0xFF44) // Divider register
  {
    MMU.memory[addr] = 0;
  }
  else if (addr == 0xFF07) // TMC reg
  {
    MMU.memory[addr] = val;
    uint8_t timer = (val & 0x03);
    int clock_speed = 0;
    switch(timer)
    {
      case 0: clock_speed = 1024; break;
      case 1: clock_speed = 16; break;
      case 2: clock_speed = 64; break;
      case 3: clock_speed = 256; break;
    }

    if (my_clock.timer_counter != clock_speed)
    {
      my_clock.timer_counter = 0;
      my_clock.clock_speed= clock_speed;
    }
  }
  else if (addr == 0xFF46)
  {
    uint16_t address = val << 8;
    for (int i = 0 ; i < 0xA0; i++)
    {
      MMU.memory[0xFE00 + i] = MMU.memory[address + i];
    }
  }
  else
  {
      MMU.memory[addr] = val;
  }
}

void request_interupt(uint8_t val)
{
  // printf("Request interupt: %u\n", val);
  uint8_t mem = MMU.memory[0xFF0F];
  mem |= (1 << val);
  MMU.memory[0xFF0F] =  mem;
}

void do_interupt(void)
{
  if (r.ime)
  {
    uint8_t mem = MMU.memory[0xFF0F];
    uint8_t ena = MMU.memory[0xFFFF];
    if (mem > 0)
    {
        for (uint8_t i = 0; i < 5; i++)
        {
          if (test_bit(mem, i) && test_bit(ena, i))
          {
            // printf("Execute interupt: %u\n", i);
            execute_interupt(i);
            return;
          }
        }
    }
  }
}

void execute_interupt(uint8_t i)
{
  MMU.HALT = 0;
  r.ime = 0;

  uint8_t mem = MMU.memory[0xFF0F];
  mem &= ~(1 << i);
  MMU.memory[0xFF0F] = mem;

  push_stack(r.PC.val);
  switch (i)
  {
    case 0: r.PC.val = 0x40; break;
    case 1: r.PC.val = 0x48; break;
    case 2: r.PC.val = 0x50; break;
    case 4: r.PC.val = 0x60; break;
  }
}
