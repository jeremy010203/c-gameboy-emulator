#ifndef HELPERS_H
# define HELPERS_H

#include "CUnit/Basic.h"
#include "utils.h"

int check_flags(int Z, int N, int H, int C);
void init_execute_byte(uint8_t op);
void test_8(uint8_t op, void (*init)(void), void (*condition)(void));
void test_inc_normal(uint8_t op, uint8_t *reg, void (*custom_init)(void));
void test_inc_overflow(uint8_t op, uint8_t *reg, void (*custom_init)(void));
void test_inc_half(uint8_t op, uint8_t *reg, void (*custom_init)(void));

#endif /* HELPERS_H */
