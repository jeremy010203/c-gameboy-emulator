#include "vram.h"

static void print_tile(SDL_Renderer *renderer, uint16_t addr, int x, int y)
{
  SDL_Point points[64];
  int k = 0;
  for (uint16_t i = 0; i < 8; i++)
  {
    uint16_t l1 = addr + (i * 2);
    uint16_t l2 = addr + (i * 2) + 1;

    for (uint16_t j = 0; j < 8; j++)
    {
      uint8_t d = 1 << (7 - j);
      uint8_t val = ((read_memory(l1) & d) ? 1 : 0) + ((read_memory(l2) & d) ? 2 : 0);
      if (val > 0)
      {
        points[k].x = x + j;
        points[k].y = y + i;
        k++;
        //SDL_RenderDrawPoint(renderer, x + j, y + i);
      }
    }
  }
  SDL_RenderDrawPoints(renderer, &points[0], k);
}

void print_tiles(SDL_Renderer *renderer)
{
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

  int i = 0;
  for (uint16_t j = 0x9800; j < 0x9BFF; j++)
  {
    print_tile(renderer, 0x8000 + read_memory(j) * 8 * 2, (i % 32) * 8 - read_memory(0xFF43), (i / 32) * 8 - read_memory(0xFF42));
    i++;
  }
}
