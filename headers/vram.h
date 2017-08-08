#ifndef VRAM_H
# define VRAM_H

#include "mmu.h"
#include "registers.h"
#include <SDL2/SDL.h>

void print_tiles(SDL_Renderer *renderer);
void print_vram(SDL_Renderer *renderer);

#endif
