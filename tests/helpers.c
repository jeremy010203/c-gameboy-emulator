#include "helpers.h"

#define LAMBDA(c_) ({ c_ _;})

int check_flags(int Z, int N, int H, int C)
{
    return getZ() == Z && getN() == N && getH() == H && getC() == C;
}

void init_execute_byte(uint8_t op)
{
  init_registers();
  MMU.memory[0] = op;
  execute(read_byte());
}

void test_8(uint8_t op, void (*init)(void), void (*condition)(void))
{
  init_registers();
  MMU.memory[0] = op;
  init();
  execute(read_byte());

  condition();
}

void test_inc_normal(uint8_t op, uint8_t *reg, void (*custom_init)(void))
{
  test_8(op,
    LAMBDA(void _(void) {
      *reg = 0;
      custom_init();
    }),
    LAMBDA(void _(void) {
      CU_ASSERT(check_flags(0, 0, 0, 0));
      CU_ASSERT(*reg == 1);
      CU_ASSERT(r.SP.val == 0);
      CU_ASSERT(r.PC.val == 1);
    })
  );
}

void test_dec_normal(uint8_t op, uint8_t *reg, void (*custom_init)(void))
{
  test_8(op,
    LAMBDA(void _(void) {
      *reg = 2;
      custom_init();
    }),
    LAMBDA(void _(void) {
      CU_ASSERT(check_flags(0, 1, 0, 0));
      CU_ASSERT(*reg == 1);
      CU_ASSERT(r.SP.val == 0);
      CU_ASSERT(r.PC.val == 1);
    })
  );
}

void test_inc_overflow(uint8_t op, uint8_t *reg, void (*custom_init)(void))
{
  test_8(op,
    LAMBDA(void _(void) {
      *reg = 255;
      custom_init();
    }),
    LAMBDA(void _(void) {
      CU_ASSERT(check_flags(1, 0, 1, 0));
      CU_ASSERT(*reg == 0);
      CU_ASSERT(r.SP.val == 0);
      CU_ASSERT(r.PC.val == 1);
    })
  );
}

void test_inc_half(uint8_t op, uint8_t *reg, void (*custom_init)(void))
{
  test_8(op,
    LAMBDA(void _(void) {
      *reg = 0x0F;
      custom_init();
    }),
    LAMBDA(void _(void) {
      CU_ASSERT(check_flags(0, 0, 1, 0));
    })
  );

  test_8(op,
    LAMBDA(void _(void) {
      *reg = 0x3F;
      custom_init();
    }),
    LAMBDA(void _(void) {
      CU_ASSERT(check_flags(0, 0, 1, 0));
    })
  );

  test_8(op,
    LAMBDA(void _(void) {
      *reg = 0x3E;
      custom_init();
    }),
    LAMBDA(void _(void) {
      CU_ASSERT(check_flags(0, 0, 0, 0));
    })
  );
}
