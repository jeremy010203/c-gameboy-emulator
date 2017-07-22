SOURCE_DIR=src
HEADER_DIR=headers

SOURCE_FILES= \
$(SOURCE_DIR)/main.c \
$(SOURCE_DIR)/registers.c \
$(SOURCE_DIR)/mmu.c \
$(SOURCE_DIR)/utils.c

all:
	gcc -g -I$(HEADER_DIR) $(SOURCE_FILES) -o main

clean:
	$(RM) main
	$(RM) *~
	$(RM) src/*~
