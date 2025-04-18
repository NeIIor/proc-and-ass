#ifndef PROC_H__
#define PROC_H__
#include "stack.h"
#include "gen.h"
#include "colors.h"
#define NUM_REGS 4
#define MAX_CHAR 127

typedef struct proc {
    type* regs;
    stack_t Stk;
    type* code;
    size_t size_code;
    size_t num_cmd;
    type* ram;
    size_t ip;
} proc_t;

void procInputCmd (proc_t* Proc, FILE* proc);
int  procRunCmd   (proc_t* Proc);
void procInit     (proc_t* Proc);
void procDtor     (proc_t* Proc);
type procArgPush  (proc_t* Proc);
type procArgPop   (proc_t* Proc);


#endif //PROC_H__