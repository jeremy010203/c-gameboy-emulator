#ifndef HELPERS_OP_H
# define HELPERS_OP_H

#include "mmu.h"
#include "registers.h"

uint8_t read_byte(void);
uint16_t read_word(void);
uint8_t peak_byte(void);
void push_stack(const uint16_t val);
uint16_t pop_stack(void);

void inc_op(uint8_t *reg);
void dec_op(uint8_t *reg);
void pop_op(uint16_t *reg);
void push_op(const uint16_t reg);
void rst_op(const uint16_t addr);
void swap_op(uint8_t *reg);
void add_8_op(uint8_t *first, const uint8_t second);
void add_16_op(uint16_t *first, const uint16_t second);
void sub_8_op(uint8_t *first, const uint8_t second);
void xor_8_op(uint8_t *first, const uint8_t second);
void ret_cond_op(int cond);

#endif /* HELPERS_OP_H */
