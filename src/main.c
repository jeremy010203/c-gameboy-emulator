#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "utils.h"
#include "vram.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

void handleInterupt(int nb);
void keyPressed(int key);
void keyReleased(int key);
void print_joypad(SDL_Renderer *renderer, SDL_Texture *imgs[], SDL_Rect rects[]);

static int trace = 0;
static int debug = 0;
static int sdl = 0;
static int WIDTH = (160 + 320) * 2;
static int HEIGHT = 144 * 2 + 100;

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

void print_screen(SDL_Renderer *renderer, SDL_Texture *texture, uint8_t pixels[]
                  , SDL_Texture *imgs[], SDL_Rect rects[])
{
  SDL_UpdateTexture
      (
      texture,
      NULL,
      &pixels[0],
      WIDTH * 4
      );

  SDL_Rect src_rect = {0, 0, 160, 144};
  SDL_Rect dst_rect = {0, 0, 160, 144};
  SDL_RenderCopy(renderer, texture, &src_rect, &dst_rect);

  SDL_Rect rect = {0, 144, 160, 100};
  SDL_Rect rect2 = {160, 0, 320, 144 + 100};
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderFillRect(renderer, &rect);
  SDL_RenderFillRect(renderer, &rect2);

  print_joypad(renderer, imgs, rects);
  //print_vram(renderer);

  SDL_RenderSetScale(renderer, 2, 2);
  SDL_RenderPresent(renderer);
}

void debug_mode(SDL_Renderer *renderer, SDL_Texture *texture, uint8_t pixels[], int16_t* breakpoints
              , SDL_Texture *imgs[], SDL_Rect rects[])
{
  char input[10];
  printf("0x%x(0x%x)> ", r.PC.val, peak_byte());
  fgets(input, 100, stdin);

  if (strcmp(input, "r\n") == 0)
  {
    while (1)
    {
      uint8_t op = read_byte();
      if (MMU.HALT)
        r.PC.val--;

      if (trace)
        printf("At 0x%x : 0x%x\n", r.PC.val - 1, op);
      int a = 0;
      execute(op, pixels, &a);

      if (renderer && a)
      {
        SDL_PumpEvents();
        print_screen(renderer, texture, pixels, imgs, rects);
      }

      if (renderer)
      {
        const Uint8 *state = SDL_GetKeyboardState(NULL);
        state[SDL_SCANCODE_A] ? keyPressed(4) : keyReleased(4);
        state[SDL_SCANCODE_S] ? keyPressed(5) : keyReleased(5);
        state[SDL_SCANCODE_RETURN] ? keyPressed(7) : keyReleased(7);
        state[SDL_SCANCODE_SPACE] ? keyPressed(6) : keyReleased(6);
        state[SDL_SCANCODE_LEFT] ? keyPressed(1) : keyReleased(1);
        state[SDL_SCANCODE_RIGHT] ? keyPressed(0) : keyReleased(0);
        state[SDL_SCANCODE_UP] ? keyPressed(2) : keyReleased(2);
        state[SDL_SCANCODE_DOWN] ? keyPressed(3) : keyReleased(3);
        if (state[SDL_SCANCODE_Q])
          break;
        if (state[SDL_SCANCODE_ESCAPE])
          exit(1);
      }
      do_interupt();

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

    int a = 0;
    execute(op, pixels, &a);
    if (renderer && a)
    {
      SDL_PumpEvents();
      print_screen(renderer, texture, pixels, imgs, rects);
    }

    do_interupt();
    print_r();
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
        printf("%2x \n", read_memory(j));
    }
  }
  else if (strcmp(input, "show tilemap\n") == 0)
  {
    int i = 0;
    for (uint16_t j = 0x9800; j <= 0x9BFF; j++)
    {
      printf("%2x ", read_memory(j));
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
      printf("- 'r'            : run until a breakpoint\n");
      printf("- 'n'            : execute one op\n");
      printf("- 'b <address>   : set a breakpoint at <address>\n");
      printf("- 'q'            : quit\n");
      printf("- 'show reg'     : print registers informations\n");
      printf("- 'show rom'     : print rom informations -10<PC<+10\n");
      printf("- 'show tilemap' : print tilemap informations\n");
  }
}

void keyPressed(int key)
{
  if (key <= -1)
    return;

  int unset = !test_bit(r.joypad, key);
  r.joypad &= ~(1 << key);

  int button = key > 3;
  uint8_t keyReq = MMU.memory[0xFF00];

  int requestInterupt = (button && !test_bit(keyReq, 5)) || (!button && !test_bit(keyReq, 4));
  if (requestInterupt && !unset)
    request_interupt(4);
}

void keyReleased(int key)
{
  if (key == -1)
    return;

  r.joypad |= (1 << key);
}

void print_joypad(SDL_Renderer *renderer, SDL_Texture *imgs[], SDL_Rect rects[])
{
  if (!test_bit(r.joypad, 0))
    SDL_RenderCopy(renderer, imgs[0], NULL, &rects[0]);
  else if (!test_bit(r.joypad, 1))
    SDL_RenderCopy(renderer, imgs[1], NULL, &rects[1]);
  else if (!test_bit(r.joypad, 2))
    SDL_RenderCopy(renderer, imgs[2], NULL, &rects[2]);
  else if (!test_bit(r.joypad, 3))
    SDL_RenderCopy(renderer, imgs[3], NULL, &rects[3]);
  else
    SDL_RenderCopy(renderer, imgs[8], NULL, &rects[8]);
}

void handle_args(int argc, char *args[])
{
  for (int i = 1; i < argc; i++)
  {
    if (strcmp(args[i], "--debug") == 0)
      debug = 1;
    else if (strcmp(args[i], "--sdl") == 0)
      sdl = 1;
    else if (strcmp(args[i], "--trace") == 0)
      trace = 1;
    else if (strcmp(args[i], "--rom") == 0)
    {
      MMU.path_rom = malloc(strlen(args[i + 1]));
      memcpy(MMU.path_rom, args[i + 1], strlen(args[i + 1]) + 1);
      i++;
    }
    else
    {
      printf("Unknown arg: %s\n", args[i]);
      exit(1);
    }
  }
}

int main(int argc, char *args[])
{
  handle_args(argc, args);

  if (sdl)
    SDL_Init(SDL_INIT_VIDEO);

  uint8_t pixels[WIDTH * HEIGHT * 4];
  SDL_Window* pWindow = NULL;
  SDL_Renderer *renderer = NULL;
  SDL_Texture* texture = NULL;
  SDL_Texture *imgs[9];
  SDL_Rect rects[9];
  rects[0].x = 10; rects[0].y = 144 + 5; rects[0].w = 40; rects[0].h = 40;
  rects[1].x = 10; rects[1].y = 144 + 5; rects[1].w = 40; rects[1].h = 40;
  rects[2].x = 10; rects[2].y = 144 + 5; rects[2].w = 40; rects[2].h = 40;
  rects[3].x = 10; rects[3].y = 144 + 5; rects[3].w = 40; rects[3].h = 40;
  rects[8].x = 10; rects[8].y = 144 + 5; rects[8].w = 40; rects[8].h = 40;

  if (sdl)
  {
    SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &pWindow, &renderer);
    texture = SDL_CreateTexture
        (
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        WIDTH, HEIGHT
        );

    if (!pWindow || !renderer)
    {
      fprintf(stderr,"Erreur de création de la fenêtre: %s\n", SDL_GetError());
    }
    else
    {
      imgs[8] = IMG_LoadTexture(renderer, "imgs/release.png");
      imgs[0] = IMG_LoadTexture(renderer, "imgs/right.png");
      imgs[1] = IMG_LoadTexture(renderer, "imgs/left.png");
      imgs[2] = IMG_LoadTexture(renderer, "imgs/up.png");
      imgs[3] = IMG_LoadTexture(renderer, "imgs/down.png");
      SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
      SDL_RenderClear(renderer);
    }
  }

  init();

  int16_t breakpoints[100];
  for (int i = 0; i < 100; i++)
    breakpoints[i] = -1;

  while (1)
  {
    if (debug)
    {
      debug_mode(renderer, texture, pixels, &breakpoints[0], imgs, rects);
    }
    else
    {
      uint8_t op = read_byte();
      if (MMU.HALT)
        r.PC.val--;
      int a = 0;
      execute(op, pixels, &a);

      if (renderer && a)
      {
        SDL_PumpEvents();
        print_screen(renderer, texture, pixels, imgs, rects);
      }

      if (renderer)
      {
        const Uint8 *state = SDL_GetKeyboardState(NULL);
        state[SDL_SCANCODE_A] ? keyPressed(4) : keyReleased(4);
        state[SDL_SCANCODE_S] ? keyPressed(5) : keyReleased(5);
        state[SDL_SCANCODE_RETURN] ? keyPressed(7) : keyReleased(7);
        state[SDL_SCANCODE_SPACE] ? keyPressed(6) : keyReleased(6);
        state[SDL_SCANCODE_LEFT] ? keyPressed(1) : keyReleased(1);
        state[SDL_SCANCODE_RIGHT] ? keyPressed(0) : keyReleased(0);
        state[SDL_SCANCODE_UP] ? keyPressed(2) : keyReleased(2);
        state[SDL_SCANCODE_DOWN] ? keyPressed(3) : keyReleased(3);
        if (state[SDL_SCANCODE_Q])
          break;
        if (state[SDL_SCANCODE_ESCAPE])
          exit(1);
      }
      do_interupt();
    }
  }

  if (sdl)
  {
    for (int i = 0; i < 9; i++)
      SDL_DestroyTexture(imgs[i]);

    SDL_DestroyWindow(pWindow);
    SDL_Quit();
  }

  return 0;
}
