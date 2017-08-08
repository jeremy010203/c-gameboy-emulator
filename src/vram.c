#include "vram.h"

extern Mmu MMU;
extern Registers r;
extern My_clock my_clock;
extern void (*Opcodes[0xFF]) (void);

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
  uint8_t scrollY = read_memory(0xFF42);
  uint8_t scrollX = read_memory(0xFF43);
  uint8_t windowY = read_memory(0xFF4A);
  uint8_t windowX = read_memory(0xFF4B) - 7;
  uint8_t flags = read_memory(0xFF40);
  uint8_t window = (test_bit(flags, 5) && (windowY <= read_memory(0xFF44)));
  uint8_t tile_map = test_bit(flags, 3);
  uint8_t tile_set = test_bit(flags, 4);

  //int i = 0;
  uint16_t start_tile = 0;
  if (!window)
    start_tile = tile_map ? 0x9c00 : 0x9800;
  else
    start_tile = test_bit(flags, 6) ? 0x9c00 : 0x9800;

  //uint16_t end = tile_map ? 0x9fff : 0x9bff;
  uint16_t start_set = tile_set ? 0x8000 : 0x8800;

  uint16_t y = (window ? read_memory(0xFF44) - windowY : read_memory(0xFF44) + scrollY);
  uint16_t tile_row = ((uint8_t)(y / 8)) * 32;
  for (uint16_t j = 0; j < 160; j++)
  {
    uint8_t x = j + scrollX;
    if (window && (j >= windowX))
    {
      x = j - windowX;
    }

    uint16_t tile_col = x / 8;
    uint16_t addr = start_tile + tile_row + tile_col;
    uint16_t tile_loc = start_set + (!tile_set ? (((int)read_memory(addr)) + 128) : read_memory(addr)) * 16;
    uint8_t line = (y % 8) * 2;
    uint8_t data1 = read_memory(tile_loc + line);
    uint8_t data2 = read_memory(tile_loc + line + 1);

    int colourBit = x % 8 ;
    colourBit -= 7;
    colourBit *= -1;

    int colourNum = test_bit(data2, colourBit);
    colourNum <<= 1;
    colourNum |= test_bit(data1,colourBit);

    int col = 0;
    uint8_t palette = read_memory(0xFF47) ;
    int hi = 0 ;
    int lo = 0 ;

    switch (colourNum)
    {
      case 0: hi = 1 ; lo = 0 ;break ;
      case 1: hi = 3 ; lo = 2 ;break ;
      case 2: hi = 5 ; lo = 4 ;break ;
      case 3: hi = 7 ; lo = 6 ;break ;
    }

    int colour = 0;
    colour = test_bit(palette, hi) << 1;
    colour |= test_bit(palette, lo) ;
    col = colour;

    int red = 0;
    int green = 0;
    int blue = 0;

    switch(col)
    {
     case 0:	red = 255; green = 255 ; blue = 255; break;
     case 1:  red = 0xCC; green = 0xCC ; blue = 0xCC; break;
     case 2:	red = 0x77; green = 0x77 ; blue = 0x77; break;
    }

    SDL_SetRenderDrawColor(renderer, red, green, blue, 255);
    SDL_RenderDrawPoint(renderer, j, read_memory(0xFF44));
  }

  /*
  for (uint16_t j = start_map; j <= end; j++)
  {
    uint8_t mem = 0;
    mem = (!tile_set ? (((int)read_memory(j)) + 128) : read_memory(j));
    print_tile(renderer, start_set + mem * 8 * 2, (i % 32) * 8 - read_memory(0xFF43), (i / 32) * 8 - read_memory(0xFF42));
    i++;
  }
  */
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
