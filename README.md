
# Instruction Set

The following table is an example of an instruction set:

| Instruction | Explanation |
|-------------|-------------|
| **Opcode Operand** | |
| `LDM #n` | Immediate addressing. Load the number `n` to ACC |
| `LDD <address>` | Direct addressing. Load the contents of the location at the given address to ACC |
| `LDI <address>` | Indirect addressing. The address to be used is at the given address. Load the contents of this second address to ACC |
| `LDX <address>` | Indexed addressing. Form the address from `<address>` + the contents of the index register. Copy the contents of this calculated address to ACC |
| `LDR #n` | Immediate addressing. Load the number `n` to IX |
| `MOV <register>` | Move the contents of the accumulator to the given register (IX) |
| `STO <address>` | Store the contents of ACC at the given address |
| `ADD <address>` | Add the contents of the given address to the ACC |
| `ADD #n/Bn/&n` | Add the number `n` to the ACC |
| `SUB <address>` | Subtract the contents of the given address from the ACC |
| `SUB #n/Bn/&n` | Subtract the number `n` from the ACC |
| `INC <register>` | Add 1 to the contents of the register (ACC or IX) |
| `DEC <register>` | Subtract 1 from the contents of the register (ACC or IX) |
| `JMP <address>` | Jump to the given address |
| `CMP <address>` | Compare the contents of ACC with the contents of `<address>` |
| `CMP #n` | Compare the contents of ACC with number `n` |
| `CMI <address>` | Indirect addressing. The address to be used is at the given address. Compare the contents of ACC with the contents of this second address |
| `JPE <address>` | Following a compare instruction, jump to `<address>` if the compare was True |
| `JPN <address>` | Following a compare instruction, jump to `<address>` if the compare was False |
| `IN` | Key in a character and store its ASCII value in ACC |
| `OUT` | Output to the screen the character whose ASCII value is stored in ACC |
| `END` | Return control to the operating system |

---

## Notes

- All questions will assume there is **only one general purpose register available** (Accumulator).
- `ACC` denotes the **Accumulator**.
- `IX` denotes the **Index Register**.
- `<address>` can be an **absolute or symbolic address**.
- `#` denotes a **denary number**, e.g. `#123`
- `B` denotes a **binary number**, e.g. `B01001010`
- `&` denotes a **hexadecimal number**, e.g. `&4A`

---

## Bitwise Operations

| Instruction | Explanation |
|-------------|-------------|
| `AND #n / Bn / &n` | Bitwise AND operation of the contents of ACC with the operand |
| `AND <address>` | Bitwise AND operation of the contents of ACC with the contents of `<address>` |
| `XOR #n / Bn / &n` | Bitwise XOR operation of the contents of ACC with the operand |
| `XOR <address>` | Bitwise XOR operation of the contents of ACC with the contents of `<address>` |
| `OR #n / Bn / &n` | Bitwise OR operation of the contents of ACC with the operand |
| `OR <address>` | Bitwise OR operation of the contents of ACC with the contents of `<address>` |

---

## Shift Operations

| Instruction | Explanation |
|-------------|-------------|
| `LSL #n` | Bits in ACC are shifted logically `n` places to the left. Zeros are introduced on the right-hand end |
| `LSR #n` | Bits in ACC are shifted logically `n` places to the right. Zeros are introduced on the left-hand end |

---

## Labels

| Syntax | Explanation |
|--------|-------------|
| `<label>: <opcode> <operand>` | Labels an instruction |
| `<label>: <data>` | Gives a symbolic address `<label>` to the memory location with contents `<data>` |


# Current Syntax

```c
INST(LDM, #10);
L(x); INST(DECA, #0);
INST(ISP, #0);
INST(CMPI, b1010);
INST(JPN, x);
INST(END, #0);
```

