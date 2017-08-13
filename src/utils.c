#include "utils.h"

extern Mmu MMU;
extern Registers r;
extern My_clock my_clock;
extern void (*Opcodes[0x100]) (void);
extern void (*PrefixCB[0x100]) (void);

void init(void)
{
    init_registers();
    load_opcodes();
    load_prefixcb();
    init_mmu("misc/bios.bin");
}

// NOP
void opcode_0x00(void) { my_clock.m = 1; my_clock.t = 4; }

// POP OPS
void opcode_0xc1(void) { pop_op(&r.BC.val); }
void opcode_0xd1(void) { pop_op(&r.DE.val); }
void opcode_0xe1(void) { pop_op(&r.HL.val); }
void opcode_0xf1(void) { pop_op(&r.AF.val); r.AF.val &= 0xFFF0; }

// PUSH OPS
void opcode_0xc5(void) { push_op(r.BC.val); }
void opcode_0xd5(void) { push_op(r.DE.val); }
void opcode_0xe5(void) { push_op(r.HL.val); }
void opcode_0xf5(void) { push_op(r.AF.val); }

// CALL Z, a16
void opcode_0xcc(void)
{
  uint16_t addr = read_word();
  if (getZ())
  {
    push_stack(r.PC.val);
    r.PC.val = addr;
    my_clock.t = 24;
  }
  else
  {
    my_clock.t = 12;
  }
  my_clock.m = 3;
}

// CALL NZ, a16
void opcode_0xc4(void)
{
  uint16_t addr = read_word();
  if (!getZ())
  {
    push_stack(r.PC.val);
    r.PC.val = addr;
    my_clock.t = 24;
  }
  else
  {
    my_clock.t = 12;
  }
  my_clock.m = 3;
}

// CALL C, a16
void opcode_0xdc(void)
{
  uint16_t addr = read_word();
  if (getC())
  {
    push_stack(r.PC.val);
    r.PC.val = addr;
    my_clock.t = 24;
  }
  else
  {
    my_clock.t = 12;
  }
  my_clock.m = 3;
}

// CALL a16
void opcode_0xcd(void)
{
  uint16_t addr = read_word();
  push_stack(r.PC.val);
  r.PC.val = addr;

  my_clock.m = 3;
  my_clock.t = 24;
}

void prefixcb(void)
{
    uint8_t op = read_byte();
    if (!PrefixCB[op])
    {
      fprintf(stderr, "Unknown Prefix Op: 0x%x at 0x%x", op, r.PC.val - 1);
      exit(1);
    }
    PrefixCB[op]();
}

void loadhlpa(void)
{
  write_memory(r.HL.val, r.AF.bytes.high);
  r.HL.val++;

  my_clock.m = 1;
  my_clock.t = 8;
}

// LD (BC), A
void opcode_0x02(void)
{
  write_memory(r.BC.val, r.AF.bytes.high);

  my_clock.m = 1;
  my_clock.t = 8;
}

// LD (DE), A
void opcode_0x12(void)
{
  write_memory(r.DE.val, r.AF.bytes.high);

  my_clock.m = 1;
  my_clock.t = 8;
}

void loadhlma(void)
{
  write_memory(r.HL.val, r.AF.bytes.high);
  r.HL.val--;

  my_clock.m = 1;
  my_clock.t = 8;
}

// JR NZ, r8
void opcode_0x20(void)
{
  int8_t addr = read_byte();
  if (!getZ())
  {
    r.PC.val += addr;
    my_clock.t = 12;
  }
  else
  {
    my_clock.t = 8;
  }

  my_clock.m = 2;
}

void loadcd8(void)
{
  r.BC.bytes.low = read_byte();
  my_clock.m = 2;
  my_clock.t = 8;
}

void loaded8(void)
{
  r.DE.bytes.low = read_byte();
  my_clock.m = 2;
  my_clock.t = 8;
}

void loadld8(void)
{
  r.HL.bytes.low = read_byte();
  my_clock.m = 2;
  my_clock.t = 8;
}

void loadad8(void)
{
  r.AF.bytes.high = read_byte();
  my_clock.m = 2;
  my_clock.t = 8;
}

// LOAD (C), A
void opcode_0xe2(void)
{
  uint16_t pos = 0xFF00 + r.BC.bytes.low;
  write_memory(pos, r.AF.bytes.high);

  my_clock.m = 2;
  my_clock.t = 8;
}

// INC OPS
void opcode_0x0c(void) { inc_op(&r.BC.bytes.low); }
void opcode_0x1c(void) { inc_op(&r.DE.bytes.low); }
void opcode_0x2c(void) { inc_op(&r.HL.bytes.low); }
void opcode_0x3c(void) { inc_op(&r.AF.bytes.high); }
void opcode_0x04(void) { inc_op(&r.BC.bytes.high); }
void opcode_0x14(void) { inc_op(&r.DE.bytes.high); }
void opcode_0x24(void) { inc_op(&r.HL.bytes.high); }
void opcode_0x34(void)
{
  uint8_t hl = read_memory(r.HL.val);

  write_memory(r.HL.val, hl + 1);
  read_memory(r.HL.val) == 0 ? setZ() : resetZ();
  ((read_memory(r.HL.val) ^ 0x01 ^ hl) & 0x10) ? setH() : resetH();
  resetN();

  my_clock.m = 1;
  my_clock.t = 12;
}

// LD (HL), B
void opcode_0x70(void)
{
  write_memory(r.HL.val, r.BC.bytes.high);
  my_clock.m = 1;
  my_clock.t = 8;
}

// LD (HL), C
void opcode_0x71(void)
{
  write_memory(r.HL.val, r.BC.bytes.low);
  my_clock.m = 1;
  my_clock.t = 8;
}

// LD (HL), D
void opcode_0x72(void)
{
  write_memory(r.HL.val, r.DE.bytes.high);
  my_clock.m = 1;
  my_clock.t = 8;
}

// LD (HL), E
void opcode_0x73(void)
{
  write_memory(r.HL.val, r.DE.bytes.low);
  my_clock.m = 1;
  my_clock.t = 8;
}

// LD (HL), H
void opcode_0x74(void)
{
  write_memory(r.HL.val, r.HL.bytes.high);
  my_clock.m = 1;
  my_clock.t = 8;
}

// LD (HL), L
void opcode_0x75(void)
{
  write_memory(r.HL.val, r.HL.bytes.low);
  my_clock.m = 1;
  my_clock.t = 8;
}

// LD (HL), A
void opcode_0x77(void)
{
  write_memory(r.HL.val, r.AF.bytes.high);
  my_clock.m = 1;
  my_clock.t = 8;
}

// LD BC, d16
void opcode_0x01(void)
{
  r.BC.val = read_word();
  my_clock.m = 3;
  my_clock.t = 12;
}

// LD (a16), SP
void opcode_0x08(void)
{
  uint16_t addr = read_word();
  write_memory(addr, r.SP.val);

  my_clock.m = 3;
  my_clock.t = 20;
}

// LD DE, d16
void opcode_0x11(void)
{
  r.DE.val = read_word();
  my_clock.m = 3;
  my_clock.t = 12;
}

// LD A,(BC)
void opcode_0x0a(void)
{
  r.AF.bytes.high = read_memory(r.BC.val);
  my_clock.m = 1;
  my_clock.t = 8;
}

// LD A,(DE)
void opcode_0x1a(void)
{
  r.AF.bytes.high = read_memory(r.DE.val);
  my_clock.m = 1;
  my_clock.t = 8;
}

// LD A,(HL+)
void opcode_0x2a(void) { r.AF.bytes.high = read_memory(r.HL.val); r.HL.val++; my_clock.m = 1; my_clock.t = 8; }
void opcode_0x36(void) { write_memory(r.HL.val, read_byte()); my_clock.m = 2; my_clock.t = 12; }
void opcode_0x46(void) { load(&r.BC.bytes.high, read_memory(r.HL.val)); my_clock.m = 1; my_clock.t = 8; }
void opcode_0x56(void) { load(&r.DE.bytes.high, read_memory(r.HL.val)); my_clock.m = 1; my_clock.t = 8; }
void opcode_0x66(void) { load(&r.HL.bytes.high, read_memory(r.HL.val)); my_clock.m = 1; my_clock.t = 8; }

// DEC OPS
void opcode_0x0b(void) { r.BC.val--; my_clock.m = 1; my_clock.t = 8; }
void opcode_0x1b(void) { r.DE.val--; my_clock.m = 1; my_clock.t = 8; }
void opcode_0x2b(void) { r.HL.val--; my_clock.m = 1; my_clock.t = 8; }
void opcode_0x3b(void) { r.SP.val--; my_clock.m = 1; my_clock.t = 8; }

// LD OPS
void opcode_0x3a(void) { r.AF.bytes.high = read_memory(r.HL.val); r.HL.val--; my_clock.m = 1; my_clock.t = 8; }
void opcode_0x21(void) { r.HL.val = read_word(); my_clock.m = 3; my_clock.t = 12; }
void opcode_0x31(void) { r.SP.val = read_word(); my_clock.m = 3; my_clock.t = 12; }
void opcode_0x06(void) { r.BC.bytes.high = read_byte(); my_clock.m = 2; my_clock.t = 8; }
void opcode_0x16(void) { r.DE.bytes.high = read_byte(); my_clock.m = 2; my_clock.t = 8; }
void opcode_0x26(void) { r.HL.bytes.high = read_byte(); my_clock.m = 2; my_clock.t = 8; }
void opcode_0xf9(void) { r.SP.val = r.HL.val; my_clock.m = 1; my_clock.t = 8; }

// ADD OPS
void opcode_0x09(void) { add_16_op(&r.HL.val, r.BC.val); }
void opcode_0x19(void) { add_16_op(&r.HL.val, r.DE.val); }
void opcode_0x29(void) { add_16_op(&r.HL.val, r.HL.val); }
void opcode_0x39(void) { add_16_op(&r.HL.val, r.SP.val); }

// ADC OPS
void opcode_0x88(void) { adc_op(&r.AF.bytes.high, r.BC.bytes.high); }
void opcode_0x89(void) { adc_op(&r.AF.bytes.high, r.BC.bytes.low); }
void opcode_0x8a(void) { adc_op(&r.AF.bytes.high, r.DE.bytes.high); }
void opcode_0x8b(void) { adc_op(&r.AF.bytes.high, r.DE.bytes.low); }
void opcode_0x8c(void) { adc_op(&r.AF.bytes.high, r.HL.bytes.high); }
void opcode_0x8d(void) { adc_op(&r.AF.bytes.high, r.HL.bytes.low); }
void opcode_0x8e(void)
{
  adc_op(&r.AF.bytes.high, read_memory(r.HL.val));
  my_clock.m = 1;
  my_clock.t = 8;
}
void opcode_0x8f(void) { adc_op(&r.AF.bytes.high, r.AF.bytes.high); }

// ADC A, d8
void opcode_0xce(void)
{
  adc_op(&r.AF.bytes.high, read_byte());
  my_clock.m = 2;
  my_clock.t = 8;
}

// ADD OPS
void opcode_0x80(void) { add_8_op(&r.AF.bytes.high, r.BC.bytes.high); }
void opcode_0x81(void) { add_8_op(&r.AF.bytes.high, r.BC.bytes.low); }
void opcode_0x82(void) { add_8_op(&r.AF.bytes.high, r.DE.bytes.high); }
void opcode_0x83(void) { add_8_op(&r.AF.bytes.high, r.DE.bytes.low); }
void opcode_0x84(void) { add_8_op(&r.AF.bytes.high, r.HL.bytes.high); }
void opcode_0x85(void) { add_8_op(&r.AF.bytes.high, r.HL.bytes.low); }
void opcode_0x86(void)
{
  add_8_op(&r.AF.bytes.high, read_memory(r.HL.val));
  my_clock.m = 1;
  my_clock.t = 8;
}
void opcode_0x87(void) { add_8_op(&r.AF.bytes.high, r.AF.bytes.high); }
void opcode_0xc6(void) { add_8_op(&r.AF.bytes.high, read_byte()); }

// SUB OPS
void opcode_0x90(void) { sub_8_op(&r.AF.bytes.high, r.BC.bytes.high); }
void opcode_0x91(void) { sub_8_op(&r.AF.bytes.high, r.BC.bytes.low); }
void opcode_0x92(void) { sub_8_op(&r.AF.bytes.high, r.DE.bytes.high); }
void opcode_0x93(void) { sub_8_op(&r.AF.bytes.high, r.DE.bytes.low); }
void opcode_0x94(void) { sub_8_op(&r.AF.bytes.high, r.HL.bytes.high); }
void opcode_0x95(void) { sub_8_op(&r.AF.bytes.high, r.HL.bytes.low); }
void opcode_0x96(void)
{
  sub_8_op(&r.AF.bytes.high, read_memory(r.HL.val));
  my_clock.m = 1;
  my_clock.t = 8;
}
void opcode_0x97(void) { r.AF.bytes.high = 0; resetH(); resetC(); setZ(); setN(); my_clock.m = 1; my_clock.t = 4; }
void opcode_0xd6(void) { sub_8_op(&r.AF.bytes.high, read_byte()); }

// SBC OPS
void opcode_0x98(void) { sbc_op(&r.AF.bytes.high, r.BC.bytes.high); }
void opcode_0x99(void) { sbc_op(&r.AF.bytes.high, r.BC.bytes.low); }
void opcode_0x9a(void) { sbc_op(&r.AF.bytes.high, r.DE.bytes.high); }
void opcode_0x9b(void) { sbc_op(&r.AF.bytes.high, r.DE.bytes.low); }
void opcode_0x9c(void) { sbc_op(&r.AF.bytes.high, r.HL.bytes.high); }
void opcode_0x9d(void) { sbc_op(&r.AF.bytes.high, r.HL.bytes.low); }
void opcode_0x9e(void)
{
  sbc_op(&r.AF.bytes.high, read_memory(r.HL.val));
  my_clock.m = 1;
  my_clock.t = 8;
}
void opcode_0x9f(void) { sbc_op(&r.AF.bytes.high, r.AF.bytes.high); }
void opcode_0xde(void) { sbc_op(&r.AF.bytes.high, read_byte()); }

// XOR OPS
void opcode_0xa8(void) { xor_8_op(&r.AF.bytes.high, r.BC.bytes.high); }
void opcode_0xa9(void) { xor_8_op(&r.AF.bytes.high, r.BC.bytes.low); }
void opcode_0xaa(void) { xor_8_op(&r.AF.bytes.high, r.DE.bytes.high); }
void opcode_0xab(void) { xor_8_op(&r.AF.bytes.high, r.DE.bytes.low); }
void opcode_0xac(void) { xor_8_op(&r.AF.bytes.high, r.HL.bytes.high); }
void opcode_0xad(void) { xor_8_op(&r.AF.bytes.high, r.HL.bytes.low); }
void opcode_0xae(void)
{
  xor_8_op(&r.AF.bytes.high, read_memory(r.HL.val));
  my_clock.m = 1;
  my_clock.t = 8;
}
void opcode_0xaf(void) { xor_8_op(&r.AF.bytes.high, r.AF.bytes.high); }
void opcode_0xee(void) { xor_8_op(&r.AF.bytes.high, read_byte()); }

// AND OPS
void opcode_0xe6(void)
{
  and_op(&r.AF.bytes.high, read_byte());
  my_clock.m = 2;
  my_clock.t = 8;
}
void opcode_0xa0(void) { and_op(&r.AF.bytes.high, r.BC.bytes.high); }
void opcode_0xa1(void) { and_op(&r.AF.bytes.high, r.BC.bytes.low); }
void opcode_0xa2(void) { and_op(&r.AF.bytes.high, r.DE.bytes.high); }
void opcode_0xa3(void) { and_op(&r.AF.bytes.high, r.DE.bytes.low); }
void opcode_0xa4(void) { and_op(&r.AF.bytes.high, r.HL.bytes.high); }
void opcode_0xa5(void) { and_op(&r.AF.bytes.high, r.HL.bytes.low); }
void opcode_0xa6(void)
{
  and_op(&r.AF.bytes.high, read_memory(r.HL.val));
  my_clock.m = 1;
  my_clock.t = 8;
}
void opcode_0xa7(void) { and_op(&r.AF.bytes.high, r.AF.bytes.high); }

// OR OPS
void opcode_0xf6(void)
{
  or_op(&r.AF.bytes.high, read_byte());
  my_clock.m = 2;
  my_clock.t = 8;
}
void opcode_0xb0(void) { or_op(&r.AF.bytes.high, r.BC.bytes.high); }
void opcode_0xb1(void) { or_op(&r.AF.bytes.high, r.BC.bytes.low); }
void opcode_0xb2(void) { or_op(&r.AF.bytes.high, r.DE.bytes.high); }
void opcode_0xb3(void) { or_op(&r.AF.bytes.high, r.DE.bytes.low); }
void opcode_0xb4(void) { or_op(&r.AF.bytes.high, r.HL.bytes.high); }
void opcode_0xb5(void) { or_op(&r.AF.bytes.high, r.HL.bytes.low); }
void opcode_0xb6(void)
{
  or_op(&r.AF.bytes.high, read_memory(r.HL.val));
  my_clock.m = 1;
  my_clock.t = 8;
}
void opcode_0xb7(void) { or_op(&r.AF.bytes.high, r.AF.bytes.high); }

// CP OPS
void opcode_0xfe(void)
{
  cp_op(r.AF.bytes.high, read_byte());
  my_clock.m = 2;
  my_clock.t = 8;
}
void opcode_0xb8(void) { cp_op(r.AF.bytes.high, r.BC.bytes.high); }
void opcode_0xb9(void) { cp_op(r.AF.bytes.high, r.BC.bytes.low); }
void opcode_0xba(void) { cp_op(r.AF.bytes.high, r.DE.bytes.high); }
void opcode_0xbb(void) { cp_op(r.AF.bytes.high, r.DE.bytes.low); }
void opcode_0xbc(void) { cp_op(r.AF.bytes.high, r.HL.bytes.high); }
void opcode_0xbd(void) { cp_op(r.AF.bytes.high, r.HL.bytes.low); }
void opcode_0xbe(void)
{
  cp_op(r.AF.bytes.high, read_memory(r.HL.val));
  my_clock.m = 1;
  my_clock.t = 8;
}
void opcode_0xbf(void) { cp_op(r.AF.bytes.high, r.AF.bytes.high); }

// (a16) <- A
void opcode_0xea(void)
{
  uint16_t val = read_word();
  write_memory(val, r.AF.bytes.high);

  my_clock.m = 3;
  my_clock.t = 16;
}

// A <- (a16)
void opcode_0xfa(void)
{
  uint16_t val = read_word();
  r.AF.bytes.high = read_memory(val);

  my_clock.m = 3;
  my_clock.t = 16;
}

// JP a16
void opcode_0xc3(void)
{
  uint16_t addr = read_word();
  r.PC.val = addr;

  my_clock.m = 3;
  my_clock.t = 16;
}

// JP (HL) -> special behaviour!! :( means that PC = HL
void opcode_0xe9(void)
{
  r.PC.val = r.HL.val;

  my_clock.m = 1;
  my_clock.t = 4;
}

// JP Z, a16
void opcode_0xca(void)
{
  uint16_t addr = read_word();
  if (getZ())
  {
    r.PC.val = addr;
    my_clock.t = 16;
  }
  else
  {
    my_clock.t = 12;
  }
  my_clock.m = 3;
}

// JP C, a16
void opcode_0xda(void)
{
  uint16_t addr = read_word();
  if (getC())
  {
    r.PC.val = addr;
    my_clock.t = 16;
  }
  else
  {
    my_clock.t = 12;
  }
  my_clock.m = 3;
}

// JP NZ, a16
void opcode_0xc2(void)
{
  uint16_t addr = read_word();
  if (!getZ())
  {
    r.PC.val = addr;
    my_clock.t = 16;
  }
  else
  {
    my_clock.t = 12;
  }
  my_clock.m = 3;
}

// JP NC, a16
void opcode_0xd2(void)
{
  uint16_t addr = read_word();
  if (!getC())
  {
    r.PC.val = addr;
    my_clock.t = 16;
  }
  else
  {
    my_clock.t = 12;
  }
  my_clock.m = 3;
}

// ADD SP, r8
void opcode_0xe8(void)
{
  int32_t tmp2 = read_byte();
  uint16_t tmp = r.SP.val + tmp2;
  tmp2 = r.SP.val ^ tmp2 ^ tmp;
  r.SP.val = tmp2;

  (tmp2 & 0x100) == 0x100 ? setC() : resetC();
  (tmp2 & 0x10) == 0x10 ? setH() : resetH();
  resetZ();
  resetN();

  my_clock.m = 2;
  my_clock.t = 16;
}

// ADD HL, (SP + r8)
void opcode_0xf8(void)
{
  int8_t tmp = read_byte();
  r.HL.val = (r.SP.val + tmp);
  tmp = (r.SP.val ^ tmp ^ r.HL.val);

  (tmp & 0x100) == 0x100 ? setC() : resetC();
  (tmp & 0x10) == 0x10 ? setH() : resetH();
  resetZ();
  resetN();

  my_clock.m = 2;
  my_clock.t = 12;
}

// JR r8
void opcode_0x18(void)
{
  int8_t addr = read_byte();
  r.PC.val += addr;

  my_clock.m = 2;
  my_clock.t = 12;
}

// JR Z, r8
void opcode_0x28(void)
{
  int8_t addr = read_byte();
  if (getZ())
  {
    r.PC.val += addr;
    my_clock.t = 12;
  }
  else
  {
    my_clock.t = 8;
  }

  my_clock.m = 2;
}

// JR C,r8
void opcode_0x38(void)
{
  int8_t addr = read_byte();
  if (getC())
  {
    r.PC.val += addr;
    my_clock.t = 12;
  }
  else
  {
    my_clock.t = 8;
  }

  my_clock.m = 2;
}

// JR NC,r8
void opcode_0x30(void)
{
  int8_t addr = read_byte();
  if (!getC())
  {
    r.PC.val += addr;
    my_clock.t = 12;
  }
  else
  {
    my_clock.t = 8;
  }

  my_clock.m = 2;
}

// RRCA
void opcode_0x0f(void)
{
  r.AF.bytes.high = ((r.AF.bytes.high >> 1) | ((r.AF.bytes.high & 1) << 7));
  (r.AF.bytes.high > 0x7F) ? setC() : resetC();

  resetZ();
  resetN();
  resetH();

  my_clock.m = 1;
  my_clock.t = 4;
}

// RLCA
void opcode_0x07(void)
{
  (r.AF.bytes.high > 0x7F) ? setC() : resetC();
  r.AF.bytes.high = ((r.AF.bytes.high << 1) | (r.AF.bytes.high >> 7));

  resetZ();
  resetN();
  resetH();

  my_clock.m = 1;
  my_clock.t = 4;
}

// RLA
void opcode_0x17(void)
{
  uint8_t carry = getC();
  (r.AF.bytes.high > 0x7F) ? setC() : resetC();
  r.AF.bytes.high = ((r.AF.bytes.high << 1) | carry);

  resetZ();
  resetN();
  resetH();

  my_clock.m = 1;
  my_clock.t = 4;
}

// RRA
void opcode_0x1f(void)
{
  uint8_t carry = (getC() << 7);
  (r.AF.bytes.high & 1) ? setC() : resetC();
  r.AF.bytes.high = ((r.AF.bytes.high >> 1) | carry);

  resetZ();
  resetN();
  resetH();

  my_clock.m = 1;
  my_clock.t = 4;
}

// DEC OPS
void opcode_0x0d(void) { dec_op(&r.BC.bytes.low); }
void opcode_0x1d(void) { dec_op(&r.DE.bytes.low); }
void opcode_0x2d(void) { dec_op(&r.HL.bytes.low); }
void opcode_0x3d(void) { dec_op(&r.AF.bytes.high); }
void opcode_0x05(void) { dec_op(&r.BC.bytes.high); }
void opcode_0x15(void) { dec_op(&r.DE.bytes.high); }
void opcode_0x25(void) { dec_op(&r.HL.bytes.high); }
void opcode_0x35(void)
{
  uint8_t hl = read_memory(r.HL.val);
  if (hl == 0)
    setH();

  write_memory(r.HL.val, hl - 1);
  read_memory(r.HL.val) == 0 ? setZ() : resetZ();
  setN();

  my_clock.m = 1;
  my_clock.t = 12;
}

// INC OPS
void opcode_0x03(void) { r.BC.val++; my_clock.m = 1; my_clock.t = 8; }
void opcode_0x13(void) { r.DE.val++; my_clock.m = 1; my_clock.t = 8; }
void opcode_0x23(void) { r.HL.val++; my_clock.m = 1; my_clock.t = 8; }
void opcode_0x33(void) { r.SP.val++; my_clock.m = 1; my_clock.t = 8; }

// RET
void opcode_0xc9(void) { pop_op(&r.PC.val); my_clock.m = 1; my_clock.t = 16; }

// RETI (return then enable interupt)
void opcode_0xd9(void) { opcode_0xc9(); r.ime = 1; }

// RET OPS
void opcode_0xc0(void) { ret_cond_op(!getZ()); }
void opcode_0xc8(void) { ret_cond_op(getZ()); }
void opcode_0xd0(void) { ret_cond_op(!getC()); }
void opcode_0xd8(void) { ret_cond_op(getC()); }

// LDH (a8), A
void opcode_0xe0(void)
{
  uint16_t addr = 0xFF00 + read_byte();
  write_memory(addr, r.AF.bytes.high);

  my_clock.m = 2;
  my_clock.t = 12;
}

// LDH A, (a8)
void opcode_0xf0(void)
{
  uint16_t addr = 0xFF00 + read_byte();
  r.AF.bytes.high = read_memory(addr);

  my_clock.m = 2;
  my_clock.t = 12;
}

// DI
void opcode_0xf3(void)
{
  r.ime = 0;

  my_clock.m = 1;
  my_clock.t = 4;
}

// EI
void opcode_0xfb(void)
{
  r.ime = 1;

  my_clock.m = 1;
  my_clock.t = 4;
}

// CPL
void opcode_0x2f(void)
{
  r.AF.bytes.high ^= 0xFF;
  setN();
  setH();

  my_clock.m = 1;
  my_clock.t = 4;
}

// CCF
void opcode_0x3f(void)
{
  getC() ? resetC() : setC(); // Inverse Carry flag
  resetN();
  resetH();

  my_clock.m = 1;
  my_clock.t = 4;
}

// RST OPS
void opcode_0xc7(void) { rst_op(0); }
void opcode_0xcf(void) { rst_op(0x08); }
void opcode_0xd7(void) { rst_op(0x10); }
void opcode_0xdf(void) { rst_op(0x18); }
void opcode_0xe7(void) { rst_op(0x20); }
void opcode_0xef(void) { rst_op(0x28); }
void opcode_0xf7(void) { rst_op(0x30); }
void opcode_0xff(void) { rst_op(0x38); }

// LD C, (HL)
void opcode_0x4e(void)
{
  load(&r.BC.bytes.low, read_memory(r.HL.val));
  my_clock.m = 1;
  my_clock.t = 8;
}

// LD E, (HL)
void opcode_0x5e(void)
{
  load(&r.DE.bytes.low, read_memory(r.HL.val));
  my_clock.m = 1;
  my_clock.t = 8;
}

// LD L, (HL)
void opcode_0x6e(void)
{
  load(&r.HL.bytes.low, read_memory(r.HL.val));
  my_clock.m = 1;
  my_clock.t = 8;
}

// LD A, (HL)
void opcode_0x7e(void)
{
  load(&r.AF.bytes.high, read_memory(r.HL.val));
  my_clock.m = 1;
  my_clock.t = 8;
}

// DAA
void opcode_0x27(void)
{
  if (!getN())
  {
    if (getC() || (r.AF.bytes.high > 0x99))
    {
      r.AF.bytes.high += 0x60;
      setC();
    }
    if (getH() || ((r.AF.bytes.high & 0x0F) > 0x09))
    {
			r.AF.bytes.high += 0x6;
    }
	}
  else
  {
    if (getC())
    {
      r.AF.bytes.high -= 0x60;
    }
    if (getH())
    {
  		r.AF.bytes.high -= 0x6;
    }
  }

  (r.AF.bytes.high == 0) ? setZ() : resetZ();
  resetH();

  my_clock.m = 1;
  my_clock.t = 4;
}

// HALT
void opcode_0x76(void)
{
  MMU.HALT = 1;

  my_clock.m = 1;
  my_clock.t = 4;
}

// RL OPS
void prefix_0x10(void) { rl_op(&r.BC.bytes.high); }
void prefix_0x11(void) { rl_op(&r.BC.bytes.low); }
void prefix_0x12(void) { rl_op(&r.DE.bytes.high); }
void prefix_0x13(void) { rl_op(&r.DE.bytes.low); }
void prefix_0x14(void) { rl_op(&r.HL.bytes.high); }
void prefix_0x15(void) { rl_op(&r.HL.bytes.low); }
void prefix_0x16(void) { rl_op(&MMU.memory[r.HL.val]); my_clock.m = 2; my_clock.t = 16; }
void prefix_0x17(void) { rl_op(&r.AF.bytes.high); }

// RR OPS
void prefix_0x18(void) { rr_op(&r.BC.bytes.high); }
void prefix_0x19(void) { rr_op(&r.BC.bytes.low); }
void prefix_0x1a(void) { rr_op(&r.DE.bytes.high); }
void prefix_0x1b(void) { rr_op(&r.DE.bytes.low); }
void prefix_0x1c(void) { rr_op(&r.HL.bytes.high); }
void prefix_0x1d(void) { rr_op(&r.HL.bytes.low); }
void prefix_0x1e(void) { rr_op(&MMU.memory[r.HL.val]); my_clock.m = 2; my_clock.t = 16; }
void prefix_0x1f(void) { rr_op(&r.AF.bytes.high); }

// SLA OPS
void prefix_0x20(void) { sla_op(&r.BC.bytes.high); }
void prefix_0x21(void) { sla_op(&r.BC.bytes.low); }
void prefix_0x22(void) { sla_op(&r.DE.bytes.high); }
void prefix_0x23(void) { sla_op(&r.DE.bytes.low); }
void prefix_0x24(void) { sla_op(&r.HL.bytes.high); }
void prefix_0x25(void) { sla_op(&r.HL.bytes.low); }
void prefix_0x26(void) { sla_op(&MMU.memory[r.HL.val]); my_clock.m = 2; my_clock.t = 16; }
void prefix_0x27(void) { sla_op(&r.AF.bytes.high); }

// SRA OPS
void prefix_0x28(void) { sla_op(&r.BC.bytes.high); }
void prefix_0x29(void) { sla_op(&r.BC.bytes.low); }
void prefix_0x2a(void) { sla_op(&r.DE.bytes.high); }
void prefix_0x2b(void) { sla_op(&r.DE.bytes.low); }
void prefix_0x2c(void) { sla_op(&r.HL.bytes.high); }
void prefix_0x2d(void) { sla_op(&r.HL.bytes.low); }
void prefix_0x2e(void) { sla_op(&MMU.memory[r.HL.val]); my_clock.m = 2; my_clock.t = 16; }
void prefix_0x2f(void) { sla_op(&r.AF.bytes.high); }

// SWAP OPS
void prefix_0x30(void) { swap_op(&r.BC.bytes.high); }
void prefix_0x31(void) { swap_op(&r.BC.bytes.low); }
void prefix_0x32(void) { swap_op(&r.DE.bytes.high); }
void prefix_0x33(void) { swap_op(&r.DE.bytes.low); }
void prefix_0x34(void) { swap_op(&r.HL.bytes.high); }
void prefix_0x35(void) { swap_op(&r.HL.bytes.low); }
void prefix_0x36(void) { swap_op(&MMU.memory[r.HL.val]); my_clock.m = 2; my_clock.t = 16; }
void prefix_0x37(void) { swap_op(&r.AF.bytes.high); }

// SLR OPS
void prefix_0x38(void) { srl_op(&r.BC.bytes.high); }
void prefix_0x39(void) { srl_op(&r.BC.bytes.low); }
void prefix_0x3a(void) { srl_op(&r.DE.bytes.high); }
void prefix_0x3b(void) { srl_op(&r.DE.bytes.low); }
void prefix_0x3c(void) { srl_op(&r.HL.bytes.high); }
void prefix_0x3d(void) { srl_op(&r.HL.bytes.low); }
void prefix_0x3e(void) { srl_op(&MMU.memory[r.HL.val]); my_clock.m = 2; my_clock.t = 16; }
void prefix_0x3f(void) { srl_op(&r.AF.bytes.high); }

// BIT PREFIX OPS
void prefix_0x40(void) { bit_op(r.BC.bytes.high, 0); }
void prefix_0x41(void) { bit_op(r.BC.bytes.low, 0); }
void prefix_0x42(void) { bit_op(r.DE.bytes.high, 0); }
void prefix_0x43(void) { bit_op(r.DE.bytes.low, 0); }
void prefix_0x44(void) { bit_op(r.HL.bytes.high, 0); }
void prefix_0x45(void) { bit_op(r.HL.bytes.low, 0); }
void prefix_0x46(void) { bit_op(read_memory(r.HL.val), 0); my_clock.m = 2; my_clock.t = 16; }
void prefix_0x47(void) { bit_op(r.AF.bytes.high, 0); }

void prefix_0x48(void) { bit_op(r.BC.bytes.high, 1); }
void prefix_0x49(void) { bit_op(r.BC.bytes.low, 1); }
void prefix_0x4a(void) { bit_op(r.DE.bytes.high, 1); }
void prefix_0x4b(void) { bit_op(r.DE.bytes.low, 1); }
void prefix_0x4c(void) { bit_op(r.HL.bytes.high, 1); }
void prefix_0x4d(void) { bit_op(r.HL.bytes.low, 1); }
void prefix_0x4e(void) { bit_op(read_memory(r.HL.val), 1); my_clock.m = 2; my_clock.t = 16; }
void prefix_0x4f(void) { bit_op(r.AF.bytes.high, 1); }

void prefix_0x50(void) { bit_op(r.BC.bytes.high, 2); }
void prefix_0x51(void) { bit_op(r.BC.bytes.low, 2); }
void prefix_0x52(void) { bit_op(r.DE.bytes.high, 2); }
void prefix_0x53(void) { bit_op(r.DE.bytes.low, 2); }
void prefix_0x54(void) { bit_op(r.HL.bytes.high, 2); }
void prefix_0x55(void) { bit_op(r.HL.bytes.low, 2); }
void prefix_0x56(void) { bit_op(read_memory(r.HL.val), 2); my_clock.m = 2; my_clock.t = 16; }
void prefix_0x57(void) { bit_op(r.AF.bytes.high, 2); }

void prefix_0x58(void) { bit_op(r.BC.bytes.high, 3); }
void prefix_0x59(void) { bit_op(r.BC.bytes.low, 3); }
void prefix_0x5a(void) { bit_op(r.DE.bytes.high, 3); }
void prefix_0x5b(void) { bit_op(r.DE.bytes.low, 3); }
void prefix_0x5c(void) { bit_op(r.HL.bytes.high, 3); }
void prefix_0x5d(void) { bit_op(r.HL.bytes.low, 3); }
void prefix_0x5e(void) { bit_op(read_memory(r.HL.val), 3); my_clock.m = 2; my_clock.t = 16; }
void prefix_0x5f(void) { bit_op(r.AF.bytes.high, 3); }

void prefix_0x60(void) { bit_op(r.BC.bytes.high, 4); }
void prefix_0x61(void) { bit_op(r.BC.bytes.low, 4); }
void prefix_0x62(void) { bit_op(r.DE.bytes.high, 4); }
void prefix_0x63(void) { bit_op(r.DE.bytes.low, 4); }
void prefix_0x64(void) { bit_op(r.HL.bytes.high, 4); }
void prefix_0x65(void) { bit_op(r.HL.bytes.low, 4); }
void prefix_0x66(void) { bit_op(read_memory(r.HL.val), 4); my_clock.m = 2; my_clock.t = 16; }
void prefix_0x67(void) { bit_op(r.AF.bytes.high, 4); }

void prefix_0x68(void) { bit_op(r.BC.bytes.high, 5); }
void prefix_0x69(void) { bit_op(r.BC.bytes.low, 5); }
void prefix_0x6a(void) { bit_op(r.DE.bytes.high, 5); }
void prefix_0x6b(void) { bit_op(r.DE.bytes.low, 5); }
void prefix_0x6c(void) { bit_op(r.HL.bytes.high, 5); }
void prefix_0x6d(void) { bit_op(r.HL.bytes.low, 5); }
void prefix_0x6e(void) { bit_op(read_memory(r.HL.val), 5); my_clock.m = 2; my_clock.t = 16; }
void prefix_0x6f(void) { bit_op(r.AF.bytes.high, 5); }

void prefix_0x70(void) { bit_op(r.BC.bytes.high, 6); }
void prefix_0x71(void) { bit_op(r.BC.bytes.low, 6); }
void prefix_0x72(void) { bit_op(r.DE.bytes.high, 6); }
void prefix_0x73(void) { bit_op(r.DE.bytes.low, 6); }
void prefix_0x74(void) { bit_op(r.HL.bytes.high, 6); }
void prefix_0x75(void) { bit_op(r.HL.bytes.low, 6); }
void prefix_0x76(void) { bit_op(read_memory(r.HL.val), 6); my_clock.m = 2; my_clock.t = 16; }
void prefix_0x77(void) { bit_op(r.AF.bytes.high, 6); }

void prefix_0x78(void) { bit_op(r.BC.bytes.high, 7); }
void prefix_0x79(void) { bit_op(r.BC.bytes.low, 7); }
void prefix_0x7a(void) { bit_op(r.DE.bytes.high, 7); }
void prefix_0x7b(void) { bit_op(r.DE.bytes.low, 7); }
void prefix_0x7c(void) { bit_op(r.HL.bytes.high, 7); }
void prefix_0x7d(void) { bit_op(r.HL.bytes.low, 7); }
void prefix_0x7e(void) { bit_op(read_memory(r.HL.val), 7); my_clock.m = 2; my_clock.t = 16; }
void prefix_0x7f(void) { bit_op(r.AF.bytes.high, 7); }

// RES PREFIX OPS
void prefix_0x80(void) { res_op(&r.BC.bytes.high, 0); }
void prefix_0x81(void) { res_op(&r.BC.bytes.low, 0); }
void prefix_0x82(void) { res_op(&r.DE.bytes.high, 0); }
void prefix_0x83(void) { res_op(&r.DE.bytes.low, 0); }
void prefix_0x84(void) { res_op(&r.HL.bytes.high, 0); }
void prefix_0x85(void) { res_op(&r.HL.bytes.low, 0); }
void prefix_0x86(void) { res_op(&MMU.memory[r.HL.val], 0); my_clock.m = 2; my_clock.t = 16; }
void prefix_0x87(void) { res_op(&r.AF.bytes.high, 0); }

void prefix_0x88(void) { res_op(&r.BC.bytes.high, 1); }
void prefix_0x89(void) { res_op(&r.BC.bytes.low, 1); }
void prefix_0x8a(void) { res_op(&r.DE.bytes.high, 1); }
void prefix_0x8b(void) { res_op(&r.DE.bytes.low, 1); }
void prefix_0x8c(void) { res_op(&r.HL.bytes.high, 1); }
void prefix_0x8d(void) { res_op(&r.HL.bytes.low, 1); }
void prefix_0x8e(void) { res_op(&MMU.memory[r.HL.val], 1); my_clock.m = 2; my_clock.t = 16; }
void prefix_0x8f(void) { res_op(&r.AF.bytes.high, 1); }

void prefix_0x90(void) { res_op(&r.BC.bytes.high, 2); }
void prefix_0x91(void) { res_op(&r.BC.bytes.low, 2); }
void prefix_0x92(void) { res_op(&r.DE.bytes.high, 2); }
void prefix_0x93(void) { res_op(&r.DE.bytes.low, 2); }
void prefix_0x94(void) { res_op(&r.HL.bytes.high, 2); }
void prefix_0x95(void) { res_op(&r.HL.bytes.low, 2); }
void prefix_0x96(void) { res_op(&MMU.memory[r.HL.val], 2); my_clock.m = 2; my_clock.t = 16; }
void prefix_0x97(void) { res_op(&r.AF.bytes.high, 2); }

void prefix_0x98(void) { res_op(&r.BC.bytes.high, 3); }
void prefix_0x99(void) { res_op(&r.BC.bytes.low, 3); }
void prefix_0x9a(void) { res_op(&r.DE.bytes.high, 3); }
void prefix_0x9b(void) { res_op(&r.DE.bytes.low, 3); }
void prefix_0x9c(void) { res_op(&r.HL.bytes.high, 3); }
void prefix_0x9d(void) { res_op(&r.HL.bytes.low, 3); }
void prefix_0x9e(void) { res_op(&MMU.memory[r.HL.val], 3); my_clock.m = 2; my_clock.t = 16; }
void prefix_0x9f(void) { res_op(&r.AF.bytes.high, 3); }

void prefix_0xa0(void) { res_op(&r.BC.bytes.high, 4); }
void prefix_0xa1(void) { res_op(&r.BC.bytes.low, 4); }
void prefix_0xa2(void) { res_op(&r.DE.bytes.high, 4); }
void prefix_0xa3(void) { res_op(&r.DE.bytes.low, 4); }
void prefix_0xa4(void) { res_op(&r.HL.bytes.high, 4); }
void prefix_0xa5(void) { res_op(&r.HL.bytes.low, 4); }
void prefix_0xa6(void) { res_op(&MMU.memory[r.HL.val], 4); my_clock.m = 2; my_clock.t = 16; }
void prefix_0xa7(void) { res_op(&r.AF.bytes.high, 4); }

void prefix_0xa8(void) { res_op(&r.BC.bytes.high, 5); }
void prefix_0xa9(void) { res_op(&r.BC.bytes.low, 5); }
void prefix_0xaa(void) { res_op(&r.DE.bytes.high, 5); }
void prefix_0xab(void) { res_op(&r.DE.bytes.low, 5); }
void prefix_0xac(void) { res_op(&r.HL.bytes.high, 5); }
void prefix_0xad(void) { res_op(&r.HL.bytes.low, 5); }
void prefix_0xae(void) { res_op(&MMU.memory[r.HL.val], 5); my_clock.m = 2; my_clock.t = 16; }
void prefix_0xaf(void) { res_op(&r.AF.bytes.high, 5); }

void prefix_0xb0(void) { res_op(&r.BC.bytes.high, 6); }
void prefix_0xb1(void) { res_op(&r.BC.bytes.low, 6); }
void prefix_0xb2(void) { res_op(&r.DE.bytes.high, 6); }
void prefix_0xb3(void) { res_op(&r.DE.bytes.low, 6); }
void prefix_0xb4(void) { res_op(&r.HL.bytes.high, 6); }
void prefix_0xb5(void) { res_op(&r.HL.bytes.low, 6); }
void prefix_0xb6(void) { res_op(&MMU.memory[r.HL.val], 6); my_clock.m = 2; my_clock.t = 16; }
void prefix_0xb7(void) { res_op(&r.AF.bytes.high, 6); }

void prefix_0xb8(void) { res_op(&r.BC.bytes.high, 7); }
void prefix_0xb9(void) { res_op(&r.BC.bytes.low, 7); }
void prefix_0xba(void) { res_op(&r.DE.bytes.high, 7); }
void prefix_0xbb(void) { res_op(&r.DE.bytes.low, 7); }
void prefix_0xbc(void) { res_op(&r.HL.bytes.high, 7); }
void prefix_0xbd(void) { res_op(&r.HL.bytes.low, 7); }
void prefix_0xbe(void) { res_op(&MMU.memory[r.HL.val], 7); my_clock.m = 2; my_clock.t = 16; }
void prefix_0xbf(void) { res_op(&r.AF.bytes.high, 7); }

// SET PREFIX OPS
void prefix_0xc0(void) { set_op(&r.BC.bytes.high, 0); }
void prefix_0xc1(void) { set_op(&r.BC.bytes.low, 0); }
void prefix_0xc2(void) { set_op(&r.DE.bytes.high, 0); }
void prefix_0xc3(void) { set_op(&r.DE.bytes.low, 0); }
void prefix_0xc4(void) { set_op(&r.HL.bytes.high, 0); }
void prefix_0xc5(void) { set_op(&r.HL.bytes.low, 0); }
void prefix_0xc6(void) { set_op(&MMU.memory[r.HL.val], 0); my_clock.m = 2; my_clock.t = 16; }
void prefix_0xc7(void) { set_op(&r.AF.bytes.high, 0); }

void prefix_0xc8(void) { set_op(&r.BC.bytes.high, 1); }
void prefix_0xc9(void) { set_op(&r.BC.bytes.low, 1); }
void prefix_0xca(void) { set_op(&r.DE.bytes.high, 1); }
void prefix_0xcb(void) { set_op(&r.DE.bytes.low, 1); }
void prefix_0xcc(void) { set_op(&r.HL.bytes.high, 1); }
void prefix_0xcd(void) { set_op(&r.HL.bytes.low, 1); }
void prefix_0xce(void) { set_op(&MMU.memory[r.HL.val], 1); my_clock.m = 2; my_clock.t = 16; }
void prefix_0xcf(void) { set_op(&r.AF.bytes.high, 1); }

void prefix_0xd0(void) { set_op(&r.BC.bytes.high, 2); }
void prefix_0xd1(void) { set_op(&r.BC.bytes.low, 2); }
void prefix_0xd2(void) { set_op(&r.DE.bytes.high, 2); }
void prefix_0xd3(void) { set_op(&r.DE.bytes.low, 2); }
void prefix_0xd4(void) { set_op(&r.HL.bytes.high, 2); }
void prefix_0xd5(void) { set_op(&r.HL.bytes.low, 2); }
void prefix_0xd6(void) { set_op(&MMU.memory[r.HL.val], 2); my_clock.m = 2; my_clock.t = 16; }
void prefix_0xd7(void) { set_op(&r.AF.bytes.high, 2); }

void prefix_0xd8(void) { set_op(&r.BC.bytes.high, 3); }
void prefix_0xd9(void) { set_op(&r.BC.bytes.low, 3); }
void prefix_0xda(void) { set_op(&r.DE.bytes.high, 3); }
void prefix_0xdb(void) { set_op(&r.DE.bytes.low, 3); }
void prefix_0xdc(void) { set_op(&r.HL.bytes.high, 3); }
void prefix_0xdd(void) { set_op(&r.HL.bytes.low, 3); }
void prefix_0xde(void) { set_op(&MMU.memory[r.HL.val], 3); my_clock.m = 2; my_clock.t = 16; }
void prefix_0xdf(void) { set_op(&r.AF.bytes.high, 3); }

void prefix_0xe0(void) { set_op(&r.BC.bytes.high, 4); }
void prefix_0xe1(void) { set_op(&r.BC.bytes.low, 4); }
void prefix_0xe2(void) { set_op(&r.DE.bytes.high, 4); }
void prefix_0xe3(void) { set_op(&r.DE.bytes.low, 4); }
void prefix_0xe4(void) { set_op(&r.HL.bytes.high, 4); }
void prefix_0xe5(void) { set_op(&r.HL.bytes.low, 4); }
void prefix_0xe6(void) { set_op(&MMU.memory[r.HL.val], 4); my_clock.m = 2; my_clock.t = 16; }
void prefix_0xe7(void) { set_op(&r.AF.bytes.high, 4); }

void prefix_0xe8(void) { set_op(&r.BC.bytes.high, 5); }
void prefix_0xe9(void) { set_op(&r.BC.bytes.low, 5); }
void prefix_0xea(void) { set_op(&r.DE.bytes.high, 5); }
void prefix_0xeb(void) { set_op(&r.DE.bytes.low, 5); }
void prefix_0xec(void) { set_op(&r.HL.bytes.high, 5); }
void prefix_0xed(void) { set_op(&r.HL.bytes.low, 5); }
void prefix_0xee(void) { set_op(&MMU.memory[r.HL.val], 5); my_clock.m = 2; my_clock.t = 16; }
void prefix_0xef(void) { set_op(&r.AF.bytes.high, 5); }

void prefix_0xf0(void) { set_op(&r.BC.bytes.high, 6); }
void prefix_0xf1(void) { set_op(&r.BC.bytes.low, 6); }
void prefix_0xf2(void) { set_op(&r.DE.bytes.high, 6); }
void prefix_0xf3(void) { set_op(&r.DE.bytes.low, 6); }
void prefix_0xf4(void) { set_op(&r.HL.bytes.high, 6); }
void prefix_0xf5(void) { set_op(&r.HL.bytes.low, 6); }
void prefix_0xf6(void) { set_op(&MMU.memory[r.HL.val], 6); my_clock.m = 2; my_clock.t = 16; }
void prefix_0xf7(void) { set_op(&r.AF.bytes.high, 6); }

void prefix_0xf8(void) { set_op(&r.BC.bytes.high, 7); }
void prefix_0xf9(void) { set_op(&r.BC.bytes.low, 7); }
void prefix_0xfa(void) { set_op(&r.DE.bytes.high, 7); }
void prefix_0xfb(void) { set_op(&r.DE.bytes.low, 7); }
void prefix_0xfc(void) { set_op(&r.HL.bytes.high, 7); }
void prefix_0xfd(void) { set_op(&r.HL.bytes.low, 7); }
void prefix_0xfe(void) { set_op(&MMU.memory[r.HL.val], 7); my_clock.m = 2; my_clock.t = 16; }
void prefix_0xff(void) { set_op(&r.AF.bytes.high, 7); }

void load_opcodes(void)
{
  Opcodes[0x00] = &opcode_0x00;
  Opcodes[0x01] = &opcode_0x01;
  Opcodes[0x02] = &opcode_0x02;
  Opcodes[0x03] = &opcode_0x03;
  Opcodes[0x04] = &opcode_0x04;
  Opcodes[0x05] = &opcode_0x05;
  Opcodes[0x06] = &opcode_0x06;
  Opcodes[0x07] = &opcode_0x07;
  Opcodes[0x08] = &opcode_0x08;
  Opcodes[0x09] = &opcode_0x09;
  Opcodes[0x0A] = &opcode_0x0a;
  Opcodes[0x0B] = &opcode_0x0b;
  Opcodes[0x0C] = &opcode_0x0c;
  Opcodes[0x0D] = &opcode_0x0d;
  Opcodes[0x0E] = &loadcd8;
  Opcodes[0x0F] = &opcode_0x0f;

  Opcodes[0x11] = &opcode_0x11;
  Opcodes[0x12] = &opcode_0x12;
  Opcodes[0x13] = &opcode_0x13;
  Opcodes[0x14] = &opcode_0x14;
  Opcodes[0x15] = &opcode_0x15;
  Opcodes[0x16] = &opcode_0x16;
  Opcodes[0x17] = &opcode_0x17;
  Opcodes[0x18] = &opcode_0x18;
  Opcodes[0x19] = &opcode_0x19;
  Opcodes[0x1A] = &opcode_0x1a;
  Opcodes[0x1B] = &opcode_0x1b;
  Opcodes[0x1C] = &opcode_0x1c;
  Opcodes[0x1D] = &opcode_0x1d;
  Opcodes[0x1E] = &loaded8;
  Opcodes[0x1F] = &opcode_0x1f;

  Opcodes[0x20] = &opcode_0x20;
  Opcodes[0x21] = &opcode_0x21;
  Opcodes[0x22] = &loadhlpa;
  Opcodes[0x23] = &opcode_0x23;
  Opcodes[0x24] = &opcode_0x24;
  Opcodes[0x25] = &opcode_0x25;
  Opcodes[0x26] = &opcode_0x26;
  Opcodes[0x27] = &opcode_0x27;
  Opcodes[0x28] = &opcode_0x28;
  Opcodes[0x29] = &opcode_0x29;
  Opcodes[0x2A] = &opcode_0x2a;
  Opcodes[0x2B] = &opcode_0x2b;
  Opcodes[0x2C] = &opcode_0x2c;
  Opcodes[0x2D] = &opcode_0x2d;
  Opcodes[0x2E] = &loadld8;
  Opcodes[0x2F] = &opcode_0x2f;

  Opcodes[0x30] = &opcode_0x30;
  Opcodes[0x31] = &opcode_0x31;
  Opcodes[0x32] = &loadhlma;
  Opcodes[0x33] = &opcode_0x33;
  Opcodes[0x34] = &opcode_0x34;
  Opcodes[0x35] = &opcode_0x35;
  Opcodes[0x36] = &opcode_0x36;
  Opcodes[0x38] = &opcode_0x38;
  Opcodes[0x39] = &opcode_0x39;
  Opcodes[0x3A] = &opcode_0x3a;
  Opcodes[0x3B] = &opcode_0x3b;
  Opcodes[0x3C] = &opcode_0x3c;
  Opcodes[0x3D] = &opcode_0x3d;
  Opcodes[0x3E] = &loadad8;
  Opcodes[0x3F] = &opcode_0x3f;

  Opcodes[0x40] = &loadbb;
  Opcodes[0x41] = &loadbc;
  Opcodes[0x42] = &loadbd;
  Opcodes[0x43] = &loadbe;
  Opcodes[0x44] = &loadbh;
  Opcodes[0x45] = &loadbl;
  Opcodes[0x46] = &opcode_0x46;
  Opcodes[0x47] = &loadba;
  Opcodes[0x48] = &loadcb;
  Opcodes[0x49] = &loadcc;
  Opcodes[0x4A] = &loadcd;
  Opcodes[0x4B] = &loadce;
  Opcodes[0x4C] = &loadch;
  Opcodes[0x4D] = &loadcl;
  Opcodes[0x4E] = &opcode_0x4e;
  Opcodes[0x4F] = &loadca;

  Opcodes[0x50] = &loaddb;
  Opcodes[0x51] = &loaddc;
  Opcodes[0x52] = &loaddd;
  Opcodes[0x53] = &loadde;
  Opcodes[0x54] = &loaddh;
  Opcodes[0x55] = &loaddl;
  Opcodes[0x56] = &opcode_0x56;
  Opcodes[0x57] = &loadda;
  Opcodes[0x58] = &loadeb;
  Opcodes[0x59] = &loadec;
  Opcodes[0x5A] = &loaded;
  Opcodes[0x5B] = &loadee;
  Opcodes[0x5C] = &loadeh;
  Opcodes[0x5D] = &loadel;
  Opcodes[0x5E] = &opcode_0x5e;
  Opcodes[0x5F] = &loadea;

  Opcodes[0x60] = &loadhb;
  Opcodes[0x61] = &loadhc;
  Opcodes[0x62] = &loadhd;
  Opcodes[0x63] = &loadhe;
  Opcodes[0x64] = &loadhh;
  Opcodes[0x65] = &loadhl;
  Opcodes[0x66] = &opcode_0x66;
  Opcodes[0x67] = &loadha;
  Opcodes[0x68] = &loadlb;
  Opcodes[0x69] = &loadlc;
  Opcodes[0x6A] = &loadld;
  Opcodes[0x6B] = &loadle;
  Opcodes[0x6C] = &loadlh;
  Opcodes[0x6D] = &loadll;
  Opcodes[0x6E] = &opcode_0x6e;
  Opcodes[0x6F] = &loadla;

  Opcodes[0x70] = &opcode_0x70;
  Opcodes[0x71] = &opcode_0x71;
  Opcodes[0x72] = &opcode_0x72;
  Opcodes[0x73] = &opcode_0x73;
  Opcodes[0x74] = &opcode_0x74;
  Opcodes[0x75] = &opcode_0x75;
  Opcodes[0x76] = &opcode_0x76;
  Opcodes[0x77] = &opcode_0x77;
  Opcodes[0x78] = &loadab;
  Opcodes[0x79] = &loadac;
  Opcodes[0x7A] = &loadad;
  Opcodes[0x7B] = &loadae;
  Opcodes[0x7C] = &loadah;
  Opcodes[0x7D] = &loadal;
  Opcodes[0x7E] = &opcode_0x7e;
  Opcodes[0x7F] = &loadaa;

  Opcodes[0x80] = &opcode_0x80;
  Opcodes[0x81] = &opcode_0x81;
  Opcodes[0x82] = &opcode_0x82;
  Opcodes[0x83] = &opcode_0x83;
  Opcodes[0x84] = &opcode_0x84;
  Opcodes[0x85] = &opcode_0x85;
  Opcodes[0x86] = &opcode_0x86;
  Opcodes[0x87] = &opcode_0x87;
  Opcodes[0x88] = &opcode_0x88;
  Opcodes[0x89] = &opcode_0x89;
  Opcodes[0x8a] = &opcode_0x8a;
  Opcodes[0x8b] = &opcode_0x8b;
  Opcodes[0x8c] = &opcode_0x8c;
  Opcodes[0x8d] = &opcode_0x8d;
  Opcodes[0x8e] = &opcode_0x8e;
  Opcodes[0x8f] = &opcode_0x8f;

  Opcodes[0x90] = &opcode_0x90;
  Opcodes[0x91] = &opcode_0x91;
  Opcodes[0x92] = &opcode_0x92;
  Opcodes[0x93] = &opcode_0x93;
  Opcodes[0x94] = &opcode_0x94;
  Opcodes[0x95] = &opcode_0x95;
  Opcodes[0x96] = &opcode_0x96;
  Opcodes[0x97] = &opcode_0x97;
  Opcodes[0x98] = &opcode_0x98;
  Opcodes[0x99] = &opcode_0x99;
  Opcodes[0x9a] = &opcode_0x9a;
  Opcodes[0x9b] = &opcode_0x9b;
  Opcodes[0x9c] = &opcode_0x9c;
  Opcodes[0x9d] = &opcode_0x9d;
  Opcodes[0x9e] = &opcode_0x9e;
  Opcodes[0x9f] = &opcode_0x9f;

  Opcodes[0xA0] = &opcode_0xa0;
  Opcodes[0xA1] = &opcode_0xa1;
  Opcodes[0xA2] = &opcode_0xa2;
  Opcodes[0xA3] = &opcode_0xa3;
  Opcodes[0xA4] = &opcode_0xa4;
  Opcodes[0xA5] = &opcode_0xa5;
  Opcodes[0xA6] = &opcode_0xa6;
  Opcodes[0xA7] = &opcode_0xa7;
  Opcodes[0xA8] = &opcode_0xa8;
  Opcodes[0xA9] = &opcode_0xa9;
  Opcodes[0xAA] = &opcode_0xaa;
  Opcodes[0xAB] = &opcode_0xab;
  Opcodes[0xAC] = &opcode_0xac;
  Opcodes[0xAD] = &opcode_0xad;
  Opcodes[0xAE] = &opcode_0xae;
  Opcodes[0xAF] = &opcode_0xaf;

  Opcodes[0xB0] = &opcode_0xb0;
  Opcodes[0xB1] = &opcode_0xb1;
  Opcodes[0xB2] = &opcode_0xb2;
  Opcodes[0xB3] = &opcode_0xb3;
  Opcodes[0xB4] = &opcode_0xb4;
  Opcodes[0xB5] = &opcode_0xb5;
  Opcodes[0xB6] = &opcode_0xb6;
  Opcodes[0xB7] = &opcode_0xb7;
  Opcodes[0xB8] = &opcode_0xb8;
  Opcodes[0xB9] = &opcode_0xb9;
  Opcodes[0xBA] = &opcode_0xba;
  Opcodes[0xBB] = &opcode_0xbb;
  Opcodes[0xBC] = &opcode_0xbc;
  Opcodes[0xBD] = &opcode_0xbd;
  Opcodes[0xBE] = &opcode_0xbe;
  Opcodes[0xBF] = &opcode_0xbf;

  Opcodes[0xC0] = &opcode_0xc0;
  Opcodes[0xC1] = &opcode_0xc1;
  Opcodes[0xC2] = &opcode_0xc2;
  Opcodes[0xC3] = &opcode_0xc3;
  Opcodes[0xC4] = &opcode_0xc4;
  Opcodes[0xC5] = &opcode_0xc5;
  Opcodes[0xC6] = &opcode_0xc6;
  Opcodes[0xC7] = &opcode_0xc7;
  Opcodes[0xC8] = &opcode_0xc8;
  Opcodes[0xC9] = &opcode_0xc9;
  Opcodes[0xCA] = &opcode_0xca;
  Opcodes[0xCB] = &prefixcb;
  Opcodes[0xCC] = &opcode_0xcc;
  Opcodes[0xCD] = &opcode_0xcd;
  Opcodes[0xCE] = &opcode_0xce;
  Opcodes[0xCF] = &opcode_0xcf;

  Opcodes[0xD0] = &opcode_0xd0;
  Opcodes[0xD1] = &opcode_0xd1;
  Opcodes[0xD2] = &opcode_0xd2;
  Opcodes[0xD5] = &opcode_0xd5;
  Opcodes[0xD6] = &opcode_0xd6;
  Opcodes[0xD7] = &opcode_0xd7;
  Opcodes[0xD8] = &opcode_0xd8;
  Opcodes[0xD9] = &opcode_0xd9;
  Opcodes[0xDA] = &opcode_0xda;
  Opcodes[0xDC] = &opcode_0xdc;
  Opcodes[0xDE] = &opcode_0xde;
  Opcodes[0xDF] = &opcode_0xdf;

  Opcodes[0xE0] = &opcode_0xe0;
  Opcodes[0xE1] = &opcode_0xe1;
  Opcodes[0xE2] = &opcode_0xe2;
  Opcodes[0xE5] = &opcode_0xe5;
  Opcodes[0xE6] = &opcode_0xe6;
  Opcodes[0xE7] = &opcode_0xe7;
  Opcodes[0xE8] = &opcode_0xe8;
  Opcodes[0xE9] = &opcode_0xe9;
  Opcodes[0xEA] = &opcode_0xea;
  Opcodes[0xEE] = &opcode_0xee;
  Opcodes[0xEF] = &opcode_0xef;

  Opcodes[0xF0] = &opcode_0xf0;
  Opcodes[0xF1] = &opcode_0xf1;
  Opcodes[0xF3] = &opcode_0xf3;
  Opcodes[0xF5] = &opcode_0xf5;
  Opcodes[0xF6] = &opcode_0xf6;
  Opcodes[0xF7] = &opcode_0xf7;
  Opcodes[0xF8] = &opcode_0xf8;
  Opcodes[0xF9] = &opcode_0xf9;
  Opcodes[0xFA] = &opcode_0xfa;
  Opcodes[0xFB] = &opcode_0xfb;
  Opcodes[0xFE] = &opcode_0xfe;
  Opcodes[0xFF] = &opcode_0xff;
}

void load_prefixcb(void)
{
  PrefixCB[0x10] = &prefix_0x10;
  PrefixCB[0x11] = &prefix_0x11;
  PrefixCB[0x12] = &prefix_0x12;
  PrefixCB[0x13] = &prefix_0x13;
  PrefixCB[0x14] = &prefix_0x14;
  PrefixCB[0x15] = &prefix_0x15;
  PrefixCB[0x16] = &prefix_0x16;
  PrefixCB[0x17] = &prefix_0x17;
  PrefixCB[0x18] = &prefix_0x18;
  PrefixCB[0x19] = &prefix_0x19;
  PrefixCB[0x1a] = &prefix_0x1a;
  PrefixCB[0x1b] = &prefix_0x1b;
  PrefixCB[0x1c] = &prefix_0x1c;
  PrefixCB[0x1d] = &prefix_0x1d;
  PrefixCB[0x1e] = &prefix_0x1e;
  PrefixCB[0x1f] = &prefix_0x1f;

  PrefixCB[0x20] = &prefix_0x20;
  PrefixCB[0x21] = &prefix_0x21;
  PrefixCB[0x22] = &prefix_0x22;
  PrefixCB[0x23] = &prefix_0x23;
  PrefixCB[0x24] = &prefix_0x24;
  PrefixCB[0x25] = &prefix_0x25;
  PrefixCB[0x26] = &prefix_0x26;
  PrefixCB[0x27] = &prefix_0x27;
  PrefixCB[0x28] = &prefix_0x28;
  PrefixCB[0x29] = &prefix_0x29;
  PrefixCB[0x2a] = &prefix_0x2a;
  PrefixCB[0x2b] = &prefix_0x2b;
  PrefixCB[0x2c] = &prefix_0x2c;
  PrefixCB[0x2d] = &prefix_0x2d;
  PrefixCB[0x2e] = &prefix_0x2e;
  PrefixCB[0x2f] = &prefix_0x2f;

  PrefixCB[0x30] = &prefix_0x30;
  PrefixCB[0x31] = &prefix_0x31;
  PrefixCB[0x32] = &prefix_0x32;
  PrefixCB[0x33] = &prefix_0x33;
  PrefixCB[0x34] = &prefix_0x34;
  PrefixCB[0x35] = &prefix_0x35;
  PrefixCB[0x36] = &prefix_0x36;
  PrefixCB[0x37] = &prefix_0x37;
  PrefixCB[0x38] = &prefix_0x38;
  PrefixCB[0x39] = &prefix_0x39;
  PrefixCB[0x3a] = &prefix_0x3a;
  PrefixCB[0x3b] = &prefix_0x3b;
  PrefixCB[0x3c] = &prefix_0x3c;
  PrefixCB[0x3d] = &prefix_0x3d;
  PrefixCB[0x3e] = &prefix_0x3e;
  PrefixCB[0x3f] = &prefix_0x3f;

  PrefixCB[0x40] = &prefix_0x40;
  PrefixCB[0x41] = &prefix_0x41;
  PrefixCB[0x42] = &prefix_0x42;
  PrefixCB[0x43] = &prefix_0x43;
  PrefixCB[0x44] = &prefix_0x44;
  PrefixCB[0x45] = &prefix_0x45;
  PrefixCB[0x46] = &prefix_0x46;
  PrefixCB[0x47] = &prefix_0x47;
  PrefixCB[0x48] = &prefix_0x48;
  PrefixCB[0x49] = &prefix_0x49;
  PrefixCB[0x4A] = &prefix_0x4a;
  PrefixCB[0x4B] = &prefix_0x4b;
  PrefixCB[0x4C] = &prefix_0x4c;
  PrefixCB[0x4D] = &prefix_0x4d;
  PrefixCB[0x4E] = &prefix_0x4e;
  PrefixCB[0x4F] = &prefix_0x4f;

  PrefixCB[0x50] = &prefix_0x50;
  PrefixCB[0x51] = &prefix_0x51;
  PrefixCB[0x52] = &prefix_0x52;
  PrefixCB[0x53] = &prefix_0x53;
  PrefixCB[0x54] = &prefix_0x54;
  PrefixCB[0x55] = &prefix_0x55;
  PrefixCB[0x56] = &prefix_0x56;
  PrefixCB[0x57] = &prefix_0x57;
  PrefixCB[0x58] = &prefix_0x58;
  PrefixCB[0x59] = &prefix_0x59;
  PrefixCB[0x5A] = &prefix_0x5a;
  PrefixCB[0x5B] = &prefix_0x5b;
  PrefixCB[0x5C] = &prefix_0x5c;
  PrefixCB[0x5D] = &prefix_0x5d;
  PrefixCB[0x5E] = &prefix_0x5e;
  PrefixCB[0x5F] = &prefix_0x5f;

  PrefixCB[0x60] = &prefix_0x60;
  PrefixCB[0x61] = &prefix_0x61;
  PrefixCB[0x62] = &prefix_0x62;
  PrefixCB[0x63] = &prefix_0x63;
  PrefixCB[0x64] = &prefix_0x64;
  PrefixCB[0x65] = &prefix_0x65;
  PrefixCB[0x66] = &prefix_0x66;
  PrefixCB[0x67] = &prefix_0x67;
  PrefixCB[0x68] = &prefix_0x68;
  PrefixCB[0x69] = &prefix_0x69;
  PrefixCB[0x6A] = &prefix_0x6a;
  PrefixCB[0x6B] = &prefix_0x6b;
  PrefixCB[0x6C] = &prefix_0x6c;
  PrefixCB[0x6D] = &prefix_0x6d;
  PrefixCB[0x6E] = &prefix_0x6e;
  PrefixCB[0x6F] = &prefix_0x6f;

  PrefixCB[0x70] = &prefix_0x70;
  PrefixCB[0x71] = &prefix_0x71;
  PrefixCB[0x72] = &prefix_0x72;
  PrefixCB[0x73] = &prefix_0x73;
  PrefixCB[0x74] = &prefix_0x74;
  PrefixCB[0x75] = &prefix_0x75;
  PrefixCB[0x76] = &prefix_0x76;
  PrefixCB[0x77] = &prefix_0x77;
  PrefixCB[0x78] = &prefix_0x78;
  PrefixCB[0x79] = &prefix_0x79;
  PrefixCB[0x7A] = &prefix_0x7a;
  PrefixCB[0x7B] = &prefix_0x7b;
  PrefixCB[0x7C] = &prefix_0x7c;
  PrefixCB[0x7D] = &prefix_0x7d;
  PrefixCB[0x7E] = &prefix_0x7e;
  PrefixCB[0x7F] = &prefix_0x7f;

  PrefixCB[0x80] = &prefix_0x80;
  PrefixCB[0x81] = &prefix_0x81;
  PrefixCB[0x82] = &prefix_0x82;
  PrefixCB[0x83] = &prefix_0x83;
  PrefixCB[0x84] = &prefix_0x84;
  PrefixCB[0x85] = &prefix_0x85;
  PrefixCB[0x86] = &prefix_0x86;
  PrefixCB[0x87] = &prefix_0x87;
  PrefixCB[0x88] = &prefix_0x88;
  PrefixCB[0x89] = &prefix_0x89;
  PrefixCB[0x8A] = &prefix_0x8a;
  PrefixCB[0x8B] = &prefix_0x8b;
  PrefixCB[0x8C] = &prefix_0x8c;
  PrefixCB[0x8D] = &prefix_0x8d;
  PrefixCB[0x8E] = &prefix_0x8e;
  PrefixCB[0x8F] = &prefix_0x8f;

  PrefixCB[0x90] = &prefix_0x90;
  PrefixCB[0x91] = &prefix_0x91;
  PrefixCB[0x92] = &prefix_0x92;
  PrefixCB[0x93] = &prefix_0x93;
  PrefixCB[0x94] = &prefix_0x94;
  PrefixCB[0x95] = &prefix_0x95;
  PrefixCB[0x96] = &prefix_0x96;
  PrefixCB[0x97] = &prefix_0x97;
  PrefixCB[0x98] = &prefix_0x98;
  PrefixCB[0x99] = &prefix_0x99;
  PrefixCB[0x9A] = &prefix_0x9a;
  PrefixCB[0x9B] = &prefix_0x9b;
  PrefixCB[0x9C] = &prefix_0x9c;
  PrefixCB[0x9D] = &prefix_0x9d;
  PrefixCB[0x9E] = &prefix_0x9e;
  PrefixCB[0x9F] = &prefix_0x9f;

  PrefixCB[0xa0] = &prefix_0xa0;
  PrefixCB[0xa1] = &prefix_0xa1;
  PrefixCB[0xa2] = &prefix_0xa2;
  PrefixCB[0xa3] = &prefix_0xa3;
  PrefixCB[0xa4] = &prefix_0xa4;
  PrefixCB[0xa5] = &prefix_0xa5;
  PrefixCB[0xa6] = &prefix_0xa6;
  PrefixCB[0xa7] = &prefix_0xa7;
  PrefixCB[0xa8] = &prefix_0xa8;
  PrefixCB[0xa9] = &prefix_0xa9;
  PrefixCB[0xaA] = &prefix_0xaa;
  PrefixCB[0xaB] = &prefix_0xab;
  PrefixCB[0xaC] = &prefix_0xac;
  PrefixCB[0xaD] = &prefix_0xad;
  PrefixCB[0xaE] = &prefix_0xae;
  PrefixCB[0xaF] = &prefix_0xaf;

  PrefixCB[0xb0] = &prefix_0xb0;
  PrefixCB[0xb1] = &prefix_0xb1;
  PrefixCB[0xb2] = &prefix_0xb2;
  PrefixCB[0xb3] = &prefix_0xb3;
  PrefixCB[0xb4] = &prefix_0xb4;
  PrefixCB[0xb5] = &prefix_0xb5;
  PrefixCB[0xb6] = &prefix_0xb6;
  PrefixCB[0xb7] = &prefix_0xb7;
  PrefixCB[0xb8] = &prefix_0xb8;
  PrefixCB[0xb9] = &prefix_0xb9;
  PrefixCB[0xbA] = &prefix_0xba;
  PrefixCB[0xbB] = &prefix_0xbb;
  PrefixCB[0xbC] = &prefix_0xbc;
  PrefixCB[0xbD] = &prefix_0xbd;
  PrefixCB[0xbE] = &prefix_0xbe;
  PrefixCB[0xbF] = &prefix_0xbf;

  PrefixCB[0xc0] = &prefix_0xc0;
  PrefixCB[0xc1] = &prefix_0xc1;
  PrefixCB[0xc2] = &prefix_0xc2;
  PrefixCB[0xc3] = &prefix_0xc3;
  PrefixCB[0xc4] = &prefix_0xc4;
  PrefixCB[0xc5] = &prefix_0xc5;
  PrefixCB[0xc6] = &prefix_0xc6;
  PrefixCB[0xc7] = &prefix_0xc7;
  PrefixCB[0xc8] = &prefix_0xc8;
  PrefixCB[0xc9] = &prefix_0xc9;
  PrefixCB[0xcA] = &prefix_0xca;
  PrefixCB[0xcB] = &prefix_0xcb;
  PrefixCB[0xcC] = &prefix_0xcc;
  PrefixCB[0xcD] = &prefix_0xcd;
  PrefixCB[0xcE] = &prefix_0xce;
  PrefixCB[0xcF] = &prefix_0xcf;

  PrefixCB[0xd0] = &prefix_0xd0;
  PrefixCB[0xd1] = &prefix_0xd1;
  PrefixCB[0xd2] = &prefix_0xd2;
  PrefixCB[0xd3] = &prefix_0xd3;
  PrefixCB[0xd4] = &prefix_0xd4;
  PrefixCB[0xd5] = &prefix_0xd5;
  PrefixCB[0xd6] = &prefix_0xd6;
  PrefixCB[0xd7] = &prefix_0xd7;
  PrefixCB[0xd8] = &prefix_0xd8;
  PrefixCB[0xd9] = &prefix_0xd9;
  PrefixCB[0xdA] = &prefix_0xda;
  PrefixCB[0xdB] = &prefix_0xdb;
  PrefixCB[0xdC] = &prefix_0xdc;
  PrefixCB[0xdD] = &prefix_0xdd;
  PrefixCB[0xdE] = &prefix_0xde;
  PrefixCB[0xdF] = &prefix_0xdf;

  PrefixCB[0xe0] = &prefix_0xe0;
  PrefixCB[0xe1] = &prefix_0xe1;
  PrefixCB[0xe2] = &prefix_0xe2;
  PrefixCB[0xe3] = &prefix_0xe3;
  PrefixCB[0xe4] = &prefix_0xe4;
  PrefixCB[0xe5] = &prefix_0xe5;
  PrefixCB[0xe6] = &prefix_0xe6;
  PrefixCB[0xe7] = &prefix_0xe7;
  PrefixCB[0xe8] = &prefix_0xe8;
  PrefixCB[0xe9] = &prefix_0xe9;
  PrefixCB[0xeA] = &prefix_0xea;
  PrefixCB[0xeB] = &prefix_0xeb;
  PrefixCB[0xeC] = &prefix_0xec;
  PrefixCB[0xeD] = &prefix_0xed;
  PrefixCB[0xeE] = &prefix_0xee;
  PrefixCB[0xeF] = &prefix_0xef;

  PrefixCB[0xf0] = &prefix_0xf0;
  PrefixCB[0xf1] = &prefix_0xf1;
  PrefixCB[0xf2] = &prefix_0xf2;
  PrefixCB[0xf3] = &prefix_0xf3;
  PrefixCB[0xf4] = &prefix_0xf4;
  PrefixCB[0xf5] = &prefix_0xf5;
  PrefixCB[0xf6] = &prefix_0xf6;
  PrefixCB[0xf7] = &prefix_0xf7;
  PrefixCB[0xf8] = &prefix_0xf8;
  PrefixCB[0xf9] = &prefix_0xf9;
  PrefixCB[0xfA] = &prefix_0xfa;
  PrefixCB[0xfB] = &prefix_0xfb;
  PrefixCB[0xfC] = &prefix_0xfc;
  PrefixCB[0xfD] = &prefix_0xfd;
  PrefixCB[0xfE] = &prefix_0xfe;
  PrefixCB[0xfF] = &prefix_0xff;
}

void update_timers(void)
{
  if (my_clock.divider > 255)
  {
      my_clock.divider = 0;
      MMU.memory[0xFF04]++;
  }

  if (test_bit(read_memory(0xFF07), 2))
  {
    my_clock.timer_counter -= my_clock.m;
    if (my_clock.timer_counter <= 0)
    {
      switch(read_memory(0xFF07) & 0x3)
      {
        case 0: my_clock.timer_counter = 1024; break;
        case 1: my_clock.timer_counter = 16; break;
        case 2: my_clock.timer_counter = 64; break;
        case 3: my_clock.timer_counter = 256; break;
      }

      if (read_memory(0xFF05) == 255)
      {
          write_memory(0xFF05, read_memory(0xFF06));
          request_interupt(2);
      }
      else
      {
        write_memory(0xFF05, read_memory(0xFF05) + 1);
      }
    }
  }
}

static struct timespec start, end;
static void my_clock_handling(uint8_t pixels[], int *display)
{
  my_clock.total_m += my_clock.m;
  my_clock.total_t += my_clock.t;
  my_clock.lineticks += my_clock.m;
  my_clock.divider += my_clock.m;
  update_timers();

  switch (my_clock.mode)
  {
    case 0:
      if (my_clock.lineticks > 203)
      {
        if (read_memory(0xFF44) == 143)
        {
          my_clock.mode = 1;
          write_memory(0xFF41, read_memory(0xFF41) | (1 << 0));
          write_memory(0xFF41, read_memory(0xFF41) & ~(1 << 1));
          if (test_bit(read_memory(0xFF41), 4))
            request_interupt(1);
        }
        else
        {
          my_clock.mode = 2;
          write_memory(0xFF41, read_memory(0xFF41) | (1 << 1));
          write_memory(0xFF41, read_memory(0xFF41) & ~(1 << 0));
          if (test_bit(read_memory(0xFF41), 5))
            request_interupt(1);
        }
        my_clock.lineticks = 0;

        uint8_t flag = read_memory(0xFF40);
        if (test_bit(flag, 0))
          print_tiles(pixels);
        if (test_bit(flag, 1))
          print_sprites(pixels);

        write_memory(0xFF44, read_memory(0xFF44) + 1);
      }
      break;
    case 1: // VBLANK
      if (my_clock.lineticks >= 456 && read_memory(0xFF44) == 153)
      {
        my_clock.mode = 2;
        my_clock.lineticks = 0;
        write_memory(0xFF44, 0);
        request_interupt(0);

        // BEGIN SYNCHRONIZED DISPLAY LOGIC
        clock_gettime(CLOCK_MONOTONIC_RAW, &end);

        struct timespec sleep;
        sleep.tv_sec = 0;
        // 20ms
        sleep.tv_nsec = 16600000L - (end.tv_nsec - start.tv_nsec);

        if (sleep.tv_nsec > 0 && sleep.tv_nsec < 16600000L)
          nanosleep(&sleep, NULL);

        my_clock.total_m  = 0;

        *display = 1;

        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
        // END SYNCHRONIZED DISPLAY LOGIC
      }
      else if (my_clock.lineticks >= 456)
      {
        my_clock.lineticks = 0;
        write_memory(0xFF44, read_memory(0xFF44) + 1);
      }
      break;
    case 2:
      if (my_clock.lineticks > 80)
      {
        my_clock.mode = 3;
        my_clock.lineticks = 0;
        write_memory(0xFF41, read_memory(0xFF41) | (1 << 0));
        write_memory(0xFF41, read_memory(0xFF41) | (1 << 1));
      }
      break;
    case 3:
      if (my_clock.lineticks > 172)
      {
        my_clock.mode = 0;
        my_clock.lineticks = 0;
        write_memory(0xFF41, read_memory(0xFF41) & ~(1 << 0));
        write_memory(0xFF41, read_memory(0xFF41) & ~(1 << 1));
        if (test_bit(read_memory(0xFF41), 3))
          request_interupt(1);
      }
      break;
  }

  // check the conincidence flag
   if (read_memory(0xFF44) == read_memory(0xFF45))
   {
     write_memory(0xFF41, read_memory(0xFF41) | (1 << 2));
     if (test_bit(read_memory(0xFF41), 6))
       request_interupt(1);
   }
   else
   {
     write_memory(0xFF41, read_memory(0xFF41) & ~(1 << 2));
   }
}

void execute(uint16_t op, uint8_t pixels[], int *display)
{
  if (!MMU.HALT)
  {
    if (!Opcodes[op])
    {
      fprintf(stderr, "Unknown Op %x at address %x", op, r.PC.val - 1);
      exit(1);
    }
    Opcodes[op]();
  }

  my_clock_handling(pixels, display);
}
