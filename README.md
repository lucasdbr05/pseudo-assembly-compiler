# Pseudo Assembly Compiler

Project of Pseudo-Assembly compiler for the the Basic Software course at the University of Brasília.
A C++ preprocessor and compiler for a pseudo-assembly language that supports macros, labels, and various assembly-like operations.

## Supported Operations

The compiler supports the following operations:

| Operation | Opcode | Parameters | Description |
|-----------|--------|------------|-------------|
| ADD | 1 | 1 | Addition |
| SUB | 2 | 1 | Subtraction |
| MULT | 3 | 1 | Multiplication |
| DIV | 4 | 1 | Division |
| JMP | 5 | 1 | Unconditional jump |
| JMPN | 6 | 1 | Jump if negative |
| JMPP | 7 | 1 | Jump if positive |
| JMPZ | 8 | 1 | Jump if zero |
| COPY | 9 | 2 | Copy value |
| LOAD | 10 | 1 | Load value |
| STORE | 11 | 1 | Store value |
| INPUT | 12 | 1 | Input operation |
| OUTPUT | 13 | 1 | Output operation |
| STOP | 14 | 0 | Stop execution |

## Directives

- `SPACE` - Allocate space for a variable
- `CONST` - Define a constant value
- `MACRO` / `ENDMACRO` - Define macro blocks

## Building

### Prerequisites
- g++ compiler
- Make

### Compile the compiler:
```bash
make compiler
```

### Clean build artifacts:
```bash
make clear
```

## Usage

### Running the compiler:
```bash
./compiler <input_file.asm>
```

This will generate a preprocessed file with the `.pre` extension and the compiled files with the `.o1` and `.o2` extension.

### Example:
```bash
./compiler test.asm
```
This will create `test.pre`, `test.o1` and `test.o2` outputs.