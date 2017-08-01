#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
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

void debug_mode(SDL_Renderer *renderer, int16_t* breakpoints, int *frame)
{
  char input[10];
  printf("0x%x(0x%x)> ", r.PC.val, peak_byte());
  fgets(input, 100, stdin);

  if (strcmp(input, "r\n") == 0)
  {
    while (1)
    {
      uint8_t op = read_byte();
      execute(op);
      if (renderer && MMU.memory[0xFF44] == 0)
      {
        (*frame)++;
        if (*frame % 60 == 0)
        {
          SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
          SDL_RenderClear(renderer);
          print_tiles(renderer);
          (*frame) = 0;
        }
      }
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
    if (renderer && MMU.memory[0xFF44] == 0)
    {
      SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
      SDL_RenderClear(renderer);
      print_tiles(renderer);
    }
  }
  else if (strcmp(input, "show reg\n") == 0)
  {
    print_r();
  }
  else if (strcmp(input, "show rom\n") == 0)
  {
    for (int16_t j = r.PC.val - 10; j <= r.PC.val + 10; j++)
    {
      if (j == r.PC.val)
        printf("-> ");
      if (j >= 0)
        printf("%2x \n", MMU.memory[j]);
    }
  }
  else if (strcmp(input, "show tilemap\n") == 0)
  {
    int i = 0;
    for (uint16_t j = 0x9800; j <= 0x9BFF; j++)
    {
      printf("%2x ", MMU.memory[j]);
      i++;
      if (i % 32 == 0)
        printf("\n");
    }
    printf("\n");
  }
  else if (input[0] == 'b' && input[1] == ' ')
  {
    char *tmp = input + 2;
    uint16_t addr = strtoul(tmp, NULL, 16);
    if (breakpoints[99] > -1)
      printf("Could not add a breakpoint you already have 100 breakpoints registered.\n");

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
  else
  {
      printf("Available commands:\n");
      printf("- 'r'          : run until a breakpoint\n");
      printf("- 'n'          : execute one op\n");
      printf("- 'b <address> : set a breakpoint at <address>\n");
      printf("- 'q'          : quit\n");
      printf("- 'show reg'   : print registers informations\n");
  }
}

// Handle Interupts
void handleInterupt(int nb)
{
  // Joypad interupt
  if (nb == 4)
  {

  }
}

void keyPressed(int key)
{
  if (key == -1)
    return;

  int unset = !test_bit(r.joypad, key);
  r.joypad &= ~(1 << key);

  int button = key > 3;
  uint8_t keyReq = MMU.memory[0xFF00];
  int requestInterupt = (button && !test_bit(keyReq, 5)) || (!button && !test_bit(keyReq, 4));
  if (requestInterupt && unset)
    handleInterupt(4);
}

void keyReleased(int key)
{
  if (key == -1)
    return;

  r.joypad |= (1 << key);
}

int main(int argc, char *args[])
{

  int debug = 0;
  int sdl = 0;

  for (int i = 1; i < argc; i++)
  {
    if (strcmp(args[i], "--debug") == 0)
      debug = 1;
    else if (strcmp(args[i], "--sdl") == 0)
      sdl = 1;
    else
    {
      printf("Unknown arg: %s\n", args[i]);
      exit(1);
    }
  }

  if (sdl)
    SDL_Init(SDL_INIT_VIDEO);

  SDL_Window* pWindow = NULL;
  SDL_Renderer *renderer = NULL;

  if (sdl)
  {
    SDL_CreateWindowAndRenderer(160 * 2, 144 * 2, 0, &pWindow, &renderer);

    if (!pWindow || !renderer)
    {
      fprintf(stderr,"Erreur de création de la fenêtre: %s\n", SDL_GetError());
    }
    else
    {
      SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
      SDL_RenderClear(renderer);
    }
  }

  init();

  printf("Load bios successfully\n");

  int16_t breakpoints[100];
  for (int i = 0; i < 100; i++)
    breakpoints[i] = -1;

  int frame = 0;

  while (1)
  {
    if (debug)
    {
      debug_mode(renderer, &breakpoints[0], &frame);
    }
    else
    {
      uint8_t op = read_byte();
      execute(op);
      if (renderer && MMU.memory[0xFF44] == 0)
      {
        frame++;
        if (frame % 60 == 0)
        {
          SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
          SDL_RenderClear(renderer);
          print_tiles(renderer);
          frame = 0;
        }
      }
    }

    if (sdl)
    {
      int key = -1;

      SDL_Event event;
      while(SDL_PollEvent(&event))
      {
        if (event.type == SDL_KEYUP || event.type == SDL_KEYDOWN)
        {
          switch(event.key.keysym.sym)
          {
            case SDLK_a:      key = 4; break;
            case SDLK_s:      key = 5; break;
            case SDLK_RETURN: key = 7; break;
            case SDLK_SPACE:  key = 6; break;
            case SDLK_LEFT:   key = 1; break;
            case SDLK_RIGHT:  key = 0; break;
            case SDLK_UP:     key = 2; break;
            case SDLK_DOWN:   key = 3; break;
            case SDL_QUIT:    exit(1); break;
          }
          if (key != -1)
            printf("Pressing key %d\n", key);
          else
            printf("OTHER EVENT\n");
          event.key.type == SDL_KEYUP ? keyReleased(key) : keyPressed(key);
        }
      }
    }
  }

  if (sdl)
  {
    SDL_DestroyWindow(pWindow);
    SDL_Quit();
  }

  return 0;
}
