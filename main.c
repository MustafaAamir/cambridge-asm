#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Virtual machine component ---  //
typedef uint64_t u64;
#define MEM_SIZE 1024 // Mem
#define MAX_LABELS MEM_SIZE

typedef struct {
  u64 ACC;              // Accumulator
  u64 IX;               // Index Register
  u64 PC;               // Program Counter
  u64 flag;             // Comparison Flag
  u64 memory[MEM_SIZE]; // Full memory snapshot
  char *labels[MAX_LABELS];
} VMState;

void init_vm(VMState *state) {
  state->ACC = 0;
  state->IX = 0;
  state->PC = 0;
  state->flag = 0;
  memset(state->memory, 0, sizeof(state->memory));
  memset(state->labels, NULL, sizeof(state->labels));
}

enum Opcode {
  LDM,
  LDD,
  LDI,
  LDX,
  LDR,
  MOVIX,
  STO,
  ADDI,
  ADDA,
  SUBI,
  SUBA,
  INCA,
  INCX,
  DECA,
  DECX,
  JMP,
  CMPA,
  CMPI,
  CMII,
  JPE,
  JPN,
  ANDI,
  ANDA,
  XORI,
  XORA,
  ORI,
  ORA,
  LSL,
  LSR,
  IN,
  OUT,
  END,
  ISP
};

void inspect_state(const VMState *state) {
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
    printf("\t| Mem[%04d] |  0x%016lx | %020lu | %s", i, state->memory[i],
           state->memory[i], (i == state->PC) ? "<-- PC" : "");
    if (state->labels[i] != NULL)
      printf("  %s", state->labels[i]);
    printf("\n");
  }
  printf("\t+--------------------------------------------------------+\n");
}
u64 parse_operand(const char *operand, char *labels[1024]) {
  char *endptr;
  u64 value;
  switch (*operand) {
	  case '#': value = strtoull(operand + 1, &endptr, 10); break;
	  case 'B': value = strtoull(operand + 1, &endptr, 2); break;
	  case '&': value = strtoull(operand + 1, &endptr, 16); break;
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
    case LDM:
      state->ACC = operand;
      break;
    case LDD:
      state->ACC = state->memory[operand];
      break;
    case LDI:
      state->ACC = state->memory[state->memory[operand]];
      break;
    case LDX:
      state->ACC = state->memory[operand + state->IX];
      break;
    case LDR:
      state->IX = operand;
      break;
    case MOVIX:
      state->IX = state->ACC;
      break;
    case STO:
      state->memory[operand] = state->ACC;
      break;
    case ADDI:
      state->ACC += operand;
      break; // Add imm
    case ADDA:
      state->ACC += state->memory[operand];
      break;
    case SUBI:
      state->ACC -= operand;
      break;
    case SUBA:
      state->ACC -= state->memory[operand];
      break;
    case INCA:
      state->ACC++;
      break;
    case INCX:
      state->IX++;
      break;
    case DECA:
      --(state->ACC);
      break;
    case DECX:
      state->IX--;
      break;
    case JMP:
      state->PC = operand;
      break;
    case CMPA:
      state->flag = (state->ACC == state->memory[operand]) ? 1 : 0;
      break; // Compare direct
    case CMPI:
      state->flag = (state->ACC == operand) ? 1 : 0;
      break; // Compare imm
    case CMII:
      state->flag =
          (state->ACC == state->memory[state->memory[operand]]) ? 1 : 0;
      break; // Compare indirect
    case JPE:
      if (state->flag == 1)
        state->PC = operand;
      break;
    case JPN:
      if (state->flag == 0)
        state->PC = operand;
      break;
    case ANDI:
      state->ACC &= operand;
      break; // AND imm
    case ANDA:
      state->ACC &= state->memory[operand];
      break; // AND addr
    case XORI:
      state->ACC ^= operand;
      break; // XOR imm
    case XORA:
      state->ACC ^= state->memory[operand];
      break; // XOR addr
    case ORI:
      state->ACC |= operand;
      break; // OR imm
    case ORA:
      state->ACC |= state->memory[operand];
      break; // OR addr
    case LSL:
      state->ACC = (u64)((u64)state->ACC << operand);
      break;
    case LSR:
      state->ACC = (u64)((u64)state->ACC >> operand);
      break;
    case IN: {
      char c;
      scanf(" %c", &c);
      state->ACC = (u64)c;
      break;
    }
    case OUT: {
      char c = (char)state->ACC;
      printf("%c", c);
      break;
    }
    case END:
      return;
    case ISP:
      inspect_state(state);
      break;
    default:
      printf("Unknown opcode %ld\n", opcode);
      break;
    }
  }
}

// --- Compilation unit ---- //
void compile_to_x86_64(const char *filename, u64 *memory, char **labels) {
  FILE *out = fopen(filename, "w");
  if (!out) {
    perror("Error opening output file");
    return;
  }
  // Standard x86_64 assembly boilerplate
  fprintf(out, "section .data\n");
  fprintf(out, "    format_int db \"%%ld\", 10, 0\n");
  fprintf(out, "    format_char db \"%%c\", 0\n");
  fprintf(out, "    memory: times 1024 dq 0  ; 1024 64-bit memory locations\n");
  fprintf(out, "    acc:    dq 0  ; Accumulator\n");
  fprintf(out, "    ix:     dq 0  ; Index Register\n");
  fprintf(out, "    flag:   dq 0  ; Comparison Flag\n");
  fprintf(out, "\n");

  fprintf(out, "section .text\n");
  fprintf(out, "    extern printf\n");
  fprintf(out, "    extern scanf\n");
  fprintf(out, "    global main\n\n");

  fprintf(out, "; Macro for printing integer (debug)\n");
  fprintf(out, "%%macro print_int 1\n");
  fprintf(out, "    mov rdi, format_int\n");
  fprintf(out, "    mov rsi, %%1\n");
  fprintf(out, "    xor rax, rax\n");
  fprintf(out, "    call printf\n");
  fprintf(out, "%%endmacro\n");

  fprintf(out, "; Macro for printing character\n");
  fprintf(out, "%%macro print_char 1\n");
  fprintf(out, "    mov rdi, format_char\n");
  fprintf(out, "    mov rsi, %%1\n");
  fprintf(out, "    xor rax, rax\n");
  fprintf(out, "    call printf\n");
  fprintf(out, "%%endmacro\n");

  fprintf(out, "main:\n");

  for (int pc = 0; pc < MEM_SIZE; pc += 2) {
    if (labels[pc] != NULL)
      fprintf(out, "%s:\n", labels[pc]);
    u64 opcode = memory[pc];
    u64 operand = memory[pc + 1];
    // rbx is acc
    switch (opcode) {
      /*
	   left to implement
      enum Opcode {
        INCX, DECX, CMPA
        CMII, ANDI, ANDA, XORI,
        XORA, ORI, ORA, LSL, LSR, IN, ISP
      };
                                      */
    case LDM:
      fprintf(out, "    mov rbx, %lu\n", operand);
      break;
    case DECA:
      fprintf(out, "    dec rbx\n");
      break;
    case INCA:
      fprintf(out, "    inc rbx\n");
      break;
    case JMP:
      fprintf(out, "    jmp %s\n", labels[operand]);
      break;
    case CMPI:
      fprintf(out, "    cmp rbx, %lu\n", operand);
      break;
    case JPE:
      fprintf(out, "    je %s\n", labels[operand]);
      break;
    case JPN:
      fprintf(out, "    jne %s\n", labels[operand]);
      break;
    case ISP:
      fprintf(out, "    ; State inspection placeholder\n");
      break;
    case OUT:
      fprintf(out, "    print_int rbx\n");
      break;
    // need to implement other opcodes.
    case LDD:
      fprintf(out, "    mov rbx, [memory + %lu*8]\n", operand);
      break;
    case LDI: // Indirect load to ACC
      fprintf(out, "    mov rcx, [memory + %lu*8]\n", operand);
      fprintf(out, "    mov rbx, [memory + rcx*8]\n");
      break;
    case LDX: // Indexed load to ACC
      fprintf(out, "    mov rax, [ix]\n");
      fprintf(out, "    mov rbx, [memory + (%lu + rax)*8]\n", operand);
      break;
    case LDR: // Load immediate to IX
      fprintf(out, "    mov qword [ix], %lu\n", operand);
      break;
    case MOVIX:
      fprintf(out, "    mov [ix], rbx\n");
      break;
    case STO:
      fprintf(out, "    mov [memory + %lu*8], rbx\n", operand);
      break;
    case ADDI: // Add immeto ACC
      fprintf(out, "    add rbx, %lu\n", operand);
      break;
    case SUBI:
      fprintf(out, "    sub rbx, %lu\n", operand);
      break;
    case ADDA: // Add from memory to ACC
      fprintf(out, "    mov rax, [memory + %lu*8]\n", operand);
      fprintf(out, "    add rbx, rax\n");
      break;
    case SUBA: // Add from memory to ACC
      fprintf(out, "    mov rax, [memory + %lu*8]\n", operand);
      fprintf(out, "    sub rbx, rax\n");
      break;
    case END:
      fprintf(out, "\n    ; Exit program\n");
      fprintf(out, "    mov rax, 60\n");
      fprintf(out, "    xor rdi, rdi\n");
      fprintf(out, "    syscall\n");
      break;

    default:
      fprintf(out, "    ; Unsupported opcode %lu\n", opcode);
      break;
    }
  }
  fclose(out);
}
void generate_build_script(const char *file_name) {
  FILE *file = fopen(file_name, "w");
  fprintf(file, "#!/usr/bin/bash\n");
  fprintf(file, "nasm -f elf64 output.asm\n");
  fprintf(file, "gcc output.o -o program\n");
  fprintf(file, "rm output.o");
}

int main(int argc, char **argv) {
  u64 pos = 0, lable_count = 0;
  VMState state;
  init_vm(&state);

#define INST(opcode, operand)                                                  \
  state.memory[pos++] = opcode;                                                \
  state.memory[pos++] = parse_operand(#operand, state.labels)
#define L(label_name) state.labels[pos] = #label_name;

	  INST(LDM, B1010);
	  INST(STO, &FF);
	  INST(LDM, &FF);
	  INST(STO, &AF);
	  INST(LDI, &AF);
L(x); INST(DECA, #0);
	  INST(OUT, #0);
	  INST(ISP, #0);
	  INST(CMPI, b0);
	  INST(JPN, x);
	  INST(END, #0);

  if (argc != 2) {
    fprintf(stderr,
            "usage:\n\t-v for virtual machine\n\t-c for compilation\n\n");
    exit(0);
  }
  if (strcmp(argv[1], "-v") == 0)
    run_vm(&state);
  else if (strcmp(argv[1], "-c") == 0) {
    printf("Compiling program...\n");
    compile_to_x86_64("output.asm", state.memory, state.labels);
    printf("Generating build script...\n");
    generate_build_script("build.sh");
    printf("Run 'bash build.sh', ./program\n");
  }
#undef INST
#undef L
  return 0;
}
