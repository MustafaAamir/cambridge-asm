#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>
#include <stdlib.h>

#define MEM_SIZE 1024 // Memory size (arbitrary, can be adjusted)
typedef uint64_t u64;

typedef struct {
    u64 ACC;    // Accumulator
    u64 IX;     // Index Register
    u64 PC;     // Program Counter
    u64 flag;   // Comparison Flag
    u64 memory[MEM_SIZE];  // Full memory snapshot
	char *labels[MEM_SIZE];
} VMState;


void init_vm(VMState *state) {
	state->ACC = 0;  
    state->IX = 0;    
    state->PC = 0;    
    state->flag = 0;  
    memset(state->memory, 0, sizeof(state->memory));
    memset(state->labels, 0, sizeof(state->labels));
}

enum Opcode {
  LDM,LDD,LDI,LDX,LDR,
  MOV_IX,STO,ADDI,ADDA,
  SUBI,SUBA,INCA,INCX,
  DECA,DECX,JMP,CMPA,
  CMPI,CMII,JPE,JPN,ANDI,
  ANDA,XOR_I,XOR_A,ORI,ORA,
  LSL,LSR,IN,OUT,END,ISP
};

void inspect_state(const VMState* state) {
    printf("\n\t+--------------------------------------------------+\n");
	printf("\t|                         VM State                 |\n");
    printf("\t|--------------------------------------------------|\n");
    printf("\t| ACC |  0x%016lx | %020lu |\n", state->ACC, state->ACC);
    printf("\t|  IX |  0x%016lx | %020lu |\n", state->IX, state->IX);
    printf("\t|  PC |  0x%016lx | %020lu |\n", state->PC, state->PC);
    printf("\t| Flg |  0x%016lx | %020lu |\n", state->flag, state->flag);
    printf("\t+--------------------------------------------------+\n\n");
    printf("\t+--------------------------------------------------------+\n");
	printf("\t|                     Memory around PC:                  |\n");
    printf("\t+--------------------------------------------------------+\n");
    int start = (state->PC > 5) ? state->PC - 5 : 0;
    int end = (state->PC + 5 < MEM_SIZE) ? state->PC + 5 : MEM_SIZE;
    for (int i = start; i < end; i++) {
        printf("\t| Mem[%04d] |  0x%016lx | %020lu | %s",
               i, 
               state->memory[i], 
               state->memory[i],
               (i == state->PC) ? "<-- PC" : ""
        );
		if (state->labels[i] != NULL)
			printf("  %s", state->labels[i]);
		printf("\n");
    }
    printf("\t+--------------------------------------------------------+\n");
}
#define MAX_LABELS 1024
u64 parse_operand(const char *operand, char *labels[1024]) {
	char *endptr;
	u64 value;
	switch (*operand) {
	case '#': value = strtoull(operand + 1, &endptr, 10); break;
	case 'b': value = strtoull(operand + 1, &endptr, 2); break;
	case 'x': value = strtoull(operand + 1, &endptr, 16); break;
	case 'o': value = strtoull(operand + 1, &endptr, 8); break;
	}
	for (u64 i = 0; i < MAX_LABELS; i++)
		if (labels[i] != NULL && strcmp(labels[i], operand) == 0)
			return i;
	if (*endptr != '\0') {
		fprintf(stderr, "Invalid operand format: %s\n", operand);
		exit(1);
	}
	return value;
}



void run_vm(VMState *state) {
  while (1) {
    if (state->PC >= MEM_SIZE || state->PC < 0) {
      printf("PC out of bounds\n");
      break;
    }
    u64 opcode = state->memory[state->PC++];
    u64 operand = state->memory[state->PC++];
    switch (opcode) {
		case LDM: state->ACC = operand; break;
		case LDD: state->ACC = state->memory[operand]; break;
		case LDI: state->ACC = state->memory[state->memory[operand]]; break;
		case LDX: state->ACC = state->memory[operand + state->IX]; break;
		case LDR: state->IX = operand; break;
		case MOV_IX: state->IX = state->ACC; break;
		case STO: state->memory[operand] = state->ACC; break;
		case ADDI: state->ACC += operand; break; // Add immediate
		case ADDA: state->ACC += state->memory[operand]; break; // Add from address
		case SUBI: state->ACC -= operand; break; // Subtract immediate
		case SUBA: state->ACC -= state->memory[operand]; break; // Subtract from address
		case INCA: state->ACC++; break; // Increment state->ACC
		case INCX: state->IX++; break; // Increment IX
		case DECA: --(state->ACC); break; // Decrement state->ACC
		case DECX: state->IX--; break; // Decrement IX
		case JMP: state->PC = operand; break; // Jump
		case CMPA: state->flag = (state->ACC == state->memory[operand]) ? 1 : 0; break; // Compare direct
		case CMPI: state->flag = (state->ACC == operand) ? 1 : 0; break; // Compare immediate
		case CMII: state->flag = (state->ACC == state->memory[state->memory[operand]]) ? 1 : 0; break; // Compare indirect
		case JPE: if (state->flag == 1) state->PC = operand; break; // Jump if equal
		case JPN: if (state->flag == 0) state->PC = operand; break; // Jump if not equal
		case ANDI: state->ACC &= operand; break; // AND immediate
		case ANDA: state->ACC &= state->memory[operand]; break; // AND address
		case XOR_I: state->ACC ^= operand; break; // XOR immediate
		case XOR_A: state->ACC ^= state->memory[operand]; break; // XOR address
		case ORI: state->ACC |= operand; break; // OR immediate
		case ORA: state->ACC |= state->memory[operand]; break; // OR address
		case LSL: state->ACC = (u64)((u64)state->ACC << operand); break; // Logical shift left
		case LSR: state->ACC = (u64)((u64)state->ACC >> operand); break; // Logical shift right
		case IN: { char c; scanf(" %c", &c); state->ACC = (u64)c; break; }
		case OUT: { char c = (char)state->ACC; printf("%c", c); break; }
		case END: return; // End execution
	 	case ISP: inspect_state(state); break;
		default: printf("Unknown opcode %ld\n", opcode); break; // Error handling
    }
  }
}

int main() {
  u64 pos = 0, lable_count = 0;
  VMState state;
  init_vm(&state);

#define INST(opcode, operand) \
	state.memory[pos++] = opcode;\
	state.memory[pos++] = parse_operand(#operand, state.labels)
#define L(label_name) \
    state.labels[pos] = #label_name;


        INST(LDM, #10);
  L(x); INST(DECA, #0);
	    INST(ISP, #0);
		INST(CMPI, b1010);
	   	INST(JPN, x);
        INST(END, #0);

	run_vm(&state);
#undef INST
#undef L
  return 0;
}
