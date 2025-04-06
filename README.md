# Stack-Based Processor Implementation

## Technical Description

A virtual stack machine processor implemented in C that executes compiled bytecode programs. The processor features a minimal instruction set architecture with focus on efficient stack operations.

## Architecture Overview

### Core Components
- **Instruction Decoder**: Interprets bytecode instructions
- **Stack Unit**: Manages the data stack (32-bit integers)
- **Memory Unit**: Handles program memory (64KB address space)
- **Control Unit**: Manages program flow and execution

### Memory Model
- **Program Memory**: Stores bytecode instructions
- **Data Stack**: LIFO structure for operands (256 levels max)
- **Registers**: Includes program counter (PC) and stack pointer (SP)

## Instruction Set Architecture

### Arithmetic Operations
ADD - Add top two stack elements
SUB - Subtract top two elements
MUL - Multiply top two elements
DIV - Integer division
MOD - Modulo operation
POW - Exponentiation
SQRT - Integer square root

### Stack Manipulation
PUSH <val> - Push immediate value
POP - Remove top element
DUP - Duplicate top element
SWAP - Swap top two elements

### Program Control
JMP <addr> - Absolute jump
JZ <addr> - Jump if zero
CALL <addr>- Subroutine call
RET - Return from subroutine
HLT - Terminate execution

### I/O Operations
IN - Read integer input
OUT - Output integer value
PRINT - Output ASCII character

## Building and Execution

### Compilation
```bash
make
```
Example Program
; Calculate 5 + (3 * 2)
PUSH 3
PUSH 2
MUL
PUSH 5
ADD
OUT
HLT
## Configuration

```
#define STACK_DEPTH 256  // Maximum stack size
#define MEM_SIZE 65536   // Addressable memory 
#define DEBUG 0          // Debug output
```

## Limitations
Integer-only operations

No memory protection

Basic error handling
