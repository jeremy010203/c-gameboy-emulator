SOURCE_DIR=src
HEADER_DIR=headers
CFLAGS= -Werror -Wall -Wextra -g
SDL_DIR=/Library/Frameworks/SDL2.framework

SOURCE_FILES= \
$(SOURCE_DIR)/main.c \
$(SOURCE_DIR)/registers.c \
$(SOURCE_DIR)/mmu.c \
$(SOURCE_DIR)/utils.c \
$(SOURCE_DIR)/vram.c

all:
	gcc-7 -g -I$(HEADER_DIR) $(SOURCE_FILES) -lSDL2 -o main $(CFLAGS)

clean:
	$(RM) main
	$(RM) *~
	$(RM) *#
	$(RM) src/*~
	$(RM) -r .DS_STORE
	$(RM) -r *.dSYM
