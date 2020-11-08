#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define STACK_SIZE 256 static int stack[STACK_SIZE];

typedef enum {
	PSH,
	ADD,
	POP,
	SET,
	HLT
} InstructionSet;

typedef enum {
	A, B, C, D, E, F, I, J,
	EX,
	EXA,
	IP,
	SP,
	REGISTER_SIZE
} Registers;

static int registers[REGISTER_SIZE];

int *instructions;

int instruction_count = 0;

int instruction_space = 4;

static bool running = true;

bool is_jmp = false;

#define SP (registers[SP])
#define IP (registers[IP])

#define FETCH (instructions[IP])

void print_stack() {
	for (int i = 0; i < SP; i++) {
		printf("0x%04d ", stack[i]);
		if ((i + 1) % 4 == 0) { printf("\n"); }	
	}
	if (SP != 0) { printf("\n"); }
}

void print_registers() {
	printf("Register Dump:\n");
	for (int i = 0; i < REGISTER_SIZE; i++) {
		if (i != registers[EX] && i != registers[EXA]) { return i; }
	}
	return EX;
}

void eval(int instr) {
	is_jmp = false;
	switch (instr) {
		case HLT: {
			running = false;
			printf("Finished Execution\n");
			break;
		}
		case PSH: {
			SP = SP + 1;
			IP = IP + 1;
			stack[SP] = instructions[IP];
			break;
		}
		case ADD: {
			registers[A] stack[SP];
			SP = SP - 1;

			registers[B] = stack[SP];

			registers[C] = registers[B] + registers[A];

			stack[SP] = registers[C];

			printf("%d + %d = %d\n", registers[B], registers[A], registers[C]);
			break;
		}
		case MUL: {
            registers[A] = stack[SP];
            SP = SP - 1;

            registers[B] = stack[SP];

            registers[C] = registers[B] * registers[A];

            stack[SP] = registers[C];
            printf("%d * %d = %d\n", registers[B], registers[A], registers[C]);
            break;
        }
        case DIV: {
            registers[A] = stack[SP];
            SP = SP - 1;

            registers[B] = stack[SP];

            registers[C] = registers[B] / registers[A];

            stack[SP] = registers[C];
            printf("%d / %d = %d\n", registers[B], registers[A], registers[C]);
            break;
        }
        case SUB: {
            registers[A] = stack[SP];
            SP = SP - 1;

            registers[B] = stack[SP];

            registers[C] = registers[B] - registers[A];

            stack[SP] = registers[C];
            printf("%d - %d = %d\n", registers[B], registers[A], registers[C]);
            break;
        }
        case SLT: {
            SP = SP - 1;
            stack[SP] = stack[SP+1] < stack[SP];
            break;
        }
        case MOV: {
            registers[instructions[IP + 2]] = registers[instructions[IP + 1]];
            IP = IP + 2;
            break;
        }
        case SET: {
            registers[instructions[IP + 1]] = instructions[IP + 2];
            IP = IP + 2;
            break;
        }
        case LOG: {
            printf("%d\n", registers[instructions[IP + 1]]);
            IP = IP + 1;
            break;
        }
        case IF: {
            if (registers[instructions[IP + 1]] == instructions[IP + 2]) {
                IP = instructions[IP + 3];
                is_jmp = true;
            }
            else{
                IP = IP + 3;
            }
            break;
        }
        case IFN: {
            if (registers[instructions[IP + 1]] != instructions[IP + 2]) {
                IP = instructions[IP + 3];
                is_jmp = true;
            }
            else {
                IP = IP + 3;
            }
            break;
        }
        case GLD: {
            SP = SP + 1;
            IP = IP + 1;
            stack[SP] = registers[instructions[IP]];
            break;
        }
        case GPT: {
            registers[instructions[IP + 1]] = stack[SP];
            IP = IP + 1;
            break;
        }
        case NOP: {
            printf("Do Nothing\n");
            break;
        }
        default: {
            printf("Unknown Instruction %d\n", instr);
            break;
        }
    }
}

int main(int argc, char** argv) {
    if (argc <= 1) {
        printf("error: no input files\n");
        return -1;
    }

    char *filename = argv[1];

    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("error: could not read file `%s`\n", filename);
        return -1;
    }

    instructions = malloc(sizeof(*instructions) * instruction_space);

    int num;
    int i = 0;
    while (fscanf(file, "%d", &num) > 0) {
        instructions[i] = num;
        printf("%d\n", instructions[i]);
        i++;
        if (i >= instruction_space) {
            instruction_space *= 2;
            instructions = realloc(instructions, sizeof(*instructions) * instruction_space);
        }
    }
    
    instruction_count = i;

    fclose(file);

    SP = -1;

    while (running && IP < instruction_count) {
        eval(FETCH);
        if(!is_jmp){
            IP = IP + 1;
        }
    }

    free(instructions);

    return 0;
}