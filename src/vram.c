#include "vram.h"

static void print_tile(SDL_Renderer *renderer, uint16_t addr, int x, int y)
{
  for (uint16_t i = 0; i < 8; i++)
  {
    uint16_t l1 = addr + (i * 2);
    uint16_t l2 = addr + (i * 2) + 1;

    for (uint16_t j = 0; j < 8; j++)
    {
      uint8_t d = 1 << (7 - j);
      uint8_t val = ((MMU.memory[l1] & d) ? 1 : 0) + ((MMU.memory[l2] & d) ? 2 : 0);
      if (val > 0)
        SDL_RenderDrawPoint(renderer, x + j, y + i);
    }
  }
}

void print_tiles(SDL_Renderer *renderer)
{
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

  int i = 0;
  for (uint16_t j = 0x8000; j < 0x8800 + 0x0800; j += 8*2)
  {
    print_tile(renderer, j, (i % 32) * 8, (i / 32) * 8);
    i++;
  }
  SDL_RenderPresent(renderer);
}
