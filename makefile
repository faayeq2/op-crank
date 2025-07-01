CC = gcc
CFLAGS = -g -O3
SRC_DIR=src
BIN_DIR=bin
.PHONY: assembler run clean

$(BIN_DIR)/assembler: $(SRC_DIR)/assembler.c
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/assembler $(SRC_DIR)/assembler.c

assembler: $(BIN_DIR)/assembler

run: assembler
	./$(BIN_DIR)/assembler $(SRC_DIR)/program.asm

clean:
	rm -rf $(BIN_DIR)