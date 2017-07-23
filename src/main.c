#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "vram.h"
#include <SDL2/SDL.h>

int is_breakpoint(const int16_t breakpoints[100], const uint16_t addr)
{
  for (int i = 0; i < 100; i++)
  {
    if (breakpoints[i] == -1)
      return 0;
    if (breakpoints[i] == addr)
      return 1;
  }
  return 0;
}

void debug_mode(SDL_Renderer *renderer)
{

  int16_t breakpoints[100];
  for (int i = 0; i < 100; i++)
    breakpoints[i] = -1;

  while (1)
  {
    print_tiles(renderer);

    char input[10];
    printf("0x%x(0x%x)> ", r.PC.val, peak_byte());
    fgets(input, 10, stdin);

    if (strcmp(input, "r\n") == 0)
    {
      while (1)
      {
        uint8_t op = read_byte();
        execute(op);
        if (is_breakpoint(breakpoints, r.PC.val))
        {
          printf("Breakpoint reached.\n");
          break;
        }
      }
    }
    else if (strcmp(input, "n\n") == 0)
    {
      printf("%x -> ", r.PC.val);
      uint8_t op = read_byte();
      printf("%x\n", op);
      execute(op);
    }
    else if (strcmp(input, "show reg\n") == 0)
    {
      print_r();
    }
    else if (input[0] == 'b' && input[1] == ' ')
    {
      char *tmp = input + 2;
      uint16_t addr = strtoul(tmp, NULL, 16);
      if (breakpoints[99] > -1)
      {
        printf("Could not add a breakpoint you already have 100 breakpoints registered.");
      }

      for (int i = 0; i < 100; i++)
      {
        if (breakpoints[i] == -1)
        {
          breakpoints[i] = addr;
          break;
        }
      }
      printf("Add breakpoint at address %x.\n", addr);
    }
    else if (strcmp(input, "q\n") == 0)
    {
        exit(0);
    }
  }
}

int main(void)
{
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window* pWindow = NULL;
  SDL_Renderer *renderer = NULL;

  SDL_CreateWindowAndRenderer(256, 256, 0, &pWindow, &renderer);

  if (!pWindow)
  {
    fprintf(stderr,"Erreur de création de la fenêtre: %s\n", SDL_GetError());
  }
  else
  {
    SDL_RenderClear(renderer);
  }

  init();

  printf("Load bios successfully\n");
  int debug = 1;

  if (debug)
    debug_mode(renderer);

  while (1)
  {
    printf("%x -> ", r.PC.val);
    uint8_t op = read_byte();
    printf("%x\n", op);
    /*if (debug && op == 0xCD)
    {
      print_memory(r.PC.val, r.PC.val + 10);
      exit(1);
    }*/
    execute(op);
    //print_r();
  }

  SDL_DestroyWindow(pWindow);
  SDL_Quit();
  return 0;
}
