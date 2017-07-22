#include "mmu.h"

void init_mmu(char *path)
{
  load_bios(path);
}

static void load_bios(char *path)
{
  FILE *file;
  unsigned long len;
  file = fopen(path, "rb");
  if (!file)
  {
    fprintf(stderr, "Error loading file %s\n", path);
    exit(1);
  }
  fseek(file, 0, SEEK_END);
  len = ftell(file);
  rewind(file);

  fread(&MMU.memory, len, 1, file);
  fclose(file);
}
