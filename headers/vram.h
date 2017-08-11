#ifndef VRAM_H
# define VRAM_H

#include "mmu.h"
#include "registers.h"
#include <SDL2/SDL.h>

void print_tiles(uint8_t pixels[]);
void print_sprites(uint8_t pixels[]);
void print_vram(uint8_t pixels[]);

#endif
