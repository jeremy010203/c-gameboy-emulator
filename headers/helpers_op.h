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
void adc_op(uint8_t *first, const uint8_t second);
void add_8_op(uint8_t *first, const uint8_t second);
void add_16_op(uint16_t *first, const uint16_t second);
void sub_8_op(uint8_t *first, const uint8_t second);
void xor_8_op(uint8_t *first, const uint8_t second);
void cp_op(const uint8_t first, const uint8_t second);
void and_op(uint8_t *first, const uint8_t second);
void or_op(uint8_t *first, const uint8_t second);
void ret_cond_op(int cond);
void load(uint8_t* to, const uint8_t from);
void bit_op(const uint8_t reg, const uint8_t pos);
void res_op(uint8_t *reg, const uint8_t pos);
void set_op(uint8_t *reg, const uint8_t pos);
void sla_op(uint8_t *reg);
void srl_op(uint8_t *reg);
void rl_op(uint8_t *reg);
void rr_op(uint8_t *reg);
void sbc_op(uint8_t *first, const uint8_t second);
void sra_op(uint8_t *reg);
void rlc_op(uint8_t *reg);
void rrc_op(uint8_t *reg);

#endif /* HELPERS_OP_H */
