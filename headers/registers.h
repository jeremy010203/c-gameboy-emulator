#ifndef REGISTERS_H
# define REGISTERS_H

#include <stdlib.h>
#include <stdio.h>
#include "helpers_op.h"

struct RegisterByte
{
  uint8_t low;
  uint8_t high;
};

typedef union Register
{
  uint16_t val;
  struct RegisterByte bytes;
} Register;

typedef struct Registers
{
  Register AF;
  Register BC;
  Register DE;
  Register HL;

  Register SP;
  Register PC;
  uint8_t ime;
  uint8_t joypad;
} Registers;

typedef struct my_clock
{
  uint16_t m;
  uint16_t t;
  uint32_t total_m;
  uint32_t total_t;
  uint8_t mode;
  uint16_t lineticks;
  uint16_t divider;
} My_clock;

My_clock my_clock;
void (*Opcodes[0x100]) (void);
void (*PrefixCB[0x100]) (void);
Registers r;

void init_registers(void);
void print_r(void);
int test_bit(const uint8_t byte, const uint8_t index);

void setZ(void);
void resetZ(void);
void setN(void);
void resetN(void);
void setH(void);
void resetH(void);
void setC(void);
void resetC(void);

uint8_t getZ(void);
uint8_t getN(void);
uint8_t getH(void);
uint8_t getC(void);

// B register load instructions
void loadba(void);
void loadbb(void);
void loadbc(void);
void loadbd(void);
void loadbe(void);
void loadbh(void);
void loadbl(void);

// C register load instructions
void loadca(void);
void loadcb(void);
void loadcc(void);
void loadcd(void);
void loadce(void);
void loadch(void);
void loadcl(void);

// D register load instructions
void loadda(void);
void loaddb(void);
void loaddc(void);
void loaddd(void);
void loadde(void);
void loaddh(void);
void loaddl(void);

// E register load instructions
void loadea(void);
void loadeb(void);
void loadec(void);
void loaded(void);
void loadee(void);
void loadeh(void);
void loadel(void);

// H register load instructions
void loadha(void);
void loadhb(void);
void loadhc(void);
void loadhd(void);
void loadhe(void);
void loadhh(void);
void loadhl(void);

// L register load instructions
void loadla(void);
void loadlb(void);
void loadlc(void);
void loadld(void);
void loadle(void);
void loadlh(void);
void loadll(void);

// A register load instructions
void loadaa(void);
void loadab(void);
void loadac(void);
void loadad(void);
void loadae(void);
void loadah(void);
void loadal(void);

#endif /* REGISTERS_H */
