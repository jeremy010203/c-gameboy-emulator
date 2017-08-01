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
  uint8_t b1 = read_byte();
  uint16_t b2 = read_byte();
  b2 <<= 8;
  return b2 | b1;
}

// Read a byte in memory without increment PC
uint8_t peak_byte(void)
{
  return MMU.memory[r.PC.val];
}

void push_stack(uint16_t val)
{
  MMU.memory[r.SP.val--] = (uint8_t)(val >> 8);
  MMU.memory[r.SP.val--] = (uint8_t)((val << 8) >> 8);
}

uint16_t pop_stack(void)
{
  uint8_t v1 = MMU.memory[++r.SP.val];
  uint16_t v2 = MMU.memory[++r.SP.val];
  return (v2 << 8) | v1;
}

void opcode_0xc1(void)
{
  r.BC.val = pop_stack();

  my_clock.m = 1;
  my_clock.t = 12;
}

void opcode_0xc5(void)
{
  push_stack(r.BC.val);

  my_clock.m = 1;
  my_clock.t = 16;
}

void opcode_0xd5(void)
{
  push_stack(r.DE.val);

  my_clock.m = 1;
  my_clock.t = 16;
}

void opcode_0xe5(void)
{
  push_stack(r.HL.val);

  my_clock.m = 1;
  my_clock.t = 16;
}

void opcode_0xf5(void)
{
  push_stack(r.AF.val);

  my_clock.m = 1;
  my_clock.t = 16;
}

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
  MMU.memory[r.HL.val] = r.AF.bytes.high;
  r.HL.val++;

  my_clock.m = 1;
  my_clock.t = 8;
}

void loadhlma(void)
{
  MMU.memory[r.HL.val] = r.AF.bytes.high;
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

void opcode_0xe2(void)
{
  uint16_t pos = 0xFF00 + r.BC.bytes.low;
  MMU.memory[pos] = r.AF.bytes.high;

  my_clock.m = 2;
  my_clock.t = 8;
}

// INC C
void opcode_0x0c(void)
{
  uint8_t c = r.BC.bytes.low;

  r.BC.bytes.low++;
  r.BC.bytes.low == 0 ? setZ() : resetZ();
  ((r.BC.bytes.low ^ 0x01 ^ c) & 0x10) ? setH() : resetH();
  resetN();

  my_clock.m = 1;
  my_clock.t = 4;
}

// INC E
void opcode_0x1c(void)
{
  uint8_t e = r.DE.bytes.low;

  r.DE.bytes.low++;
  r.DE.bytes.low == 0 ? setZ() : resetZ();
  ((r.DE.bytes.low ^ 0x01 ^ e) & 0x10) ? setH() : resetH();
  resetN();

  my_clock.m = 1;
  my_clock.t = 4;
}

// INC L
void opcode_0x2c(void)
{
  uint8_t l = r.HL.bytes.low;

  r.HL.bytes.low++;
  r.HL.bytes.low == 0 ? setZ() : resetZ();
  ((r.HL.bytes.low ^ 0x01 ^ l) & 0x10) ? setH() : resetH();
  resetN();

  my_clock.m = 1;
  my_clock.t = 4;
}

// INC A
void opcode_0x3c(void)
{
  uint8_t a = r.AF.bytes.high;

  r.AF.bytes.high++;
  r.AF.bytes.high == 0 ? setZ() : resetZ();
  ((r.AF.bytes.high ^ 0x01 ^ a) & 0x10) ? setH() : resetH();
  resetN();

  my_clock.m = 1;
  my_clock.t = 4;
}

// INC B
void opcode_0x04(void)
{
  uint8_t b = r.BC.bytes.high;

  r.BC.bytes.high++;
  r.BC.bytes.high == 0 ? setZ() : resetZ();
  ((r.BC.bytes.high ^ 0x01 ^ b) & 0x10) ? setH() : resetH();
  resetN();

  my_clock.m = 1;
  my_clock.t = 4;
}

// INC D
void opcode_0x14(void)
{
  uint8_t d = r.DE.bytes.high;

  r.DE.bytes.high++;
  r.DE.bytes.high == 0 ? setZ() : resetZ();
  ((r.DE.bytes.high ^ 0x01 ^ d) & 0x10) ? setH() : resetH();
  resetN();

  my_clock.m = 1;
  my_clock.t = 4;
}

// INC H
void opcode_0x24(void)
{
  uint8_t h= r.HL.bytes.high;

  r.HL.bytes.high++;
  r.HL.bytes.high == 0 ? setZ() : resetZ();
  ((r.HL.bytes.high ^ 0x01 ^ h) & 0x10) ? setH() : resetH();
  resetN();

  my_clock.m = 1;
  my_clock.t = 4;
}

// INC (HL)
void opcode_0x34(void)
{
  uint8_t hl = MMU.memory[r.HL.bytes.high];

  MMU.memory[r.HL.bytes.high]++;
  MMU.memory[r.HL.bytes.high] == 0 ? setZ() : resetZ();
  ((MMU.memory[r.HL.bytes.high] ^ 0x01 ^ hl) & 0x10) ? setH() : resetH();
  resetN();

  my_clock.m = 1;
  my_clock.t = 12;
}

void opcode_0x77(void)
{
  MMU.memory[r.HL.val] = r.AF.bytes.high;

  my_clock.m = 1;
  my_clock.t = 8;
}

void opcode_0x01(void)
{
  r.BC.val = read_word();

  my_clock.m = 3;
  my_clock.t = 12;
}

void opcode_0x11(void)
{
  r.DE.val = read_word();

  my_clock.m = 3;
  my_clock.t = 12;
}

void opcode_0x0a(void)
{
  r.AF.bytes.high = MMU.memory[r.BC.val];

  my_clock.m = 1;
  my_clock.t = 8;
}

void opcode_0x1a(void)
{
  r.AF.bytes.high = MMU.memory[r.DE.val];

  my_clock.m = 1;
  my_clock.t = 8;
}

void opcode_0x2a(void)
{
  r.AF.bytes.high = MMU.memory[r.HL.val];
  r.HL.val++;

  my_clock.m = 1;
  my_clock.t = 8;
}

void opcode_0x36(void)
{
  MMU.memory[r.HL.val] = read_byte();

  my_clock.m = 2;
  my_clock.t = 12;
}

void opcode_0x46(void)
{
  r.BC.bytes.high = MMU.memory[r.HL.val];

  my_clock.m = 1;
  my_clock.t = 8;
}

void opcode_0x56(void)
{
  r.DE.bytes.high = MMU.memory[r.HL.val];

  my_clock.m = 1;
  my_clock.t = 8;
}

void opcode_0x66(void)
{
  r.HL.bytes.high = MMU.memory[r.HL.val];

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

void opcode_0x3a(void)
{
  r.AF.bytes.high = MMU.memory[r.HL.val];
  r.HL.val--;

  my_clock.m = 1;
  my_clock.t = 8;
}

void opcode_0x21(void)
{
  r.HL.val = read_word();

  my_clock.m = 3;
  my_clock.t = 12;
}

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

// ADD A,B
void opcode_0x80(void)
{
  uint16_t a = r.AF.bytes.high;
  uint16_t b = r.BC.bytes.high;
  (a + b > 255) ? setC() : resetC();

  r.AF.bytes.high += r.BC.bytes.high;
  r.AF.bytes.high == 0 ? setZ() : resetZ();
  ((r.AF.bytes.high ^ r.BC.bytes.high ^ (uint8_t)a) & 0x10) ? setH() : resetH();
  resetN();

  my_clock.m = 1;
  my_clock.t = 4;
}

// ADD A,C
void opcode_0x81(void)
{
  uint16_t a = r.AF.bytes.high;
  uint16_t c = r.BC.bytes.low;
  (a + c > 255) ? setC() : resetC();

  r.AF.bytes.high += r.BC.bytes.low;
  r.AF.bytes.high == 0 ? setZ() : resetZ();
  ((r.AF.bytes.high ^ r.BC.bytes.low ^ (uint8_t)a) & 0x10) ? setH() : resetH();
  resetN();

  my_clock.m = 1;
  my_clock.t = 4;
}

// ADD A,D
void opcode_0x82(void)
{
  uint16_t a = r.AF.bytes.high;
  uint16_t d = r.DE.bytes.high;
  (a + d > 255) ? setC() : resetC();

  r.AF.bytes.high += r.DE.bytes.high;
  r.AF.bytes.high == 0 ? setZ() : resetZ();
  ((r.AF.bytes.high ^ r.DE.bytes.high ^ (uint8_t)a) & 0x10) ? setH() : resetH();
  resetN();

  my_clock.m = 1;
  my_clock.t = 4;
}

// ADD A,E
void opcode_0x83(void)
{
  uint16_t a = r.AF.bytes.high;
  uint16_t e = r.DE.bytes.low;
  (a + e > 255) ? setC() : resetC();

  r.AF.bytes.high += r.DE.bytes.low;
  r.AF.bytes.high == 0 ? setZ() : resetZ();
  ((r.AF.bytes.high ^ r.DE.bytes.low ^ (uint8_t)a) & 0x10) ? setH() : resetH();
  resetN();

  my_clock.m = 1;
  my_clock.t = 4;
}

// ADD A,H
void opcode_0x84(void)
{
  uint16_t a = r.AF.bytes.high;
  uint16_t h = r.HL.bytes.high;
  (a + h > 255) ? setC() : resetC();

  r.AF.bytes.high += r.HL.bytes.high;
  r.AF.bytes.high == 0 ? setZ() : resetZ();
  ((r.AF.bytes.high ^ r.HL.bytes.high ^ (uint8_t)a) & 0x10) ? setH() : resetH();
  resetN();

  my_clock.m = 1;
  my_clock.t = 4;
}

// ADD A,L
void opcode_0x85(void)
{
  uint16_t a = r.AF.bytes.high;
  uint16_t l = r.HL.bytes.low;
  (a + l > 255) ? setC() : resetC();

  r.AF.bytes.high += r.HL.bytes.low;
  r.AF.bytes.high == 0 ? setZ() : resetZ();
  ((r.AF.bytes.high ^ r.HL.bytes.low ^ (uint8_t)a) & 0x10) ? setH() : resetH();
  resetN();

  my_clock.m = 1;
  my_clock.t = 4;
}

// ADD A,(HL)
void opcode_0x86(void)
{
  uint16_t a = r.AF.bytes.high;
  uint16_t hl = MMU.memory[r.HL.val];
  (a + hl > 255) ? setC() : resetC();

  r.AF.bytes.high += MMU.memory[r.HL.val];
  r.AF.bytes.high == 0 ? setZ() : resetZ();
  ((r.AF.bytes.high ^ MMU.memory[r.HL.val] ^ (uint8_t)a) & 0x10) ? setH() : resetH();
  resetN();

  my_clock.m = 1;
  my_clock.t = 8;
}

// ADD A,A
void opcode_0x87(void)
{
  uint16_t a = r.AF.bytes.high;
  (a + a > 255) ? setC() : resetC();

  r.AF.bytes.high += r.AF.bytes.high;
  r.AF.bytes.high == 0 ? setZ() : resetZ();
  ((r.AF.bytes.high ^ r.AF.bytes.high ^ (uint8_t)a) & 0x10) ? setH() : resetH();
  resetN();

  my_clock.m = 1;
  my_clock.t = 4;
}

// SUB B
void opcode_0x90(void)
{
  uint16_t a = r.AF.bytes.high;
  uint16_t b = r.BC.bytes.high;
  (a < b) ? setC() : resetC();

  r.AF.bytes.high -= r.BC.bytes.high;
  r.AF.bytes.high == 0 ? setZ() : resetZ();
  ((r.AF.bytes.high ^ r.BC.bytes.high ^ (uint8_t)a) & 0x10) ? setH() : resetH();
  setN();

  my_clock.m = 1;
  my_clock.t = 4;
}

// SUB C
void opcode_0x91(void)
{
  uint16_t a = r.AF.bytes.high;
  uint16_t c = r.BC.bytes.low;
  (a < c) ? setC() : resetC();

  r.AF.bytes.high -= r.BC.bytes.low;
  r.AF.bytes.high == 0 ? setZ() : resetZ();
  ((r.AF.bytes.high ^ r.BC.bytes.low ^ (uint8_t)a) & 0x10) ? setH() : resetH();
  setN();

  my_clock.m = 1;
  my_clock.t = 4;
}

// SUB D
void opcode_0x92(void)
{
  uint16_t a = r.AF.bytes.high;
  uint16_t d = r.DE.bytes.high;
  (a < d) ? setC() : resetC();

  r.AF.bytes.high -= r.DE.bytes.high;
  r.AF.bytes.high == 0 ? setZ() : resetZ();
  ((r.AF.bytes.high ^ r.DE.bytes.high ^ (uint8_t)a) & 0x10) ? setH() : resetH();
  setN();

  my_clock.m = 1;
  my_clock.t = 4;
}

// SUB E
void opcode_0x93(void)
{
  uint16_t a = r.AF.bytes.high;
  uint16_t e = r.DE.bytes.low;
  (a < e) ? setC() : resetC();

  r.AF.bytes.high -= r.DE.bytes.low;
  r.AF.bytes.high == 0 ? setZ() : resetZ();
  ((r.AF.bytes.high ^ r.DE.bytes.low ^ (uint8_t)a) & 0x10) ? setH() : resetH();
  setN();

  my_clock.m = 1;
  my_clock.t = 4;
}

// SUB H
void opcode_0x94(void)
{
  uint16_t a = r.AF.bytes.high;
  uint16_t h = r.HL.bytes.high;
  (a < h) ? setC() : resetC();

  r.AF.bytes.high -= r.HL.bytes.high;
  r.AF.bytes.high == 0 ? setZ() : resetZ();
  ((r.AF.bytes.high ^ r.HL.bytes.high ^ (uint8_t)a) & 0x10) ? setH() : resetH();
  setN();

  my_clock.m = 1;
  my_clock.t = 4;
}

// SUB L
void opcode_0x95(void)
{
  uint16_t a = r.AF.bytes.high;
  uint16_t l = r.HL.bytes.low;
  (a < l) ? setC() : resetC();

  r.AF.bytes.high -= r.HL.bytes.low;
  r.AF.bytes.high == 0 ? setZ() : resetZ();
  ((r.AF.bytes.high ^ r.HL.bytes.low ^ (uint8_t)a) & 0x10) ? setH() : resetH();
  setN();

  my_clock.m = 1;
  my_clock.t = 4;
}

// SUB (HL)
void opcode_0x96(void)
{
  uint16_t a = r.AF.bytes.high;
  uint16_t hl = MMU.memory[r.HL.val];
  (a < hl) ? setC() : resetC();

  r.AF.bytes.high -= MMU.memory[r.HL.val];
  r.AF.bytes.high == 0 ? setZ() : resetZ();
  ((r.AF.bytes.high ^ MMU.memory[r.HL.val] ^ (uint8_t)a) & 0x10) ? setH() : resetH();
  setN();

  my_clock.m = 1;
  my_clock.t = 8;
}

// SUB A
void opcode_0x97(void)
{
  uint16_t a = r.AF.bytes.high;
  resetC();

  r.AF.bytes.high -= r.AF.bytes.high;
  r.AF.bytes.high == 0 ? setZ() : resetZ();
  ((r.AF.bytes.high ^ r.AF.bytes.high ^ (uint8_t)a) & 0x10) ? setH() : resetH();
  setN();

  my_clock.m = 1;
  my_clock.t = 4;
}

// OR B
void opcode_0xb0(void)
{
  r.AF.bytes.high |= r.BC.bytes.high;
  r.AF.bytes.high == 0 ? setZ() : resetZ();

  my_clock.m = 1;
  my_clock.t = 4;
}

// OR C
void opcode_0xb1(void)
{
  r.AF.bytes.high |= r.BC.bytes.low;
  r.AF.bytes.high == 0 ? setZ() : resetZ();

  my_clock.m = 1;
  my_clock.t = 4;
}

// OR D
void opcode_0xb2(void)
{
  r.AF.bytes.high |= r.DE.bytes.high;
  r.AF.bytes.high == 0 ? setZ() : resetZ();

  my_clock.m = 1;
  my_clock.t = 4;
}

// OR E
void opcode_0xb3(void)
{
  r.AF.bytes.high |= r.DE.bytes.low;
  r.AF.bytes.high == 0 ? setZ() : resetZ();

  my_clock.m = 1;
  my_clock.t = 4;
}

// OR H
void opcode_0xb4(void)
{
  r.AF.bytes.high |= r.HL.bytes.high;
  r.AF.bytes.high == 0 ? setZ() : resetZ();

  my_clock.m = 1;
  my_clock.t = 4;
}

// OR L
void opcode_0xb5(void)
{
  r.AF.bytes.high |= r.HL.bytes.low;
  r.AF.bytes.high == 0 ? setZ() : resetZ();

  my_clock.m = 1;
  my_clock.t = 4;
}

// OR (HL)
void opcode_0xb6(void)
{
  r.AF.bytes.high |= MMU.memory[r.HL.val];
  r.AF.bytes.high == 0 ? setZ() : resetZ();

  my_clock.m = 1;
  my_clock.t = 8;
}

// OR A
void opcode_0xb7(void)
{
  r.AF.bytes.high |= r.AF.bytes.high;
  r.AF.bytes.high == 0 ? setZ() : resetZ();

  my_clock.m = 1;
  my_clock.t = 4;
}

// CP d8
void opcode_0xfe(void)
{
  uint8_t val = read_byte();

  (val > r.AF.bytes.high) ? setC() : resetC();
  ((val & 0x0f) > (r.AF.bytes.high & 0x0f)) ? setH() : resetH();
  r.AF.bytes.high == val ? setZ() : resetZ();
  setN();

  my_clock.m = 2;
  my_clock.t = 8;
}

// CP B
void opcode_0xb8(void)
{
  uint8_t val = r.BC.bytes.high;

  (val > r.AF.bytes.high) ? setC() : resetC();
  ((val & 0x0f) > (r.AF.bytes.high & 0x0f)) ? setH() : resetH();
  r.AF.bytes.high == val ? setZ() : resetZ();
  setN();

  my_clock.m = 1;
  my_clock.t = 8;
}

// CP C
void opcode_0xb9(void)
{
  uint8_t val = r.BC.bytes.low;

  (val > r.AF.bytes.high) ? setC() : resetC();
  ((val & 0x0f) > (r.AF.bytes.high & 0x0f)) ? setH() : resetH();
  r.AF.bytes.high == val ? setZ() : resetZ();
  setN();

  my_clock.m = 1;
  my_clock.t = 8;
}

// CP D
void opcode_0xba(void)
{
  uint8_t val = r.DE.bytes.high;

  (val > r.AF.bytes.high) ? setC() : resetC();
  ((val & 0x0f) > (r.AF.bytes.high & 0x0f)) ? setH() : resetH();
  r.AF.bytes.high == val ? setZ() : resetZ();
  setN();

  my_clock.m = 1;
  my_clock.t = 8;
}

// CP E
void opcode_0xbb(void)
{
  uint8_t val = r.DE.bytes.low;

  (val > r.AF.bytes.high) ? setC() : resetC();
  ((val & 0x0f) > (r.AF.bytes.high & 0x0f)) ? setH() : resetH();
  r.AF.bytes.high == val ? setZ() : resetZ();
  setN();

  my_clock.m = 1;
  my_clock.t = 8;
}

// CP H
void opcode_0xbc(void)
{
  uint8_t val = r.HL.bytes.high;

  (val > r.AF.bytes.high) ? setC() : resetC();
  ((val & 0x0f) > (r.AF.bytes.high & 0x0f)) ? setH() : resetH();
  r.AF.bytes.high == val ? setZ() : resetZ();
  setN();

  my_clock.m = 1;
  my_clock.t = 8;
}

// CP L
void opcode_0xbd(void)
{
  uint8_t val = r.HL.bytes.low;

  (val > r.AF.bytes.high) ? setC() : resetC();
  ((val & 0x0f) > (r.AF.bytes.high & 0x0f)) ? setH() : resetH();
  r.AF.bytes.high == val ? setZ() : resetZ();
  setN();

  my_clock.m = 1;
  my_clock.t = 8;
}

// CP (HL)
void opcode_0xbe(void)
{
  uint8_t val = MMU.memory[r.HL.val];

  (val > r.AF.bytes.high) ? setC() : resetC();
  ((val & 0x0f) > (r.AF.bytes.high & 0x0f)) ? setH() : resetH();
  r.AF.bytes.high == val ? setZ() : resetZ();
  setN();

  my_clock.m = 1;
  my_clock.t = 8;
}

// CP A
void opcode_0xbf(void)
{
  uint8_t val = r.AF.bytes.high;

  (val > r.AF.bytes.high) ? setC() : resetC();
  ((val & 0x0f) > (r.AF.bytes.high & 0x0f)) ? setH() : resetH();
  r.AF.bytes.high == val ? setZ() : resetZ();
  setN();

  my_clock.m = 1;
  my_clock.t = 8;
}

// (a16) <- A
void opcode_0xea(void)
{
  uint16_t val = read_word();
  MMU.memory[val] = r.AF.bytes.high;

  my_clock.m = 3;
  my_clock.t = 16;
}

// A <- (a16)
void opcode_0xfa(void)
{
  uint16_t val = read_word();
  r.AF.bytes.high = MMU.memory[val];

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

void opcode_0x0d(void)
{
  if (r.BC.bytes.low == 0)
    setH();

  r.BC.bytes.low--;
  r.BC.bytes.low == 0 ? setZ() : resetZ();
  setN();

  my_clock.m = 1;
  my_clock.t = 4;
}

void opcode_0x1d(void)
{
  if (r.DE.bytes.low == 0)
    setH();

  r.DE.bytes.low--;
  r.DE.bytes.low == 0 ? setZ() : resetZ();
  setN();

  my_clock.m = 1;
  my_clock.t = 4;
}

void opcode_0x2d(void)
{
  if (r.HL.bytes.low == 0)
    setH();

  r.HL.bytes.low--;
  r.HL.bytes.low == 0 ? setZ() : resetZ();
  setN();

  my_clock.m = 1;
  my_clock.t = 4;
}

void opcode_0x3d(void)
{
  if (r.AF.bytes.high == 0)
    setH();

  r.AF.bytes.high--;
  r.AF.bytes.high == 0 ? setZ() : resetZ();
  setN();

  my_clock.m = 1;
  my_clock.t = 4;
}

// DEC B
void opcode_0x05(void)
{
  if (r.BC.bytes.high == 0)
    setH();

  r.BC.bytes.high--;
  r.BC.bytes.high == 0 ? setZ() : resetZ();
  setN();

  my_clock.m = 1;
  my_clock.t = 4;
}

void opcode_0x15(void)
{
  if (r.DE.bytes.high == 0)
    setH();

  r.DE.bytes.high--;
  r.DE.bytes.high == 0 ? setZ() : resetZ();
  setN();

  my_clock.m = 1;
  my_clock.t = 4;
}

void opcode_0x25(void)
{
  if (r.HL.bytes.high == 0)
    setH();

  r.HL.bytes.high--;
  r.HL.bytes.high == 0 ? setZ() : resetZ();
  setN();

  my_clock.m = 1;
  my_clock.t = 4;
}

void opcode_0x35(void)
{
  uint8_t hl = MMU.memory[r.HL.val];
  if (hl == 0)
    setH();

  MMU.memory[r.HL.val]--;
  MMU.memory[r.HL.val] == 0 ? setZ() : resetZ();
  setN();

  my_clock.m = 1;
  my_clock.t = 4;
}

void opcode_0x03(void)
{
  r.BC.val++;

  my_clock.m = 1;
  my_clock.t = 8;
}

void opcode_0x13(void)
{
  r.DE.val++;

  my_clock.m = 1;
  my_clock.t = 8;
}

void opcode_0x23(void)
{
  r.HL.val++;

  my_clock.m = 1;
  my_clock.t = 8;
}

void opcode_0x33(void)
{
  r.SP.val++;

  my_clock.m = 1;
  my_clock.t = 8;
}

// RET
void opcode_0xc9(void)
{
  r.PC.val = pop_stack();

  my_clock.m = 1;
  my_clock.t = 16;
}

// LDH (a8), A
void opcode_0xe0(void)
{
  uint16_t addr = 0xFF00 + read_byte();
  MMU.memory[addr] = r.AF.bytes.high;

  my_clock.m = 2;
  my_clock.t = 12;
}

// LDH A, (a8)
void opcode_0xf0(void)
{
  uint16_t addr = 0xFF00 + read_byte();
  r.AF.bytes.high = MMU.memory[addr];

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

void load_opcodes(void)
{
  Opcodes[0x00] = &nop;
  Opcodes[0x01] = &opcode_0x01;
  Opcodes[0x03] = &opcode_0x03;
  Opcodes[0x04] = &opcode_0x04;
  Opcodes[0x05] = &opcode_0x05;
  Opcodes[0x06] = &opcode_0x06;
  Opcodes[0x0a] = &opcode_0x0a;
  Opcodes[0x0b] = &opcode_0x0b;
  Opcodes[0x0c] = &opcode_0x0c;
  Opcodes[0x0d] = &opcode_0x0d;
  Opcodes[0x0e] = &loadcd8;

  Opcodes[0x11] = &opcode_0x11;
  Opcodes[0x13] = &opcode_0x13;
  Opcodes[0x14] = &opcode_0x14;
  Opcodes[0x15] = &opcode_0x15;
  Opcodes[0x16] = &opcode_0x16;
  Opcodes[0x17] = &opcode_0x17;
  Opcodes[0x18] = &opcode_0x18;
  Opcodes[0x1a] = &opcode_0x1a;
  Opcodes[0x1b] = &opcode_0x1b;
  Opcodes[0x1c] = &opcode_0x1c;
  Opcodes[0x1d] = &opcode_0x1d;
  Opcodes[0x1e] = &loaded8;

  Opcodes[0x20] = &opcode_0x20;
  Opcodes[0x21] = &opcode_0x21;
  Opcodes[0x22] = &loadhlpa;
  Opcodes[0x23] = &opcode_0x23;
  Opcodes[0x24] = &opcode_0x24;
  Opcodes[0x25] = &opcode_0x25;
  Opcodes[0x26] = &opcode_0x26;
  Opcodes[0x28] = &opcode_0x28;
  Opcodes[0x2a] = &opcode_0x2a;
  Opcodes[0x2b] = &opcode_0x2b;
  Opcodes[0x2c] = &opcode_0x2c;
  Opcodes[0x2d] = &opcode_0x2d;
  Opcodes[0x2e] = &loadld8;

  Opcodes[0x30] = &opcode_0x30;
  Opcodes[0x31] = &opcode_0x31;
  Opcodes[0x32] = &loadhlma;
  Opcodes[0x33] = &opcode_0x33;
  Opcodes[0x34] = &opcode_0x34;
  Opcodes[0x35] = &opcode_0x35;
  Opcodes[0x36] = &opcode_0x36;
  Opcodes[0x38] = &opcode_0x38;
  Opcodes[0x3a] = &opcode_0x3a;
  Opcodes[0x3b] = &opcode_0x3b;
  Opcodes[0x3c] = &opcode_0x3c;
  Opcodes[0x3d] = &opcode_0x3d;
  Opcodes[0x3e] = &loadad8;

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

  Opcodes[0xAF] = &xora;

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

  Opcodes[0xC1] = &opcode_0xc1;
  Opcodes[0xC3] = &opcode_0xc3;
  Opcodes[0xC5] = &opcode_0xc5;
  Opcodes[0xC9] = &opcode_0xc9;
  Opcodes[0xCB] = &prefixcb;
  Opcodes[0xCC] = &opcode_0xcc;
  Opcodes[0xCD] = &opcode_0xcd;

  Opcodes[0xD5] = &opcode_0xd5;

  Opcodes[0xE0] = &opcode_0xe0;
  Opcodes[0xE2] = &opcode_0xe2;
  Opcodes[0xE5] = &opcode_0xe5;
  Opcodes[0xEA] = &opcode_0xea;

  Opcodes[0xF0] = &opcode_0xf0;
  Opcodes[0xF3] = &opcode_0xf3;
  Opcodes[0xF5] = &opcode_0xf5;
  Opcodes[0xFA] = &opcode_0xfa;
  Opcodes[0xFB] = &opcode_0xfb;
  Opcodes[0xFE] = &opcode_0xfe;
}

void load_prefixcb(void)
{
  PrefixCB[0x11] = &prefix_0x11;
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
        if (MMU.memory[0xFF44] == 143)
        {
          my_clock.mode = 1;
        }
        else
        {
          my_clock.mode = 2;
        }
        my_clock.lineticks = 0;
        MMU.memory[0xFF44]++;
      }
      break;
    case 1: // VBLANK
      if (my_clock.lineticks >= 456 && MMU.memory[0xFF44] == 153)
      {
        my_clock.mode = 2;
        my_clock.lineticks = 0;
        MMU.memory[0xFF44] = 0;
        my_clock.total_m  = 0;
      }
      else if (my_clock.lineticks >= 456)
      {
        my_clock.lineticks = 0;
        MMU.memory[0xFF44]++;
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
