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

## Assembly Language Syntax
The assembler processes a custom assembly language with the following key components:
- **Instructions**: `MOV`, `ADD`, `SUB`, `CMP`, `JMP`, `BNE`, `INC`, `DEC`, `STOP`
- **Addressing Modes**: Immediate (`#5`), Direct (`VAR`), Register (`@r2`)
- **Directives**: `.data`, `.string`, `.extern`, `.entry`
- **Macros**: Defined with `mcro` and `endmcro`

### Example Assembly Program
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
