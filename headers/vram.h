#ifndef VRAM_H
# define VRAM_H

#include "mmu.h"
#include "registers.h"
#include <SDL2/SDL.h>

extern Mmu MMU;
extern Registers r;
extern Clock clock;
extern void (*Opcodes[0xFF]) (void);

static void print_tile(SDL_Renderer *renderer, uint16_t addr, int x, int y);
void print_tiles(SDL_Renderer *renderer);

#endif
