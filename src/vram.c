#include "vram.h"

static void print_tile(SDL_Renderer *renderer, uint16_t addr, int x, int y)
{
  //SDL_Point points[64];
  //int k = 0;
  for (uint16_t i = 0; i < 8; i++)
  {
    uint16_t l1 = addr + (i * 2);
    uint16_t l2 = addr + (i * 2) + 1;

    for (uint16_t j = 0; j < 8; j++)
    {
      uint8_t d = 1 << (7 - j);
      uint8_t val = ((read_memory(l1) & d) ? 1 : 0) + ((read_memory(l2) & d) ? 2 : 0);
      switch (val)
      {
        case 1:
          SDL_SetRenderDrawColor(renderer, 192, 192, 192, 255);
          break;
        case 2:
          SDL_SetRenderDrawColor(renderer, 96, 96, 96, 255);
          break;
        default:
          SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
          break;
      }
      if (val > 0)
      {
        //points[k].x = x + j;
        //points[k].y = y + i;
        //k++;
        SDL_RenderDrawPoint(renderer, x + j, y + i);
      }
    }
  }
  // SDL_RenderDrawPoints(renderer, &points[0], k);
}

void print_tiles(SDL_Renderer *renderer)
{
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

  //uint8_t windowsY = read_memory(0xFF4A);
  //uint8_t windowsX = read_memory(0xFF4B) - 7;

  uint8_t flags = read_memory(0xFF40);
  uint8_t tile_map = test_bit(flags, 3);
  uint8_t tile_set = test_bit(flags, 4);

  int i = 0;
  uint16_t start = tile_map ? 0x9c00 : 0x9800;
  uint16_t end = tile_map ? 0x9fff : 0x9bff;
  uint16_t start_set = tile_set ? 0x8000 : 0x8800;

  for (uint16_t j = start; j <= end; j++)
  {
    uint8_t mem = 0;
    mem = (!tile_set ? (((int)read_memory(j)) + 128) : read_memory(j));
    print_tile(renderer, start_set + mem * 8 * 2, (i % 32) * 8 - read_memory(0xFF43), (i / 32) * 8 - read_memory(0xFF42));
    i++;
  }
}

void print_vram(SDL_Renderer *renderer)
{
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

  int i = 0;
  for (uint16_t j = 0x8000; j < 0x8800 + 0x0800; j += 8*2)
  {
    print_tile(renderer, j, (i % 32) * 8 + 160, (i / 32) * 8);
    i++;
  }

  i = 0;
  for (uint16_t j = 0x8800 + 0x0800; j < 0x9800; j += 8*2)
  {
    print_tile(renderer, j, (i % 32) * 8 + 320, (i / 32) * 8);
    i++;
  }
}
