#include "utils.h"

void init(void)
{
    init_registers();
    load_opcodes();
    load_prefixcb();
    init_mmu("misc/bios.bin");
}

// Read a byte in memory and increment PC
uint8_t read_byte(void)
{
  return MMU.memory[r.PC.val++];
}

// Read a word in memory and increment PC by 2
uint16_t read_word(void)
{
  uint16_t b1 = MMU.memory[r.PC.val++];
  uint8_t b2 = MMU.memory[r.PC.val++];
  b1 <<= 8;
  return b1 | b2;
}

void prefixcb(void)
{
    uint8_t op = read_byte();
    if (!PrefixCB[op])
    {
      fprintf(stderr, "Unknown Prefix Op: %x", op);
      exit(1);
    }
    PrefixCB[op]();
}

void loadspd16(void)
{
  r.SP.val = read_word();

  clock.m = 3;
  clock.t = 12;
}

void loadhld16(void)
{
  r.HL.val = read_word();

  clock.m = 3;
  clock.t = 12;
}

void loadhlpa(void)
{
  MMU.memory[r.HL.val] = r.AF.bytes.high;
  r.HL.val++;

  clock.m = 1;
  clock.t = 8;
}

void loadhlma(void)
{
  MMU.memory[r.HL.val] = r.AF.bytes.high;
  r.HL.val--;

  clock.m = 1;
  clock.t = 8;
}

void jrnz(void)
{
  int8_t addr = read_byte();
  if (getZ())
  {
    r.PC.val += addr;
    clock.t = 12;
  }
  else
  {
    clock.t = 8;
  }

  clock.m = 2;
}

void loadcd8(void)
{
  r.BC.bytes.low = read_byte();
  clock.m = 2;
  clock.t = 8;
}

void loaded8(void)
{
  r.DE.bytes.low = read_byte();
  clock.m = 2;
  clock.t = 8;
}

void loadld8(void)
{
  r.HL.bytes.low = read_byte();
  clock.m = 2;
  clock.t = 8;
}

void loadad8(void)
{
  r.AF.bytes.high = read_byte();
  clock.m = 2;
  clock.t = 8;
}

void opcode_0xe2(void)
{
  uint16_t pos = 0xFF00 + r.BC.bytes.low;
  MMU.memory[pos] = r.AF.bytes.high;

  clock.m = 2;
  clock.t = 8;
}

void opcode_0x0c(void)
{
  if((r.BC.bytes.low & 0x0F + 0x01 & 0x0F) & 0xF0)
    setH();
  else
    resetH();

  r.BC.bytes.low++;
  if (r.BC.bytes.low == 0)
    setZ();
  else
    resetZ();
  resetN();

  clock.m = 1;
  clock.t = 4;
}

void load_opcodes(void)
{
  Opcodes[0x00] = &nop;

  Opcodes[0x0c] = &opcode_0x0c;
  Opcodes[0x0e] = &loadcd8;
  Opcodes[0x1e] = &loaded8;
  Opcodes[0x2e] = &loadld8;
  Opcodes[0x3e] = &loadad8;

  Opcodes[0x20] = &jrnz;
  Opcodes[0x21] = &loadhld16;
  Opcodes[0x22] = &loadhlpa;
  Opcodes[0x31] = &loadspd16;
  Opcodes[0x32] = &loadhlma;

  Opcodes[0x40] = &loadbb;
  Opcodes[0x41] = &loadbc;
  Opcodes[0x42] = &loadbd;
  Opcodes[0x43] = &loadbe;
  Opcodes[0x44] = &loadbh;
  Opcodes[0x45] = &loadbl;
  Opcodes[0x47] = &loadba;

  Opcodes[0x48] = &loadcb;
  Opcodes[0x49] = &loadcc;
  Opcodes[0x4A] = &loadcd;
  Opcodes[0x4B] = &loadce;
  Opcodes[0x4C] = &loadch;
  Opcodes[0x4D] = &loadcl;
  Opcodes[0x4F] = &loadca;

  Opcodes[0x50] = &loaddb;
  Opcodes[0x51] = &loaddc;
  Opcodes[0x52] = &loaddd;
  Opcodes[0x53] = &loadde;
  Opcodes[0x54] = &loaddh;
  Opcodes[0x55] = &loaddl;
  Opcodes[0x57] = &loadda;

  Opcodes[0x58] = &loadeb;
  Opcodes[0x59] = &loadec;
  Opcodes[0x5A] = &loaded;
  Opcodes[0x5B] = &loadee;
  Opcodes[0x5C] = &loadeh;
  Opcodes[0x5D] = &loadel;
  Opcodes[0x5F] = &loadea;

  Opcodes[0x60] = &loadhb;
  Opcodes[0x61] = &loadhc;
  Opcodes[0x62] = &loadhd;
  Opcodes[0x63] = &loadhe;
  Opcodes[0x64] = &loadhh;
  Opcodes[0x65] = &loadhl;
  Opcodes[0x67] = &loadha;

  Opcodes[0x68] = &loadlb;
  Opcodes[0x69] = &loadlc;
  Opcodes[0x6A] = &loadld;
  Opcodes[0x6B] = &loadle;
  Opcodes[0x6C] = &loadlh;
  Opcodes[0x6D] = &loadll;
  Opcodes[0x6F] = &loadla;

  Opcodes[0x78] = &loadab;
  Opcodes[0x79] = &loadac;
  Opcodes[0x7A] = &loadad;
  Opcodes[0x7B] = &loadae;
  Opcodes[0x7C] = &loadah;
  Opcodes[0x7D] = &loadal;
  Opcodes[0x7F] = &loadaa;

  Opcodes[0xAF] = &xora;
  Opcodes[0xCB] = &prefixcb;
  Opcodes[0xE2] = &opcode_0xe2;
}

void load_prefixcb(void)
{
  PrefixCB[0x7C] = &bit7h;
}

void execute(uint16_t op)
{
  if (!Opcodes[op])
  {
    fprintf(stderr, "Unknown Op: %x", op);
    exit(1);
  }
  Opcodes[op]();
}
