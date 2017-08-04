#include "utils.h"

extern Mmu MMU;
extern Registers r;
extern My_clock my_clock;
extern void (*Opcodes[0xFF]) (void);

void init(void)
{
    init_registers();
    load_opcodes();
    load_prefixcb();
    init_mmu("misc/bios.bin");
}

// NOP
void opcode_0x00(void)
{
  my_clock.m = 1;
  my_clock.t = 4;
}

// POP BC
void opcode_0xc1(void)
{
  pop_op(&r.BC.val);
}

// POP DE
void opcode_0xd1(void)
{
  pop_op(&r.DE.val);
}

// POP HL
void opcode_0xe1(void)
{
  pop_op(&r.HL.val);
}

// POP AF
void opcode_0xf1(void)
{
  pop_op(&r.AF.val);
}

// PUSH BC
void opcode_0xc5(void)
{
  push_op(r.BC.val);
}

// PUSH DE
void opcode_0xd5(void)
{
  push_op(r.DE.val);
}

// PUSH HL
void opcode_0xe5(void)
{
  push_op(r.HL.val);
}

// PUSH AF
void opcode_0xf5(void)
{
  push_op(r.AF.val);
}

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
      fprintf(stderr, "Unknown Prefix Op: %x", op);
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

// INC C
void opcode_0x0c(void)
{
  inc_op(&r.BC.bytes.low);
}

// INC E
void opcode_0x1c(void)
{
  inc_op(&r.DE.bytes.low);
}

// INC L
void opcode_0x2c(void)
{
  inc_op(&r.HL.bytes.low);
}

// INC A
void opcode_0x3c(void)
{
  inc_op(&r.AF.bytes.high);
}

// INC B
void opcode_0x04(void)
{
  inc_op(&r.BC.bytes.high);
}

// INC D
void opcode_0x14(void)
{
  inc_op(&r.DE.bytes.high);
}

// INC H
void opcode_0x24(void)
{
  inc_op(&r.HL.bytes.high);
}

// INC (HL)
void opcode_0x34(void)
{
  uint8_t hl = read_memory(r.HL.val);

  write_memory(r.HL.val, read_memory(r.HL.val) + 1);
  read_memory(r.HL.val) == 0 ? setZ() : resetZ();
  ((read_memory(r.HL.val) ^ 0x01 ^ hl) & 0x10) ? setH() : resetH();
  resetN();

  my_clock.m = 1;
  my_clock.t = 12;
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
void opcode_0x2a(void)
{
  r.AF.bytes.high = read_memory(r.HL.val);
  r.HL.val++;

  my_clock.m = 1;
  my_clock.t = 8;
}

void opcode_0x36(void)
{
  write_memory(r.HL.val, read_byte());

  my_clock.m = 2;
  my_clock.t = 12;
}

void opcode_0x46(void)
{
  r.BC.bytes.high = read_memory(r.HL.val);

  my_clock.m = 1;
  my_clock.t = 8;
}

void opcode_0x56(void)
{
  r.DE.bytes.high = read_memory(r.HL.val);

  my_clock.m = 1;
  my_clock.t = 8;
}

void opcode_0x66(void)
{
  r.HL.bytes.high = read_memory(r.HL.val);

  my_clock.m = 1;
  my_clock.t = 8;
}

void opcode_0x0b(void)
{
  r.BC.val--;

  my_clock.m = 1;
  my_clock.t = 8;
}

void opcode_0x1b(void)
{
  r.DE.val--;

  my_clock.m = 1;
  my_clock.t = 8;
}

void opcode_0x2b(void)
{
  r.HL.val--;

  my_clock.m = 1;
  my_clock.t = 8;
}

void opcode_0x3b(void)
{
  r.SP.val--;

  my_clock.m = 1;
  my_clock.t = 8;
}

// LD A,(HL-)
void opcode_0x3a(void)
{
  r.AF.bytes.high = read_memory(r.HL.val);
  r.HL.val--;

  my_clock.m = 1;
  my_clock.t = 8;
}

// LD HL,d16
void opcode_0x21(void)
{
  r.HL.val = read_word();

  my_clock.m = 3;
  my_clock.t = 12;
}

// LD SP,d16
void opcode_0x31(void)
{
  r.SP.val = read_word();

  my_clock.m = 3;
  my_clock.t = 12;
}

void opcode_0x06(void)
{
  r.BC.bytes.high = read_byte();

  my_clock.m = 2;
  my_clock.t = 8;
}

void opcode_0x16(void)
{
  r.DE.bytes.high = read_byte();

  my_clock.m = 2;
  my_clock.t = 8;
}

void opcode_0x26(void)
{
  r.HL.bytes.high = read_byte();

  my_clock.m = 2;
  my_clock.t = 8;
}

// ADD HL, BC
void opcode_0x09(void)
{
  add_16_op(&r.HL.val, r.BC.val);
}

// ADD HL, DE
void opcode_0x19(void)
{
  add_16_op(&r.HL.val, r.DE.val);
}

// ADD HL, HL
void opcode_0x29(void)
{
  add_16_op(&r.HL.val, r.HL.val);
}

// ADD HL, SP
void opcode_0x39(void)
{
  add_16_op(&r.HL.val, r.SP.val);
}

// ADC A, d8
void opcode_0xce(void)
{
  add_8_op(&r.AF.bytes.high, read_byte() + getC());
  my_clock.m = 2;
  my_clock.t = 8;
}

// ADD A,B
void opcode_0x80(void)
{
  add_8_op(&r.AF.bytes.high, r.BC.bytes.high);
}

// ADD A,C
void opcode_0x81(void)
{
  add_8_op(&r.AF.bytes.high, r.BC.bytes.low);
}

// ADD A,D
void opcode_0x82(void)
{
  add_8_op(&r.AF.bytes.high, r.DE.bytes.high);
}

// ADD A,E
void opcode_0x83(void)
{
  add_8_op(&r.AF.bytes.high, r.DE.bytes.low);
}

// ADD A,H
void opcode_0x84(void)
{
  add_8_op(&r.AF.bytes.high, r.HL.bytes.high);
}

// ADD A,L
void opcode_0x85(void)
{
  add_8_op(&r.AF.bytes.high, r.HL.bytes.low);
}

// ADD A,(HL)
void opcode_0x86(void)
{
  add_8_op(&r.AF.bytes.high, read_memory(r.HL.val));
  my_clock.m = 1;
  my_clock.t = 8;
}

// ADD A,A
void opcode_0x87(void)
{
  add_8_op(&r.AF.bytes.high, r.AF.bytes.high);
}

// SUB B
void opcode_0x90(void)
{
  sub_8_op(&r.AF.bytes.high, r.BC.bytes.high);
}

// SUB C
void opcode_0x91(void)
{
  sub_8_op(&r.AF.bytes.high, r.BC.bytes.low);
}

// SUB D
void opcode_0x92(void)
{
  sub_8_op(&r.AF.bytes.high, r.DE.bytes.high);
}

// SUB E
void opcode_0x93(void)
{
  sub_8_op(&r.AF.bytes.high, r.DE.bytes.low);
}

// SUB H
void opcode_0x94(void)
{
  sub_8_op(&r.AF.bytes.high, r.HL.bytes.high);
}

// SUB L
void opcode_0x95(void)
{
  sub_8_op(&r.AF.bytes.high, r.HL.bytes.low);
}

// SUB (HL)
void opcode_0x96(void)
{
  sub_8_op(&r.AF.bytes.high, read_memory(r.HL.val));
  my_clock.m = 1;
  my_clock.t = 8;
}

// SUB A
void opcode_0x97(void)
{
  sub_8_op(&r.AF.bytes.high, r.AF.bytes.high);
}

// XOR B
void opcode_0xa8(void)
{
  xor_8_op(&r.AF.bytes.high, r.BC.bytes.high);
}

// XOR C
void opcode_0xa9(void)
{
  xor_8_op(&r.AF.bytes.high, r.BC.bytes.low);
}

// XOR D
void opcode_0xaa(void)
{
  xor_8_op(&r.AF.bytes.high, r.DE.bytes.high);
}

// XOR E
void opcode_0xab(void)
{
  xor_8_op(&r.AF.bytes.high, r.DE.bytes.low);
}

// XOR H
void opcode_0xac(void)
{
  xor_8_op(&r.AF.bytes.high, r.HL.bytes.high);
}

// XOR L
void opcode_0xad(void)
{
  xor_8_op(&r.AF.bytes.high, r.HL.bytes.low);
}

// XOR (HL)
void opcode_0xae(void)
{
  xor_8_op(&r.AF.bytes.high, read_memory(r.HL.val));
  my_clock.m = 1;
  my_clock.t = 8;
}

// XOR A
void opcode_0xaf(void)
{
  xor_8_op(&r.AF.bytes.high, r.AF.bytes.high);
}

// AND d8
void opcode_0xe6(void)
{
  and_op(&r.AF.bytes.high, read_byte());
  my_clock.m = 2;
  my_clock.t = 8;
}

// AND B
void opcode_0xa0(void)
{
  and_op(&r.AF.bytes.high, r.BC.bytes.high);
}

// AND C
void opcode_0xa1(void)
{
  and_op(&r.AF.bytes.high, r.BC.bytes.low);
}

// AND D
void opcode_0xa2(void)
{
  and_op(&r.AF.bytes.high, r.DE.bytes.high);
}

// AND E
void opcode_0xa3(void)
{
  and_op(&r.AF.bytes.high, r.DE.bytes.low);
}

// AND H
void opcode_0xa4(void)
{
  and_op(&r.AF.bytes.high, r.HL.bytes.high);
}

// AND L
void opcode_0xa5(void)
{
  and_op(&r.AF.bytes.high, r.HL.bytes.low);
}

// AND (HL)
void opcode_0xa6(void)
{
  and_op(&r.AF.bytes.high, read_memory(r.HL.val));
  my_clock.m = 1;
  my_clock.t = 8;
}

// AND A
void opcode_0xa7(void)
{
  and_op(&r.AF.bytes.high, r.AF.bytes.high);
}

// OR d8
void opcode_0xf6(void)
{
  or_op(&r.AF.bytes.high, read_byte());
  my_clock.m = 2;
  my_clock.t = 8;
}

// OR B
void opcode_0xb0(void)
{
  or_op(&r.AF.bytes.high, r.BC.bytes.high);
}

// OR C
void opcode_0xb1(void)
{
  or_op(&r.AF.bytes.high, r.BC.bytes.low);
}

// OR D
void opcode_0xb2(void)
{
  or_op(&r.AF.bytes.high, r.DE.bytes.high);
}

// OR E
void opcode_0xb3(void)
{
  or_op(&r.AF.bytes.high, r.DE.bytes.low);
}

// OR H
void opcode_0xb4(void)
{
  or_op(&r.AF.bytes.high, r.HL.bytes.high);
}

// OR L
void opcode_0xb5(void)
{
  or_op(&r.AF.bytes.high, r.HL.bytes.low);
}

// OR (HL)
void opcode_0xb6(void)
{
  or_op(&r.AF.bytes.high, read_memory(r.HL.val));
  my_clock.m = 1;
  my_clock.t = 8;
}

// OR A
void opcode_0xb7(void)
{
  or_op(&r.AF.bytes.high, r.AF.bytes.high);
}

// CP d8
void opcode_0xfe(void)
{
  cp_op(r.AF.bytes.high, read_byte());
  my_clock.m = 2;
  my_clock.t = 8;
}

// CP B
void opcode_0xb8(void)
{
  cp_op(r.AF.bytes.high, r.BC.bytes.high);
}

// CP C
void opcode_0xb9(void)
{
  cp_op(r.AF.bytes.high, r.BC.bytes.low);
}

// CP D
void opcode_0xba(void)
{
  cp_op(r.AF.bytes.high, r.DE.bytes.high);
}

// CP E
void opcode_0xbb(void)
{
  cp_op(r.AF.bytes.high, r.DE.bytes.low);
}

// CP H
void opcode_0xbc(void)
{
  cp_op(r.AF.bytes.high, r.HL.bytes.high);
}

// CP L
void opcode_0xbd(void)
{
  cp_op(r.AF.bytes.high, r.HL.bytes.low);
}

// CP (HL)
void opcode_0xbe(void)
{
  cp_op(r.AF.bytes.high, read_memory(r.HL.val));
  my_clock.m = 1;
  my_clock.t = 8;
}

// CP A
void opcode_0xbf(void)
{
  cp_op(r.AF.bytes.high, r.AF.bytes.high);
}

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

// JR r8
void opcode_0x18(void)
{
  int8_t addr = read_byte();
  r.PC.val += addr;

  my_clock.m = 2;
  my_clock.t = 12;
}

// JR Z,r8
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

// RLA
void opcode_0x17(void)
{
  uint8_t old_c = (r.AF.bytes.high & 0b10000000) >> 7;
  uint8_t carry = getC();

  r.AF.bytes.high = (r.AF.bytes.high << 1) + carry;
  old_c == 1 ? setC() : resetC();

  resetZ();
  resetN();
  resetH();

  my_clock.m = 2;
  my_clock.t = 8;
}

// DEC C
void opcode_0x0d(void)
{
  dec_op(&r.BC.bytes.low);
}

// DEC E
void opcode_0x1d(void)
{
  dec_op(&r.DE.bytes.low);
}

// DEC L
void opcode_0x2d(void)
{
  dec_op(&r.HL.bytes.low);
}

// DEC A
void opcode_0x3d(void)
{
  dec_op(&r.AF.bytes.high);
}

// DEC B
void opcode_0x05(void)
{
  dec_op(&r.BC.bytes.high);
}

// DEC E
void opcode_0x15(void)
{
  dec_op(&r.DE.bytes.high);
}

// DEC H
void opcode_0x25(void)
{
  dec_op(&r.HL.bytes.high);
}

// DEC (HL)
void opcode_0x35(void)
{
  uint8_t hl = read_memory(r.HL.val);
  if (hl == 0)
    setH();

  write_memory(r.HL.val, read_memory(r.HL.val) - 1);
  read_memory(r.HL.val) == 0 ? setZ() : resetZ();
  setN();

  my_clock.m = 1;
  my_clock.t = 4;
}

// INC BC
void opcode_0x03(void)
{
  add_16_op(&r.BC.val, 1);
}

// INC DE
void opcode_0x13(void)
{
  add_16_op(&r.DE.val, 1);
}

// INC HL
void opcode_0x23(void)
{
  add_16_op(&r.HL.val, 1);
}

// INC SP
void opcode_0x33(void)
{
  add_16_op(&r.SP.val, 1);
}

// RET
void opcode_0xc9(void)
{
  pop_op(&r.PC.val);
  my_clock.m = 1;
  my_clock.t = 16;
}

// RETI (return then enable interupt)
void opcode_0xd9(void)
{
  opcode_0xc9();
  r.ime = 1;
}

// RET NZ
void opcode_0xc0(void)
{
  ret_cond_op(!getZ());
}

// RET Z
void opcode_0xc8(void)
{
  ret_cond_op(getZ());
}

// RET NC
void opcode_0xd0(void)
{
  ret_cond_op(!getC());
}

// RET C
void opcode_0xd8(void)
{
  ret_cond_op(getC());
}

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
  r.AF.bytes.high = ~r.AF.bytes.high;
  setN();
  setH();

  my_clock.m = 1;
  my_clock.t = 4;
}

// RST 00
void opcode_0xc7(void)
{
  rst_op(0);
}

// RST 08
void opcode_0xcf(void)
{
  rst_op(0x0800);
}

// RST 10
void opcode_0xd7(void)
{
  rst_op(0x1000);
}

// RST 18
void opcode_0xdf(void)
{
  rst_op(0x1800);
}

// RST 20
void opcode_0xe7(void)
{
  rst_op(0x2000);
}

// RST 28
void opcode_0xef(void)
{
  rst_op(0x2800);
}

// RST 30
void opcode_0xf7(void)
{
  rst_op(0x3000);
}

// RST 38
void opcode_0xff(void)
{
  rst_op(0x3800);
}

void prefix_0x11(void)
{
  uint8_t old_c = (r.BC.bytes.low & 0b10000000) >> 7;
  uint8_t carry = getC();

  r.BC.bytes.low = (r.BC.bytes.low << 1) + carry;
  r.BC.bytes.low == 0 ? setZ() : resetZ();
  old_c == 1 ? setC() : resetC();

  resetN();
  resetH();

  my_clock.m = 2;
  my_clock.t = 8;
}

// SWAP B
void prefix_0x30(void)
{
  swap_op(&r.BC.bytes.high);
}

// SWAP C
void prefix_0x31(void)
{
  swap_op(&r.BC.bytes.low);
}

// SWAP D
void prefix_0x32(void)
{
  swap_op(&r.DE.bytes.high);
}

// SWAP E
void prefix_0x33(void)
{
swap_op(&r.DE.bytes.low);
}

// SWAP H
void prefix_0x34(void)
{
  swap_op(&r.HL.bytes.high);
}

// SWAP L
void prefix_0x35(void)
{
  swap_op(&r.HL.bytes.low);
}

// SWAP (HL)
void prefix_0x36(void)
{
  uint8_t high = read_memory(r.HL.val) >> 4;
  write_memory(r.HL.val, read_memory(r.HL.val) << 4);
  write_memory(r.HL.val, read_memory(r.HL.val) + high);

  read_memory(r.HL.val) == 0 ? setZ() : resetZ();
  resetC();
  resetN();
  resetH();

  my_clock.m = 2;
  my_clock.t = 16;
}

// SWAP A
void prefix_0x37(void)
{
  swap_op(&r.AF.bytes.high);
}

void load_opcodes(void)
{
  Opcodes[0x00] = &opcode_0x00;
  Opcodes[0x01] = &opcode_0x01;
  Opcodes[0x02] = &opcode_0x02;
  Opcodes[0x03] = &opcode_0x03;
  Opcodes[0x04] = &opcode_0x04;
  Opcodes[0x05] = &opcode_0x05;
  Opcodes[0x06] = &opcode_0x06;
  Opcodes[0x09] = &opcode_0x09;
  Opcodes[0x0A] = &opcode_0x0a;
  Opcodes[0x0B] = &opcode_0x0b;
  Opcodes[0x0C] = &opcode_0x0c;
  Opcodes[0x0D] = &opcode_0x0d;
  Opcodes[0x0E] = &loadcd8;

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

  Opcodes[0x20] = &opcode_0x20;
  Opcodes[0x21] = &opcode_0x21;
  Opcodes[0x22] = &loadhlpa;
  Opcodes[0x23] = &opcode_0x23;
  Opcodes[0x24] = &opcode_0x24;
  Opcodes[0x25] = &opcode_0x25;
  Opcodes[0x26] = &opcode_0x26;
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
  Opcodes[0x6F] = &loadla;

  Opcodes[0x77] = &opcode_0x77;
  Opcodes[0x78] = &loadab;
  Opcodes[0x79] = &loadac;
  Opcodes[0x7A] = &loadad;
  Opcodes[0x7B] = &loadae;
  Opcodes[0x7C] = &loadah;
  Opcodes[0x7D] = &loadal;
  Opcodes[0x7F] = &loadaa;

  Opcodes[0x80] = &opcode_0x80;
  Opcodes[0x81] = &opcode_0x81;
  Opcodes[0x82] = &opcode_0x82;
  Opcodes[0x83] = &opcode_0x83;
  Opcodes[0x84] = &opcode_0x84;
  Opcodes[0x85] = &opcode_0x85;
  Opcodes[0x86] = &opcode_0x86;
  Opcodes[0x87] = &opcode_0x87;

  Opcodes[0x90] = &opcode_0x90;
  Opcodes[0x91] = &opcode_0x91;
  Opcodes[0x92] = &opcode_0x92;
  Opcodes[0x93] = &opcode_0x93;
  Opcodes[0x94] = &opcode_0x94;
  Opcodes[0x95] = &opcode_0x95;
  Opcodes[0x96] = &opcode_0x96;
  Opcodes[0x97] = &opcode_0x97;

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
  Opcodes[0xC3] = &opcode_0xc3;
  Opcodes[0xC5] = &opcode_0xc5;
  Opcodes[0xC7] = &opcode_0xc7;
  Opcodes[0xC8] = &opcode_0xc8;
  Opcodes[0xC9] = &opcode_0xc9;
  Opcodes[0xCB] = &prefixcb;
  Opcodes[0xCC] = &opcode_0xcc;
  Opcodes[0xCD] = &opcode_0xcd;
  Opcodes[0xCE] = &opcode_0xce;
  Opcodes[0xCF] = &opcode_0xcf;

  Opcodes[0xD0] = &opcode_0xd0;
  Opcodes[0xD1] = &opcode_0xd1;
  Opcodes[0xD5] = &opcode_0xd5;
  Opcodes[0xD7] = &opcode_0xd7;
  Opcodes[0xD8] = &opcode_0xd8;
  Opcodes[0xD9] = &opcode_0xd9;
  Opcodes[0xDF] = &opcode_0xdf;

  Opcodes[0xE0] = &opcode_0xe0;
  Opcodes[0xE1] = &opcode_0xe1;
  Opcodes[0xE2] = &opcode_0xe2;
  Opcodes[0xE5] = &opcode_0xe5;
  Opcodes[0xE6] = &opcode_0xe6;
  Opcodes[0xE7] = &opcode_0xe7;
  Opcodes[0xEA] = &opcode_0xea;
  Opcodes[0xEF] = &opcode_0xef;

  Opcodes[0xF0] = &opcode_0xf0;
  Opcodes[0xF1] = &opcode_0xf1;
  Opcodes[0xF3] = &opcode_0xf3;
  Opcodes[0xF5] = &opcode_0xf5;
  Opcodes[0xF6] = &opcode_0xf6;
  Opcodes[0xF7] = &opcode_0xf7;
  Opcodes[0xFA] = &opcode_0xfa;
  Opcodes[0xFB] = &opcode_0xfb;
  Opcodes[0xFE] = &opcode_0xfe;
  Opcodes[0xFF] = &opcode_0xff;
}

void load_prefixcb(void)
{
  PrefixCB[0x11] = &prefix_0x11;

  PrefixCB[0x30] = &prefix_0x30;
  PrefixCB[0x31] = &prefix_0x31;
  PrefixCB[0x32] = &prefix_0x32;
  PrefixCB[0x33] = &prefix_0x33;
  PrefixCB[0x34] = &prefix_0x34;
  PrefixCB[0x35] = &prefix_0x35;
  PrefixCB[0x36] = &prefix_0x36;
  PrefixCB[0x37] = &prefix_0x37;

  PrefixCB[0x7C] = &bit7h;
}

void my_clock_handling(void)
{
  my_clock.total_m += my_clock.m;
  my_clock.total_t += my_clock.t;
  my_clock.lineticks += my_clock.m;
  switch (my_clock.mode)
  {
    case 0:
      if (my_clock.lineticks > 203)
      {
        if (read_memory(0xFF44) == 143)
        {
          my_clock.mode = 1;
        }
        else
        {
          my_clock.mode = 2;
        }
        my_clock.lineticks = 0;
        write_memory(0xFF44, read_memory(0xFF44) + 1);
      }
      break;
    case 1: // VBLANK
      if (my_clock.lineticks >= 456 && read_memory(0xFF44) == 153)
      {
        my_clock.mode = 2;
        my_clock.lineticks = 0;
        write_memory(0xFF44, 0);
        my_clock.total_m  = 0;
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
      }
      break;
    case 3:
      if (my_clock.lineticks > 172)
      {
        my_clock.mode = 0;
        my_clock.lineticks = 0;
      }
      break;
  }
}

void execute(uint16_t op)
{
  if (!Opcodes[op])
  {
    fprintf(stderr, "Unknown Op %x at address %x", op, r.PC.val - 1);
    exit(1);
  }
  Opcodes[op]();

  my_clock_handling();
}
