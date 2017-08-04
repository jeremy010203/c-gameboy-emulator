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
    || (NULL == CU_add_test(pSuite, "test of 0x3C", test0x3C)))
  {
    CU_cleanup_registry();
    return CU_get_error();
  }

  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  CU_cleanup_registry();
  return CU_get_error();
}
