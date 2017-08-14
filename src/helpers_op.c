#include "helpers_op.h"

extern Mmu MMU;
extern Registers r;
extern My_clock my_clock;
extern void (*Opcodes[0x100]) (void);
extern void (*PrefixCB[0x100]) (void);

// Read a byte in memory and increment PC
uint8_t read_byte(void)
{
  uint8_t tmp = read_memory(r.PC.val);
  r.PC.val += 1;
  return tmp;
}

// Read a word in memory and increment PC by 2
uint16_t read_word(void)
{
  uint16_t b1 = read_byte();
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
  r.SP.val -= 1;
  write_memory(r.SP.val, val >> 8);
  r.SP.val -= 1;
  write_memory(r.SP.val, val & 0xFF);
}

uint16_t pop_stack(void)
{
  uint16_t v1 = ((((uint16_t)read_memory(r.SP.val + 1)) << 8) | ((uint16_t)read_memory(r.SP.val)));
  r.SP.val += 2;
  return v1;
}

void inc_op(uint8_t *reg)
{
  uint8_t result = *reg + 1;

  (result == 0) ? setZ() : resetZ();
  ((result & 0xF) == 0) ? setH() : resetH();
  resetN();

  *reg = result;
  my_clock.m = 1;
  my_clock.t = 4;
}

void dec_op(uint8_t *reg)
{
  uint8_t result = *reg - 1;

  (result == 0) ? setZ() : resetZ();
  ((result & 0xF) == 0xF) ? setH() : resetH();
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
  *reg = ((*reg >> 4) | (*reg << 4));

  (*reg == 0) ? setZ() : resetZ();
  resetC();
  resetN();
  resetH();

  my_clock.m = 2;
  my_clock.t = 8;
}

void adc_op(uint8_t *first, const uint8_t second)
{
  uint8_t result = *first + second + getC();
  (((*first & 0xF) + (second & 0xF) + getC()) > 0xF) ? setH() : resetH();
  (((uint16_t)*first + (uint16_t)second + (uint16_t)getC()) > 0xFF) ? setC() : resetC();

  result == 0 ? setZ() : resetZ();
  resetN();

  *first = result;
  my_clock.m = 1;
  my_clock.t = 4;
}

void add_8_op(uint8_t *first, const uint8_t second)
{
  uint8_t result = *first + second;
  (((uint16_t)*first + (uint16_t)second) > 255) ? setC() : resetC();

  result == 0 ? setZ() : resetZ();
  (((*first & 0xF) + (second & 0xF)) & 0x10) ? setH() : resetH();
  resetN();

  *first = result;
  my_clock.m = 1;
  my_clock.t = 4;
}

void add_16_op(uint16_t *first, const uint16_t second)
{
  uint32_t result = ((uint32_t)*first + (uint32_t)second);
  ((*first & 0xFFF) > (result & 0xFFF)) ? setH() : resetH();
  (result > 0xFFFF) ? setC() : resetC();
  resetN();

  *first = (result & 0xFFFF);

  my_clock.m = 1;
  my_clock.t = 8;
}

void sub_8_op(uint8_t *first, const uint8_t second)
{
  uint8_t result = (*first - second);
  (*first < second) ? setC() : resetC();

  (result == 0) ? setZ() : resetZ();

  uint8_t testf = (result & 0xF);
  uint8_t tests = (*first & 0xF);
  (testf > tests) ? setH() : resetH();
  setN();

  *first = result;
  my_clock.m = 1;
  my_clock.t = 4;
}

void xor_8_op(uint8_t *first, const uint8_t second)
{
  *first ^= second;
  (*first == 0) ? setZ() : resetZ();
  resetH();
  resetN();
  resetC();

  my_clock.m = 1;
  my_clock.t = 4;
}

void and_op(uint8_t *first, const uint8_t second)
{
  *first &= second;
  (*first == 0) ? setZ() : resetZ();
  resetN();
  setH();
  resetC();

  my_clock.m = 1;
  my_clock.t = 4;
}

void or_op(uint8_t *first, const uint8_t second)
{
  *first |= second;
  (*first == 0) ? setZ() : resetZ();
  resetN();
  resetH();
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

void load(uint8_t *to, const uint8_t from)
{
  if (!to)
  {
    printf("Error load -> (to:%p)", to);
    exit(1);
  }
  *to = from;

  my_clock.m = 1;
  my_clock.t = 4;
}

void bit_op(const uint8_t reg, const uint8_t pos)
{
  !test_bit(reg, pos) ? setZ() : resetZ();
  resetN();
  setH();

  my_clock.m = 2;
  my_clock.t = 8;
}

void res_op(uint8_t *reg, const uint8_t pos)
{
  *reg &= ~(1 << pos);

  my_clock.m = 2;
  my_clock.t = 8;
}

void set_op(uint8_t *reg, const uint8_t pos)
{
  *reg |= (1 << pos);

  my_clock.m = 2;
  my_clock.t = 8;
}

void sla_op(uint8_t *reg)
{
  (*reg >> 7) ? setC() : resetC();
  *reg <<= 1;
  (*reg == 0) ? setZ() : resetZ();
  resetN();
  resetH();

  my_clock.m = 2;
  my_clock.t = 8;
}

void srl_op(uint8_t *reg)
{
  ((*reg & 0x01) == 0x01) ? setC() : resetC();
  *reg >>= 1;
  (*reg == 0) ? setZ() : resetZ();
  resetN();
  resetH();

  my_clock.m = 2;
  my_clock.t = 8;
}

void rl_op(uint8_t *reg)
{
  uint8_t carry = (*reg > 0x7F);
  *reg = (((*reg << 1) & 0xFF) | getC());

  carry ? setC() : resetC();
  (*reg == 0) ? setZ() : resetZ();
  resetN();
  resetH();

  my_clock.m = 2;
  my_clock.t = 8;
}

void rr_op(uint8_t *reg)
{
  uint8_t old_0 = ((*reg & 0x01) == 0x01);
  *reg = ((getC() ? 0x80 : 0) | (*reg >> 1));

  old_0 ? setC() : resetC();
  (*reg == 0) ? setZ() : resetZ();
  resetN();
  resetH();

  my_clock.m = 2;
  my_clock.t = 8;
}

void sbc_op(uint8_t *first, const uint8_t second)
{
  uint8_t result = (*first - second - getC());

  uint16_t testf = (*first & 0xF);
  uint16_t tests = (second & 0xF);
  (testf < (tests + getC())) ? setH() : resetH();
  (*first < (second + getC())) ? setC() : resetC();

  (result == 0) ? setZ() : resetZ();
  setN();

  *first = result;
  my_clock.m = 1;
  my_clock.t = 4;
}

void sra_op(uint8_t *reg)
{
  ((*reg & 0x01) == 0x01) ? setC() : resetC();
  *reg = ((*reg & 0x80) | (*reg >> 1));

  (*reg == 0) ? setZ() : resetZ();
  resetN();
  resetH();

  my_clock.m = 2;
  my_clock.t = 8;
}

void rlc_op(uint8_t *reg)
{
  (*reg > 0x7F) ? setC() : resetC();
  *reg = (((*reg << 1) & 0xFF) | getC());
  (*reg == 0) ? setZ() : resetZ();
  resetH();
  resetN();

  my_clock.m = 2;
  my_clock.t = 8;
}

void rrc_op(uint8_t *reg)
{
  ((*reg & 0x01) == 0x01) ? setC() : resetC();
	*reg = ((getC() ? 0x80 : 0) | (*reg >> 1));

  (*reg == 0) ? setZ() : resetZ();
  resetH();
  resetN();

  my_clock.m = 2;
  my_clock.t = 8;
}
