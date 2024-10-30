#ifndef PROC_H__
#define PROC_H__
#include "stack.h"
#include "enum.h"
#include "colors.h"
#define SIZE_CODE 1000
#define NUM_REGS 4
#define SIZE_RAM 100

typedef struct proc {
    type* regs;
    stack_t Stk;
    type* code;
    size_t size_code;
    size_t num_cmd;
    type* ram;
} proc_t;

void procInputCmd (proc_t* Proc);
int  procRunCmd   (proc_t* Proc);
void procInit     (proc_t* Proc);
void procDtor     (proc_t* Proc);

#endif //PROC_H__