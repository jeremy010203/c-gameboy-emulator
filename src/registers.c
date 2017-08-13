#include "registers.h"

void init_registers()
{
  r.AF.val = 0;
  r.BC.val = 0;
  r.DE.val = 0;
  r.HL.val = 0;
  r.SP.val = 0;
  r.PC.val = 0;
  r.ime = 1;
  r.joypad = 0xFF;
  my_clock.total_m = 0;
  my_clock.total_t = 0;
  my_clock.lineticks = 0;
  my_clock.mode = 2;
  my_clock.divider = 0;
}

void print_r()
{
    printf("---Registers---\n");
    printf("A: %x | F: %x\n", r.AF.bytes.high, r.AF.bytes.low);
    printf("B: %x | C: %x\n", r.BC.bytes.high, r.BC.bytes.low);
    printf("D: %x | E: %x\n", r.DE.bytes.high, r.DE.bytes.low);
    printf("H: %x | L: %x\n", r.HL.bytes.high, r.HL.bytes.low);
    printf("SP: %x\n", r.SP.val);
    printf("PC: %x\n", r.PC.val);
    printf("PAD: %x\n", r.joypad);
    printf("---------------\n");
}

void setZ(void)
{
  r.AF.bytes.low |= 0b10000000;
}

void resetZ(void)
{
  r.AF.bytes.low &= 0b01111111;
}

void setN(void)
{
  r.AF.bytes.low |= 0b01000000;
}

void resetN(void)
{
  r.AF.bytes.low &= 0b10111111;
}

void setH(void)
{
  r.AF.bytes.low |= 0b00100000;
}

void resetH(void)
{
  r.AF.bytes.low &= 0b11011111;
}

void setC(void)
{
  r.AF.bytes.low |= 0b00010000;
}

void resetC(void)
{
  r.AF.bytes.low &= 0b11101111;
}

// Get Z flag
uint8_t getZ(void)
{
  return (r.AF.bytes.low & 0b10000000) >> 7;
}

uint8_t getN(void)
{
  return (r.AF.bytes.low & 0b01000000) >> 6;
}

uint8_t getH(void)
{
  return (r.AF.bytes.low & 0b00100000) >> 5;
}

uint8_t getC(void)
{
  return (r.AF.bytes.low & 0b00010000) >> 4;
}

void loadba()
{
  load(&r.BC.bytes.high, r.AF.bytes.high);
}

void loadbb()
{
  load(&r.BC.bytes.high, r.BC.bytes.high);
}

void loadbc()
{
  load(&r.BC.bytes.high, r.BC.bytes.low);
}

void loadbd()
{
  load(&r.BC.bytes.high, r.DE.bytes.high);
}

void loadbe()
{
  load(&r.BC.bytes.high, r.DE.bytes.low);
}

void loadbh()
{
  load(&r.BC.bytes.high, r.HL.bytes.high);
}

void loadbl()
{
  load(&r.BC.bytes.high, r.HL.bytes.low);
}

void loadca()
{
  load(&r.BC.bytes.low, r.AF.bytes.high);
}

void loadcb()
{
  load(&r.BC.bytes.low, r.BC.bytes.high);
}

void loadcc()
{
  load(&r.BC.bytes.low, r.BC.bytes.low);
}

void loadcd()
{
  load(&r.BC.bytes.low, r.DE.bytes.high);
}

void loadce()
{
  load(&r.BC.bytes.low, r.DE.bytes.low);
}

void loadch()
{
  load(&r.BC.bytes.low, r.HL.bytes.high);
}

void loadcl()
{
  load(&r.BC.bytes.low, r.HL.bytes.low);
}

void loadda()
{
  load(&r.DE.bytes.high, r.AF.bytes.high);
}

void loaddb()
{
  load(&r.DE.bytes.high, r.BC.bytes.high);
}

void loaddc()
{
  load(&r.DE.bytes.high, r.BC.bytes.low);
}

void loaddd()
{
  load(&r.DE.bytes.high, r.DE.bytes.high);
}

void loadde()
{
  load(&r.DE.bytes.high, r.DE.bytes.low);
}

void loaddh()
{
  load(&r.DE.bytes.high, r.HL.bytes.high);
}

void loaddl()
{
  load(&r.DE.bytes.high, r.HL.bytes.low);
}

void loadea()
{
  load(&r.DE.bytes.low, r.AF.bytes.high);
}

void loadeb()
{
  load(&r.DE.bytes.low, r.BC.bytes.high);
}

void loadec()
{
  load(&r.DE.bytes.low, r.BC.bytes.low);
}

void loaded()
{
  load(&r.DE.bytes.low, r.DE.bytes.high);
}

void loadee()
{
  load(&r.DE.bytes.low, r.DE.bytes.low);
}

void loadeh()
{
  load(&r.DE.bytes.low, r.HL.bytes.high);
}

void loadel()
{
  load(&r.DE.bytes.low, r.HL.bytes.low);
}

void loadha()
{
  load(&r.HL.bytes.high, r.AF.bytes.high);
}

void loadhb()
{
  load(&r.HL.bytes.high, r.BC.bytes.high);
}

void loadhc()
{
  load(&r.HL.bytes.high, r.BC.bytes.low);
}

void loadhd()
{
  load(&r.HL.bytes.high, r.DE.bytes.high);
}

void loadhe()
{
  load(&r.HL.bytes.high, r.DE.bytes.low);
}

void loadhh()
{
  load(&r.HL.bytes.high, r.HL.bytes.high);
}

void loadhl()
{
  load(&r.HL.bytes.high, r.HL.bytes.low);
}

void loadla()
{
  load(&r.HL.bytes.low, r.AF.bytes.high);
}

void loadlb()
{
  load(&r.HL.bytes.low, r.BC.bytes.high);
}

void loadlc()
{
  load(&r.HL.bytes.low, r.BC.bytes.low);
}

void loadld()
{
  load(&r.HL.bytes.low, r.DE.bytes.high);
}

void loadle()
{
  load(&r.HL.bytes.low, r.DE.bytes.low);
}

void loadlh()
{
  load(&r.HL.bytes.low, r.HL.bytes.high);
}

void loadll()
{
  load(&r.HL.bytes.low, r.HL.bytes.low);
}

void loadaa()
{
  load(&r.AF.bytes.high, r.AF.bytes.high);
}

void loadab()
{
  load(&r.AF.bytes.high, r.BC.bytes.high);
}

void loadac()
{
  load(&r.AF.bytes.high, r.BC.bytes.low);
}

void loadad()
{
  load(&r.AF.bytes.high, r.DE.bytes.high);
}

void loadae()
{
  load(&r.AF.bytes.high, r.DE.bytes.low);
}

void loadah()
{
  load(&r.AF.bytes.high, r.HL.bytes.high);
}

void loadal()
{
  load(&r.AF.bytes.high, r.HL.bytes.low);
}

// Test if but = 1 with index: 7 6 5 4 3 2 1 0
int test_bit(const uint8_t byte, const uint8_t index)
{
    return ((byte & (1 << index)) != 0);
}
