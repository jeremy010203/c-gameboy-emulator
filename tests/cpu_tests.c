#include "CUnit/Basic.h"
#include "utils.h"
#include "helpers.h"

#define LAMBDA(c_) ({ c_ _;})

extern Mmu MMU;
extern Registers r;
extern My_clock my_clock;
extern void (*Opcodes[0xFF]) (void);

int init_cpu_suite(void)
{
  init();
  return 0;
}

int clean_cpu_suite(void)
{
  return 0;
}

void test0x00(void)
{
  test_8(0, LAMBDA(void _(void) {}), LAMBDA(void _(void) {
    CU_ASSERT(r.AF.val == 0);
    CU_ASSERT(r.BC.val == 0);
    CU_ASSERT(r.DE.val == 0);
    CU_ASSERT(r.HL.val == 0);
    CU_ASSERT(r.SP.val == 0);
    CU_ASSERT(r.PC.val == 1);
    CU_ASSERT(my_clock.t == 4);
    CU_ASSERT(my_clock.m == 1);
  }));
}

void test0x04(void)
{
  test_inc_normal(0x04, &r.BC.bytes.high, LAMBDA(void _(void) {}));
  test_inc_overflow(0x04, &r.BC.bytes.high, LAMBDA(void _(void) {}));
  test_inc_half(0x04, &r.BC.bytes.high, LAMBDA(void _(void) {}));
}

void test0x0C(void)
{
  test_inc_normal(0x0C, &r.BC.bytes.low, LAMBDA(void _(void) {}));
  test_inc_overflow(0x0C, &r.BC.bytes.low, LAMBDA(void _(void) {}));
  test_inc_half(0x0C, &r.BC.bytes.low, LAMBDA(void _(void) {}));
}

void test0x14(void)
{
  test_inc_normal(0x14, &r.DE.bytes.high, LAMBDA(void _(void) {}));
  test_inc_overflow(0x14, &r.DE.bytes.high, LAMBDA(void _(void) {}));
  test_inc_half(0x14, &r.DE.bytes.high, LAMBDA(void _(void) {}));
}

void test0x1C(void)
{
  test_inc_normal(0x1C, &r.DE.bytes.low, LAMBDA(void _(void) {}));
  test_inc_overflow(0x1C, &r.DE.bytes.low, LAMBDA(void _(void) {}));
  test_inc_half(0x1C, &r.DE.bytes.low, LAMBDA(void _(void) {}));
}

void test0x24(void)
{
  test_inc_normal(0x24, &r.HL.bytes.high, LAMBDA(void _(void) {}));
  test_inc_overflow(0x24, &r.HL.bytes.high, LAMBDA(void _(void) {}));
  test_inc_half(0x24, &r.HL.bytes.high, LAMBDA(void _(void) {}));
}

void test0x2C(void)
{
  test_inc_normal(0x2C, &r.HL.bytes.low, LAMBDA(void _(void) {}));
  test_inc_overflow(0x2C, &r.HL.bytes.low, LAMBDA(void _(void) {}));
  test_inc_half(0x2C, &r.HL.bytes.low, LAMBDA(void _(void) {}));
}

void test0x34(void)
{
  test_inc_normal(0x34, &MMU.memory[10], LAMBDA(void _(void) { r.HL.val = 10; }));
  test_inc_overflow(0x34, &MMU.memory[10], LAMBDA(void _(void) { r.HL.val = 10; }));
  test_inc_half(0x34, &MMU.memory[10], LAMBDA(void _(void) { r.HL.val = 10; }));
}

void test0x3C(void)
{
  test_inc_normal(0x3C, &r.AF.bytes.high, LAMBDA(void _(void) {}));
  test_inc_overflow(0x3C, &r.AF.bytes.high, LAMBDA(void _(void) {}));
  test_inc_half(0x3C, &r.AF.bytes.high, LAMBDA(void _(void) {}));
}

// ADD A,B
void test0x80(void)
{
  // normal test
  test_8(0x80,
    LAMBDA(void _(void) {
      r.AF.val = 0;
      r.BC.val = 0xFF00;
    }),
    LAMBDA(void _(void) {
      CU_ASSERT(r.AF.val == 0xFF00);
      CU_ASSERT(r.BC.val == 0xFF00);
      CU_ASSERT(r.DE.val == 0);
      CU_ASSERT(r.HL.val == 0);
      CU_ASSERT(r.SP.val == 0);
      CU_ASSERT(r.PC.val == 1);
      CU_ASSERT(my_clock.t == 4);
      CU_ASSERT(my_clock.m == 1);
      CU_ASSERT(check_flags(0, 0, 0, 0));
    })
  );

  // half carry test
  test_8(0x80,
    LAMBDA(void _(void) {
      r.AF.val = 0x0100;
      r.BC.val = 0x0F00;
    }),
    LAMBDA(void _(void) {
      CU_ASSERT(r.AF.bytes.high == 0x10);
      CU_ASSERT(r.BC.val == 0x0F00);
      CU_ASSERT(r.DE.val == 0);
      CU_ASSERT(r.HL.val == 0);
      CU_ASSERT(r.SP.val == 0);
      CU_ASSERT(r.PC.val == 1);
      CU_ASSERT(my_clock.t == 4);
      CU_ASSERT(my_clock.m == 1);
      CU_ASSERT(check_flags(0, 0, 1, 0));
    })
  );

  // carry test
  test_8(0x80,
    LAMBDA(void _(void) {
      r.AF.val = 0x0100;
      r.BC.val = 0xFF00;
    }),
    LAMBDA(void _(void) {
      CU_ASSERT(r.AF.bytes.high == 0);
      CU_ASSERT(r.BC.val == 0xFF00);
      CU_ASSERT(r.DE.val == 0);
      CU_ASSERT(r.HL.val == 0);
      CU_ASSERT(r.SP.val == 0);
      CU_ASSERT(r.PC.val == 1);
      CU_ASSERT(my_clock.t == 4);
      CU_ASSERT(my_clock.m == 1);
      CU_ASSERT(check_flags(1, 0, 1, 1));
    })
  );
}

void test_bit_reg(uint8_t *reg, int pos, uint8_t code)
{
  test_8(0xcb,
    LAMBDA(void _(void) {
      *reg = (1 << pos);
      MMU.memory[r.PC.val + 1] = code;
    }),
    LAMBDA(void _(void) {
      CU_ASSERT(check_flags(0, 0, 1, 0));
      CU_ASSERT(r.PC.val == 2);
    })
  );

  test_8(0xcb,
    LAMBDA(void _(void) {
      *reg = (1 << pos);
      *reg = ~(*reg);
      MMU.memory[r.PC.val + 1] = code;
    }),
    LAMBDA(void _(void) {
      CU_ASSERT(check_flags(1, 0, 1, 0));
      CU_ASSERT(r.PC.val == 2);
    })
  );
}

void test_bit_mem(int pos, uint8_t code)
{
  test_8(0xcb,
    LAMBDA(void _(void) {
      r.HL.val = 0xAAAA;
      MMU.memory[r.HL.val] = (1 << pos);
      MMU.memory[r.PC.val + 1] = code;
    }),
    LAMBDA(void _(void) {
      CU_ASSERT(check_flags(0, 0, 1, 0));
      CU_ASSERT(r.PC.val == 2);
    })
  );

  test_8(0xcb,
    LAMBDA(void _(void) {
      r.HL.val = 0xAAAA;
      MMU.memory[r.HL.val] = (1 << pos);
      MMU.memory[r.HL.val] = ~MMU.memory[r.HL.val];
      MMU.memory[r.PC.val + 1] = code;
    }),
    LAMBDA(void _(void) {
      CU_ASSERT(check_flags(1, 0, 1, 0));
      CU_ASSERT(r.PC.val == 2);
    })
  );
}

// CB BITS
void test0xcbBITS(void)
{
  test_bit_reg(&r.BC.bytes.high, 0, 0x40);
  test_bit_reg(&r.BC.bytes.high, 1, 0x48);
  test_bit_reg(&r.BC.bytes.high, 2, 0x50);
  test_bit_reg(&r.BC.bytes.high, 3, 0x58);
  test_bit_reg(&r.BC.bytes.high, 4, 0x60);
  test_bit_reg(&r.BC.bytes.high, 5, 0x68);
  test_bit_reg(&r.BC.bytes.high, 6, 0x70);
  test_bit_reg(&r.BC.bytes.high, 7, 0x78);

  test_bit_reg(&r.BC.bytes.low, 0, 0x41);
  test_bit_reg(&r.BC.bytes.low, 1, 0x49);
  test_bit_reg(&r.BC.bytes.low, 2, 0x51);
  test_bit_reg(&r.BC.bytes.low, 3, 0x59);
  test_bit_reg(&r.BC.bytes.low, 4, 0x61);
  test_bit_reg(&r.BC.bytes.low, 5, 0x69);
  test_bit_reg(&r.BC.bytes.low, 6, 0x71);
  test_bit_reg(&r.BC.bytes.low, 7, 0x79);

  test_bit_reg(&r.DE.bytes.high, 0, 0x42);
  test_bit_reg(&r.DE.bytes.high, 1, 0x4a);
  test_bit_reg(&r.DE.bytes.high, 2, 0x52);
  test_bit_reg(&r.DE.bytes.high, 3, 0x5a);
  test_bit_reg(&r.DE.bytes.high, 4, 0x62);
  test_bit_reg(&r.DE.bytes.high, 5, 0x6a);
  test_bit_reg(&r.DE.bytes.high, 6, 0x72);
  test_bit_reg(&r.DE.bytes.high, 7, 0x7a);

  test_bit_reg(&r.DE.bytes.low, 0, 0x43);
  test_bit_reg(&r.DE.bytes.low, 1, 0x4b);
  test_bit_reg(&r.DE.bytes.low, 2, 0x53);
  test_bit_reg(&r.DE.bytes.low, 3, 0x5b);
  test_bit_reg(&r.DE.bytes.low, 4, 0x63);
  test_bit_reg(&r.DE.bytes.low, 5, 0x6b);
  test_bit_reg(&r.DE.bytes.low, 6, 0x73);
  test_bit_reg(&r.DE.bytes.low, 7, 0x7b);

  test_bit_reg(&r.HL.bytes.high, 0, 0x44);
  test_bit_reg(&r.HL.bytes.high, 1, 0x4c);
  test_bit_reg(&r.HL.bytes.high, 2, 0x54);
  test_bit_reg(&r.HL.bytes.high, 3, 0x5c);
  test_bit_reg(&r.HL.bytes.high, 4, 0x64);
  test_bit_reg(&r.HL.bytes.high, 5, 0x6c);
  test_bit_reg(&r.HL.bytes.high, 6, 0x74);
  test_bit_reg(&r.HL.bytes.high, 7, 0x7c);

  test_bit_reg(&r.HL.bytes.low, 0, 0x45);
  test_bit_reg(&r.HL.bytes.low, 1, 0x4d);
  test_bit_reg(&r.HL.bytes.low, 2, 0x55);
  test_bit_reg(&r.HL.bytes.low, 3, 0x5d);
  test_bit_reg(&r.HL.bytes.low, 4, 0x65);
  test_bit_reg(&r.HL.bytes.low, 5, 0x6d);
  test_bit_reg(&r.HL.bytes.low, 6, 0x75);
  test_bit_reg(&r.HL.bytes.low, 7, 0x7d);

  test_bit_mem(0, 0x46);
  test_bit_mem(1, 0x4e);
  test_bit_mem(2, 0x56);
  test_bit_mem(3, 0x5e);
  test_bit_mem(4, 0x66);
  test_bit_mem(5, 0x6e);
  test_bit_mem(6, 0x76);
  test_bit_mem(7, 0x7e);
}

int main(void)
{
  CU_pSuite pSuite = NULL;
  if (CUE_SUCCESS != CU_initialize_registry())
    return CU_get_error();
  pSuite = CU_add_suite("cpu_suite", init_cpu_suite, clean_cpu_suite);
  if (NULL == pSuite) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  if ((NULL == CU_add_test(pSuite, "test of 0x00", test0x00))
    || (NULL == CU_add_test(pSuite, "test of 0x04", test0x04))
    || (NULL == CU_add_test(pSuite, "test of 0x0C", test0x0C))
    || (NULL == CU_add_test(pSuite, "test of 0x14", test0x14))
    || (NULL == CU_add_test(pSuite, "test of 0x1C", test0x1C))
    || (NULL == CU_add_test(pSuite, "test of 0x24", test0x24))
    || (NULL == CU_add_test(pSuite, "test of 0x2C", test0x2C))
    || (NULL == CU_add_test(pSuite, "test of 0x34", test0x34))
    || (NULL == CU_add_test(pSuite, "test of 0x3C", test0x3C))
    || (NULL == CU_add_test(pSuite, "test of ADD A, 8bits registers 0x80", test0x80))
    || (NULL == CU_add_test(pSuite, "test of CB BITS", test0xcbBITS))
  )
  {
    CU_cleanup_registry();
    return CU_get_error();
  }

  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  CU_cleanup_registry();
  return CU_get_error();
}
