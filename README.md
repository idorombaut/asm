# Assembler for a Custom Assembly Language

## Project Overview
This project is an assembler for a custom assembly language designed for an imaginary computer model. The assembler translates assembly source code into machine code, which can then be executed on the simulated hardware.

The assembler follows a **two-pass translation process**:
1. **First Pass:** Scans the assembly source code to identify symbols (labels, variables, and macros) and assigns addresses to them.
2. **Second Pass:** Replaces labels with actual addresses and translates instructions into machine code.

The project also includes an error-checking mechanism to detect syntax errors, invalid labels, incorrect operand usage, and other issues.

## Features
- Supports **symbol tables** to manage labels and variables.
- Implements **macro expansion** for reusable code blocks.
- Recognizes different **addressing modes** (immediate, direct, register-based).
- Generates **binary machine code** compatible with the custom CPU model.
- Detects **syntax errors** and provides detailed error messages.
- Handles **external references** and **entry points** for linking multiple files.

## Assembler Workflow

### First Pass (Symbol Table Construction)
- Reads each line of the assembly code.
- Identifies labels and stores their memory locations in a **symbol table**.
- Expands **macros** into their corresponding instructions.
- Keeps track of instruction and data memory allocation.

### Second Pass (Code Generation)
- Converts each instruction into **machine code** based on its opcode.
- Resolves labels by replacing them with their addresses from the **symbol table**.
- Writes the translated machine code to an **output file**.

### Error Checking
- Detects undefined labels.
- Reports syntax errors (e.g., missing operands, unknown instructions).
- Validates addressing modes for each instruction.
- Ensures proper macro usage and expansion.

## Installation & Compilation

### Prerequisites
- **GCC Compiler**
- **Make** (for automated compilation)
- **Linux OS** (Ubuntu recommended)

### Building the Assembler
To compile the project, open a terminal in the project directory and run:
```
make
```
This will generate an executable named `asm`.

## Usage
After preparing assembly files with the `.as` extension, open a terminal and run the assembler by passing the file names without extensions as arguments.

### Example Usage
For assembly files `x.as`, `y.as`, and `hello.as`, run:
```
./asm x y hello
```

The assembler will generate output files with the same filenames but with the following extensions:
- `.ob`: Object file (machine code)
- `.ent`: Entries file (symbols marked with `.entry`)
- `.ext`: Externals file (symbols marked with `.extern`)

## Hardware Specification

### CPU
- **Registers**: 9 total  
  **8 general-purpose registers**: r0 to r7  
  **1 program status word (PSW) register**
- **Register size**: 12 bits (1 word)

### Memory
- **RAM size**: 1024 words
- **Word size**: 12 bits
- **Stack**
- **Arithmetic**: Signed 2's complement (no floating-point support)

## Instruction Encoding

### Instruction Format (First Word)
Each instruction is stored as **one to three words** in memory. The first word is structured as follows:

|         11           9         |  8  5  |           4             2           |  1   0  |
| ------------------------------ | ------ | ----------------------------------- | ------- |
| Source Operand Addressing Mode | opcode | Destination Operand Addressing Mode | A, R, E |

### Base-64 Encoding
Each 12-bit word is represented using two characters from the base-64 encoding table ([Base-64](https://en.wikipedia.org/wiki/Base64)).

## Assembly Language Syntax
The assembler processes a custom assembly language with the following components:

### Instructions

| Opcode (decimal) | Command |
| ---------------- | ------- |
| 0                | `mov`   |
| 1                | `cmp`   |
| 2                | `add`   |
| 3                | `sub`   |
| 4                | `not`   |
| 5                | `clr`   |
| 6                | `lea`   |
| 7                | `inc`   |
| 8                | `dec`   |
| 9                | `jmp`   |
| 10               | `bne`   |
| 11               | `red`   |
| 12               | `prn`   |
| 13               | `jsr`   |
| 14               | `rts`   |
| 15               | `stop`  |

### Addressing Modes
- **Immediate Mode**: `#5` (uses a constant value)
- **Direct Mode**: `VAR` (references a memory location)
- **Register Mode**: `@r2` (uses a CPU register)

### Directives
Assembly directives control **memory allocation and linking**.

1. `.data`
   Allocates space in the **data section** for integers.  
   **Example**:
   ```
   LABEL1: .data 7, -56, 4, 9
   ```
   > Stores the values 7, -56, 4, 9 in memory.
2. `.string`
   Stores a string as **ASCII characters** in memory, followed by a null terminator (`\0`).  
   **Example**:
   ```
   STRING1: .string "hello"
   ```
   > Stores: h, e, l, l, o, \0
3. `.entry`
   Declares a label that **can be accessed by other files**.  
   **Example** (`file1.as`):
   ```
   .entry HELLO
   HELLO: add #1, r1
   ```
4. `.extern`
   Declares a label that is **defined in another file**.  
   **Example** (`file2.as`):
   ```
   .extern HELLO
   ```
   > Matches the .entry HELLO in file1.as.

### Macros
Macros allow defining reusable blocks of code:
```
mcro SUM
    ADD @r1, @r2
    MOV A, @r3
endmcro
```
To use the macro, just call it by name:
```
SUM
```

## Example Assembly Program
```
MAIN:       MOV @r3, LENGTH
LOOP:       JMP L1
            PRN -5
            BNE LOOP
            SUB @r1, @r4
            BNE END
L1:         INC K
            BNE LOOP
END:        STOP

STR:        .string "hello"
LENGTH:     .data 6, -9, 15
K:          .data 22
```
