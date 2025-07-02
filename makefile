CC = gcc
CFLAGS = -g -O3
SRC_DIR=src
BIN_DIR=bin

.PHONY: all assembler run clean #phony targets

all:assembler

assembler: $(BIN_DIR)/assembler

$(BIN_DIR)/assembler: $(SRC_DIR)/assembler.c
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/assembler $(SRC_DIR)/assembler.c

run: assembler
	./$(BIN_DIR)/assembler $(SRC_DIR)/program.asm

clean:
	rm -rf $(BIN_DIR)