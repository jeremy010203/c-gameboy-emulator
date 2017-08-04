SOURCE_DIR=src
HEADER_DIR=headers

TEST_DIR=tests
HEADER_TEST_DIR=tests

CFLAGS= -Werror -Wall -Wextra -g
SDL_DIR=/Library/Frameworks/SDL2.framework

SOURCE_FILES= \
$(SOURCE_DIR)/registers.c \
$(SOURCE_DIR)/mmu.c \
$(SOURCE_DIR)/utils.c \
$(SOURCE_DIR)/vram.c \
$(SOURCE_DIR)/helpers_op.c

TEST_FILES= \
$(TEST_DIR)/helpers.c \
$(TEST_DIR)/cpu_tests.c

all: main

main:
	gcc-7 -g -I$(HEADER_DIR) $(SOURCE_FILES) $(SOURCE_DIR)/main.c -lSDL2 -lSDL2_image -o main $(CFLAGS)

test: main
	gcc-7 -I$(HEADER_DIR) -I$(HEADER_TEST_DIR) $(SOURCE_DIR)/registers.c $(SOURCE_DIR)/mmu.c $(SOURCE_DIR)/utils.c $(SOURCE_DIR)/helpers_op.c $(TEST_FILES) -lcunit -o test $(CFLAGS)
	./test

clean:
	$(RM) main
	$(RM) test
	$(RM) *~
	$(RM) *#
	$(RM) src/*~
	$(RM) -r .DS_STORE
	$(RM) -r *.dSYM

.PHONY: clean all
