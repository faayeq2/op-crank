#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAP_SIZE	  100
#define MAX_CODE_SIZE 1024
#define MAX_DATA_SIZE 512

typedef struct sym_pair {
	struct sym_pair *next; // 8 bytes
	char *label;		   // 8 bytes
	int address;		   // 4 bytes
} sym_pair;

typedef struct sym_table {
	sym_pair *sym_table[MAP_SIZE];
} sym_table;

typedef struct {
	unsigned char *code_seg;
	unsigned char *data_seg;
	unsigned int *code_ptr;
	unsigned int *data_ptr;
	sym_table *table;
	unsigned int line_num;
} assembler_state;

sym_table global_table = {{NULL}};

typedef struct instruction {
	const char *mnemonic;
	const char **operand_types;
	int operand_count;
	unsigned char opcode;
	int byte_size;
} instruction;

instruction instruction_set[] = {
	{"MOV", (const char *[]){"reg", "reg"}, 2, 0x89, 2},
	{"ADD", (const char *[]){"reg", "imm"}, 2, 0x83, 3},
	{"SUB", (const char *[]){"reg", "reg"}, 2, 0x29, 2},
	{"CMP", (const char *[]){"reg", "reg"}, 2, 0x39, 2},
	{"JMP", (const char *[]){"label"}, 1, 0xEB, 2},
};

int instruction_set_size = (sizeof(instruction_set) / sizeof(instruction_set[0]));

unsigned int hash_func(const char *label) {
	unsigned int hash = 0;
	while (*label != '\0') {
		hash = (hash * 31) + *label;
		label++;
	}
	return hash % MAP_SIZE;
}

void print_sym_table(sym_table *table) {
	printf("\n-------Printing symbol table:-------\n");
	for (int i = 0; i < MAP_SIZE; i++) {
		sym_pair *current = table->sym_table[i];
		while (current != NULL) {
			printf("Index: %d, Label: %s, Address: %d\n", i, current->label, current->address);
			current = current->next;
		}
	}
}

void insert_label(const char *label, unsigned int addr, sym_table *table) {
	unsigned int index = hash_func(label);

	sym_pair *pair = malloc(sizeof(sym_pair)); // allocate pair to add
	if (pair == NULL) {
		perror("Cant allocate mem for new pair");
		exit(1);
	}

	pair->label = malloc(strlen(label) + 1); // allocate mem for the label char*
	if (pair->label == NULL) {
		perror("Cant allocate pair to insert's label");
		exit(1);
	}

	pair->address = addr;						   // update label and insert in hash table (avoid collisions)
	memcpy(pair->label, label, strlen(label) + 1); // syn to pair.label = label
	pair->next = NULL;

	// handle collision
	sym_pair *current = table->sym_table[index];
	if (current == NULL) {
		table->sym_table[index] = pair; // no collision
	} else {
		pair->next				= current; // allocate mem for the label char*
		table->sym_table[index] = pair;
	}

	printf("Inserted label: %s at address: %d\n", label, addr);
	
}

unsigned int duplicate_label(const char *token) {
	for (int i = 0; i < MAP_SIZE; i++) {
		sym_pair *current = global_table.sym_table[i];
		while (current != NULL) {
			if (strcmp(current->label, token) == 0) {
				return 1;
			}
			current = current->next;
		}
	}
	return 0;
}

void process_directives(const char *token, const char *rest_of_line, assembler_state *state) {
	if (strcmp(token, ".data") == 0) {
		printf("Switched to data seg:\n");
		*(state->code_ptr) = 0; // reset code ptr when section .data 
		return;
	} else if (strcmp(token, ".text") == 0) {
		printf("Switched to code seg:\n");
		return;
	}
	
}

void process_tokens(const char *line, assembler_state *state) {

	const char *delimiters = "\t,\n()[]; "; // split each line into tokens based on delims

	char *line_copy = malloc(strlen(line) + 1); // use line_copy as strtok changes input string
	if (line_copy == NULL) {
		perror("Couldn't allocate line");
		exit(1);
	}
	memcpy(line_copy, line, strlen(line) + 1);

	char *cmnt_start = strchr(line_copy, ';');
	if (cmnt_start != NULL) {
		*(cmnt_start) = '\0'; // truncate string,,,,,,
	}

	char *token = strtok(line_copy, delimiters);

	while (token != NULL) {
		// check if label
		if (token[strlen(token) - 1] == ':') {
			token[strlen(token) - 1] = '\0';

			if (duplicate_label(token) == 0) { // check for duplicating labels and insert...
				insert_label(token, *(state->code_ptr), &global_table);
				printf("tokens = [%s]\n", token);
			} else {
				printf("duplicate token : [%s]\n", token);
			}

			token = strtok(NULL, delimiters);
			continue;
		}

		else if (token[0] == '.') { // process directives
			const char *rest_of_line = strtok(NULL, "");
			process_directives(token, rest_of_line, state);
			token = strtok(NULL, delimiters);
			continue;
		}

		else if (strcmp(token, "DB") == 0 || strcmp(token, "DW") == 0 ||
				 strcmp(token, "DD") == 0) { // process data directives
			const char *rest_of_line = strtok(NULL, "");
			process_directives(token, rest_of_line, state);
			break;
		}

		else {
			int is_instruction = 0;

			for (int i = 0; i < instruction_set_size; i++) {
				if (strcmp(instruction_set[i].mnemonic, token) == 0) {
					printf("Instruction found: %s\n", token);
					is_instruction = 1;
					*(state->code_ptr) += instruction_set[i].byte_size;
					break;
				}
			}

			if (!is_instruction) {
				printf("Not an instruction\n");
			}
		}

		token = strtok(NULL, delimiters);
	}

	print_sym_table(&global_table);
	printf("Current code pointer: %d\n", *(state->code_ptr));

	free(line_copy);
}

void free_sym_table(sym_table *table) {
	for (int i = 0; i < MAP_SIZE; i++) {
		sym_pair *current = table->sym_table[i];
		while (current != NULL) {
			sym_pair *next = current->next;
			free(current->label);
			free(current);
			current = next;
		}
		table->sym_table[i] = NULL;
	}
}

unsigned char encode_reg(const char *reg) {
	// simple register encoding: AX=0, BX=1, CX=2, DX=3 (expand as needed)
	if (strcmp(reg, "AX") == 0) return 0;
	if (strcmp(reg, "BX") == 0) return 1;
	if (strcmp(reg, "CX") == 0) return 2;
	if (strcmp(reg, "DX") == 0) return 3;
	return 0xFF; // invalid
}

void second_pass(const char *f_name, assembler_state *state) {
	FILE *file = fopen(f_name, "r");
	if (file == NULL) {
		perror("Can't open file for second pass");
		return;
	}
	unsigned int code_ptr = 0;
	char line[64];
	while (fgets(line, sizeof(line), file)) {
		char *line_copy = malloc(strlen(line) + 1);
		if (line_copy == NULL) {
			perror("Couldn't allocate line");
			exit(1);
		}
		memcpy(line_copy, line, strlen(line) + 1);
		char *cmnt_start = strchr(line_copy, ';');
		if (cmnt_start != NULL) *cmnt_start = '\0';
		char *token = strtok(line_copy, "\t,\n()[]; ");
		while (token != NULL) {
			if (token[strlen(token) - 1] == ':') {
				token = strtok(NULL, "\t,\n()[]; ");
				continue;
			}
			else if (token[0] == '.') {
				token = strtok(NULL, "\t,\n()[]; ");
				continue;
			}
			else if (strcmp(token, "DB") == 0 || strcmp(token, "DW") == 0 || strcmp(token, "DD") == 0) {
				break;
			}
			else {
				int found = 0;
				for (int i = 0; i < instruction_set_size; i++) {
					if (strcmp(instruction_set[i].mnemonic, token) == 0) {
						// encoding: write opcode to code segment
						state->code_seg[code_ptr++] = instruction_set[i].opcode;
						for (int op = 0; op < instruction_set[i].operand_count; op++) {
							char *operand = strtok(NULL, "\t,\n()[]; ");
							if (operand == NULL) break;
							if (strcmp(instruction_set[i].operand_types[op], "reg") == 0) {
								// encoding: write register code
								unsigned char reg_code = encode_reg(operand);
								state->code_seg[code_ptr++] = reg_code;
							} else if (strcmp(instruction_set[i].operand_types[op], "imm") == 0) {
								// encoding: write immediate value
								int imm = atoi(operand);
								state->code_seg[code_ptr++] = (unsigned char)imm;
							} else if (strcmp(instruction_set[i].operand_types[op], "label") == 0) {
								// resolving: find label address and write it
								int addr = -1;
								for (int j = 0; j < MAP_SIZE; j++) {
									sym_pair *cur = state->table->sym_table[j];
									while (cur != NULL) {
										if (strcmp(cur->label, operand) == 0) {
											addr = cur->address;
											break;
										}
										cur = cur->next;
									}
									if (addr != -1) break;
								}
								if (addr == -1) {
									printf("Undefined label: %s\n", operand);
									state->code_seg[code_ptr++] = 0xFF;
								} else {
									state->code_seg[code_ptr++] = (unsigned char)addr;
								}
							}
						}
						found = 1;
						break;
					}
				}
				token = strtok(NULL, "\t,\n()[]; ");
			}
		}
		free(line_copy);
	}
	fclose(file);

	// write code segment to output file
	FILE *out = fopen("bin/output.bin", "wb");
	if (out == NULL) {
		perror("Can't open output file");
		return;
	}
	fwrite(state->code_seg, 1, code_ptr, out);
	fclose(out);
	printf("Machine code written to output.bin (%u bytes)\n", code_ptr);
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Using: %s <f_name>\n", argv[0]);
		return 1;
	}
	const char *f_name = argv[1];

	FILE *file = fopen(f_name, "r");
	if (file == NULL) {
		perror("Can't open file for reading");
		return 1;
	}

	unsigned char code_seg[MAX_CODE_SIZE];
	unsigned int code_ptr = 0;
	unsigned char data_seg[MAX_DATA_SIZE];
	unsigned int data_ptr = 0;

	char line[64];

	assembler_state state = {
		.code_seg = code_seg,
		.data_seg = data_seg,
		.code_ptr = &code_ptr,
		.data_ptr = &data_ptr,
		.table    = &global_table,
		.line_num = 0,
	};

	// first pass
	while (fgets(line, sizeof(line), file)) {
		state.line_num++;
		process_tokens(line, &state);
	}
	fclose(file);

	// second pass
	second_pass(f_name, &state);

	// print code segment
	printf("Code segment:\n");
	for (int i = 0; i < code_ptr; i++) {
		printf("%02X ", code_seg[i]);
	}
	printf("\n");

	// free symbol table
	free_sym_table(&global_table);

	return 0;
}