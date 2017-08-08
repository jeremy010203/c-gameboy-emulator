#ifndef UTILS_H
# define UTILS_H

#include "mmu.h"
#include "registers.h"
#include "helpers_op.h"
#include "vram.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <sys/time.h>

void init(void);
void load_opcodes(void);
void load_prefixcb(void);
void execute(uint16_t op, SDL_Renderer *renderer, int *display);

void loadhlpa(void);
void loadhlma(void);
void opcode_0x01(void);
void opcode_0x11(void);
void opcode_0x21(void);
void opcode_0x31(void);
void loadspd16(void);
void prefixcb(void);
void jrnz(void);
void loadcd8(void);
void loaded8(void);
void loadld8(void);
void loadad8(void);
void opcode_0x0c(void);
void opcode_0xe2(void);

#endif
