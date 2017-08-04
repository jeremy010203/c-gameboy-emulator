#include "helpers_op.h"

extern Mmu MMU;
extern Registers r;
extern My_clock my_clock;
extern void (*Opcodes[0xFF]) (void);

// Read a byte in memory and increment PC
uint8_t read_byte(void)
{
  return read_memory(r.PC.val++);
}

// Read a word in memory and increment PC by 2
uint16_t read_word(void)
{
  uint8_t b1 = read_byte();
  uint16_t b2 = read_byte();
  b2 <<= 8;
  return b2 | b1;
}

// Read a byte in memory without increment PC
uint8_t peak_byte(void)
{
  return read_memory(r.PC.val);
}

void push_stack(const uint16_t val)
{
  write_memory(r.SP.val--, (uint8_t)(val >> 8));
  write_memory(r.SP.val--, (uint8_t)((val << 8) >> 8));
}

uint16_t pop_stack(void)
{
  uint8_t v1 = read_memory(++r.SP.val);
  uint16_t v2 = read_memory(++r.SP.val);
  return (v2 << 8) | v1;
}

void inc_op(uint8_t *reg)
{
  uint8_t result = *reg + 1;

  result == 0 ? setZ() : resetZ();
  (((*reg & 0xF) + (uint8_t)1) & 0x10) ? setH() : resetH();
  resetN();

  *reg = result;
  my_clock.m = 1;
  my_clock.t = 4;
}

void dec_op(uint8_t *reg)
{
  uint8_t result = *reg - 1;
  *reg == 0 ? setC() : resetC();

  result == 0 ? setZ() : resetZ();
  ((*reg & 0xF) - (uint8_t)1) < 0 ? setH() : resetH();
  setN();

  *reg = result;
  my_clock.m = 1;
  my_clock.t = 4;
}

void pop_op(uint16_t *reg)
{
  *reg = pop_stack();

  my_clock.m = 1;
  my_clock.t = 12;
}

void push_op(const uint16_t reg)
{
  push_stack(reg);

  my_clock.m = 1;
  my_clock.t = 16;
}

void rst_op(const uint16_t addr)
{
  push_stack(r.PC.val);
  r.PC.val = addr;

  my_clock.m = 1;
  my_clock.t = 16;
}

void swap_op(uint8_t *reg)
{
  uint8_t high = *reg >> 4;
  *reg <<= 4;
  *reg += high;

  *reg == 0 ? setZ() : resetZ();
  resetC();
  resetN();
  resetH();

  my_clock.m = 2;
  my_clock.t = 8;
}

void add_8_op(uint8_t *first, const uint8_t second)
{
  uint8_t result = *first + second;
  ((uint16_t)*first + (uint16_t)second > 255) ? setC() : resetC();

  result == 0 ? setZ() : resetZ();
  (((*first & 0xF) + (second & 0xF)) & 0x10) ? setH() : resetH();
  resetN();

  *first = result;
  my_clock.m = 1;
  my_clock.t = 4;
}

void add_16_op(uint16_t *first, const uint16_t second)
{
  (((uint32_t)*first + (uint32_t)second) > 0xFFFF) ? setC() : resetC();
  resetN();

  *first += second;
  my_clock.m = 1;
  my_clock.t = 8;
}

void sub_8_op(uint8_t *first, const uint8_t second)
{
  uint8_t result = *first - second;
  (*first < second) ? setC() : resetC();

  result == 0 ? setZ() : resetZ();
  ((*first & 0xF) - (second & 0xF)) < 0 ? setH() : resetH();
  setN();

  *first = result;
  my_clock.m = 1;
  my_clock.t = 4;
}

void xor_8_op(uint8_t *first, const uint8_t second)
{
  *first ^= second;
  *first == 0 ? setZ() : resetZ();
  resetH();
  resetN();
  resetC();

  my_clock.m = 1;
  my_clock.t = 4;
}

void and_op(uint8_t *first, const uint8_t second)
{
  *first &= second;
  *first == 0 ? setZ() : resetZ();
  setH();
  resetN();
  resetC();

  my_clock.m = 1;
  my_clock.t = 4;
}

void or_op(uint8_t *first, const uint8_t second)
{
  *first |= second;
  *first == 0 ? setZ() : resetZ();
  setH();
  resetN();
  resetC();

  my_clock.m = 1;
  my_clock.t = 4;
}

void cp_op(const uint8_t first, const uint8_t second)
{
  uint8_t tmp = first;
  sub_8_op(&tmp, second);

  my_clock.m = 1;
  my_clock.t = 4;
}

void ret_cond_op(int cond)
{
  if (cond)
  {
    r.PC.val = pop_stack();
    my_clock.t = 16;
  }
  else
  {
    my_clock.t = 8;
  }

  my_clock.m = 1;
}
