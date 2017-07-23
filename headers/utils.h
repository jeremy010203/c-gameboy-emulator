#ifndef UTILS_H
# define UTILS_H

#include "mmu.h"
#include "registers.h"

extern Mmu MMU;
extern Registers r;
extern Clock clock;
extern void (*Opcodes[0xFF]) (void);

enum Op
{
  NOP       = 0x00,
  LOADSPd16 = 0x31,
  LOADBB    = 0x40,
  LOADBC    = 0x41,
  LOADBD    = 0x42,
  LOADBE    = 0x43,
  LOADBH    = 0x44,
  LOADBL    = 0x45,
  LOADBHL   = 0x46,
  LOADBA    = 0x47,
  LOADDB    = 0x50,
  LOADDC    = 0x51,
  LOADDD    = 0x52,
  LOADDE    = 0x53,
  LOADDH    = 0x54,
  LOADDL    = 0x55,
  LOADDHL   = 0x56,
  LOADDA    = 0x57,
  LOADHB    = 0x60,
  LOADHC    = 0x61,
  LOADHD    = 0x62,
  LOADHE    = 0x63,
  LOADHH    = 0x64,
  LOADHL    = 0x65,
  LOADHHL   = 0x66,
  LOADHA    = 0x67,
  LOADHLB   = 0x70,
  LOADHLC   = 0x71,
  LOADHLD   = 0x72,
  LOADHLE   = 0x73,
  LOADHLH   = 0x74,
  LOADHLL   = 0x75,
  HALT      = 0x76,
  LOADHLA   = 0x77,
  XORA      = 0xAF,
};

uint8_t read_byte(void);
uint16_t read_word(void);
uint8_t peak_byte(void);
void init(void);
void load_opcodes(void);
void load_prefixcb(void);
void execute(uint16_t op);

void loadhlpa(void);
void loadhlma(void);
void opcode_0x01(void);
void opcode_0x11(void);
void opcode_0x21(void);
void opcode_0x31(void);
void loadspd16(void);
void prefixcb(void);
void jrnz(void);
void loadcd8(void);
void loaded8(void);
void loadld8(void);
void loadad8(void);
void opcode_0x0c(void);
void opcode_0xe2(void);

#endif
