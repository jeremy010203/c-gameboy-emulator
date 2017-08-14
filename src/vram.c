#include "vram.h"

extern Mmu MMU;
extern Registers r;
extern My_clock my_clock;
extern void (*Opcodes[0xFF + 1]) (void);
extern void (*PrefixCB[0xFF + 1]) (void);
static int WIDTH = (160 + 320) * 2;
static int HEIGHT = 144 * 2 + 100;

static void print_tile(uint8_t pixels[], uint16_t addr, int x, int y)
{
  for (uint16_t i = 0; i < 8; i++)
  {
    uint16_t l1 = addr + (i * 2);
    uint16_t l2 = addr + (i * 2) + 1;

    for (uint16_t j = 0; j < 8; j++)
    {
      uint8_t d = 1 << (7 - j);
      uint8_t val = ((read_memory(l1) & d) ? 1 : 0) + ((read_memory(l2) & d) ? 2 : 0);

      uint8_t red = 0;
      uint8_t green = 0;
      uint8_t blue = 0;
      switch (val)
      {
        case 1:
          red = 192;
          green = 192;
          blue = 192;
          break;
        case 2:
          red = 96;
          green = 96;
          blue = 96;
          break;
      }
      if (val > 0)
      {
        unsigned int offset = (WIDTH * 4 * (y + i)) + (x + j) * 4;
        pixels[offset] = red;
        pixels[offset + 1] = green;
        pixels[offset + 2] = blue;
        pixels[offset + 3] = 255;
      }
    }
  }
}

void print_sprites(uint8_t pixels[])
{
  uint8_t flags = MMU.memory[0xFF40];
  uint8_t double_sprite = test_bit(flags, 2);
  for (int i = 0; i < 40; i++)
  {
    int index = i * 4;
    int yPos = MMU.memory[0xFE00 + index] - 16;
    int xPos = MMU.memory[0xFE00 + index + 1] - 8;
    int tileLocation = MMU.memory[0xFE00 + index + 2];
    int attributes = MMU.memory[0xFE00 + index + 3];

    int yFlip = test_bit(attributes, 6);
    int xFlip = test_bit(attributes, 5);
    int scanline = MMU.memory[0xFF44];
    int ysize = double_sprite ? 16 : 8;

    if ((scanline >= yPos) && (scanline < (yPos + ysize)))
    {
      int line = scanline - yPos;
      if (yFlip)
        line = -(line - ysize);

      line *= 2;
      uint16_t dataAddress = (0x8000 + (tileLocation * 16)) + line;
      uint8_t data1 = MMU.memory[dataAddress];
      uint8_t data2 = MMU.memory[dataAddress + 1];

      for (int tilePixel = 7; tilePixel >= 0; tilePixel--)
      {
        int colourbit = tilePixel;
        if (xFlip)
          colourbit = -(colourbit - 7);

         int colourNum = test_bit(data2, colourbit);
         colourNum <<= 1;
         colourNum |= test_bit(data1, colourbit);

         int col = 0;
         uint8_t palette = MMU.memory[(test_bit(attributes, 4) ? 0xFF49 : 0xFF48)];
         int hi = 0;
         int lo = 0;

         switch (colourNum)
         {
           case 0: hi = 1; lo = 0; break;
           case 1: hi = 3; lo = 2; break;
           case 2: hi = 5; lo = 4; break;
           case 3: hi = 7; lo = 6; break;
         }

         int colour = 0;
         colour = test_bit(palette, hi) << 1;
         colour |= test_bit(palette, lo);
         col = colour;

         if (col == 0)
           continue;

         int red = 0;
         int green = 0;
         int blue = 0;

         switch(col)
         {
          case 0:	red = 255; green = 255; blue = 255; break;
          case 1: red = 0xCC; green = 0xCC; blue = 0xCC; break;
          case 2:	red = 0x77; green = 0x77; blue = 0x77; break;
         }

         int pixel = xPos - tilePixel + 7;
         const unsigned int offset = (WIDTH * 4 * scanline) + pixel * 4;
         if (offset < (unsigned int)(WIDTH*HEIGHT*4))
         {
           pixels[offset] = red;
           pixels[offset + 1] = green;
           pixels[offset + 2] = blue;
           pixels[offset + 3] = 255;
        } else {
          //printf("offset > screen -> offset = %d\n", offset);
        }
       }
     }
  }
}

void print_tiles(uint8_t pixels[])
{
  uint8_t scrollY  = MMU.memory[0xFF42];
  uint8_t scrollX  = MMU.memory[0xFF43];
  uint8_t windowY  = MMU.memory[0xFF4A];
  uint8_t windowX  = MMU.memory[0xFF4B] - 7;
  uint8_t flags    = MMU.memory[0xFF40];
  uint8_t window   = (test_bit(flags, 5) && (windowY <= MMU.memory[0xFF44]));
  uint8_t tile_map = test_bit(flags, 3);
  uint8_t tile_set = test_bit(flags, 4);

  if (test_bit(flags, 0))
  {
    uint16_t start_tile = 0;
    if (!window)
      start_tile = tile_map ? 0x9c00 : 0x9800;
    else
      start_tile = test_bit(flags, 6) ? 0x9c00 : 0x9800;

    uint16_t start_set = tile_set ? 0x8000 : 0x8800;

    uint8_t y = (window ? MMU.memory[0xFF44] - windowY : MMU.memory[0xFF44] + scrollY);
    uint16_t tile_row = ((uint8_t)(y / 8)) * 32;

    for (int j = 0; j < 160; j++)
    {
      uint8_t x = j + scrollX;
      if (window && (j >= windowX))
        x = j - windowX;

      uint16_t tile_col = x / 8;
      uint16_t addr = start_tile + tile_row + tile_col;
      uint16_t tile_loc = start_set;
      if (!tile_set)
        tile_loc += (((int)(((int8_t)MMU.memory[addr]))) + 128) * 16;
      else
        tile_loc += ((uint16_t)MMU.memory[addr]) * 16;

      uint8_t line = (y % 8) * 2;
      uint8_t data1 = MMU.memory[tile_loc + line];
      uint8_t data2 = MMU.memory[tile_loc + line + 1];

      int colourBit = x % 8;
      colourBit -= 7;
      colourBit *= -1;

      int colourNum = test_bit(data2, colourBit);
      colourNum <<= 1;
      colourNum |= test_bit(data1, colourBit);

      int col = 0;
      uint8_t palette = MMU.memory[0xFF47];
      int hi = 0;
      int lo = 0;

      switch (colourNum)
      {
        case 0: hi = 1; lo = 0; break;
        case 1: hi = 3; lo = 2; break;
        case 2: hi = 5; lo = 4; break;
        case 3: hi = 7; lo = 6; break;
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
       case 0: red = 255; green = 255; blue = 255; break;
       case 1: red = 0xCC; green = 0xCC; blue = 0xCC; break;
       case 2: red = 0x77; green = 0x77; blue = 0x77; break;
      }

      const unsigned int offset = (WIDTH * 4 * MMU.memory[0xFF44]) + j * 4;
      pixels[offset] = red;
      pixels[offset + 1] = green;
      pixels[offset + 2] = blue;
      pixels[offset + 3] = 255;
    }
  }
}

void print_vram(uint8_t pixels[])
{
  int i = 0;
  for (uint16_t j = 0x8000; j < 0x9800; j += 8*2)
  {
    print_tile(pixels, j, (i % 32) * 8 + 160, (i / 32) * 8);
    i++;
  }
}
