#ifndef VRAM_H
# define VRAM_H

#include "mmu.h"
#include "registers.h"
#include <SDL2/SDL.h>

extern Mmu MMU;
extern Registers r;
extern My_clock my_clock;
extern void (*Opcodes[0xFF]) (void);

void print_tiles(SDL_Renderer *renderer);
void print_vram(SDL_Renderer *renderer);

#endif
