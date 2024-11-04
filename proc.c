#include <stdio.h>
#include "stack.h"
#include "proc.h"

int main () {
    FILE* proc = fopen ("proc_cmd.txt", "rb");
    proc_t Proc = {};
    procInit       (&Proc);
    procInputCmd   (&Proc, proc);
    if (procRunCmd (&Proc)) {
        return 1;
    }
    procDtor       (&Proc);
    fclose          (proc);
    return 0;
}

type  procArgPush (proc_t* Proc) {
    type arg_type = Proc->code[Proc->ip++];
    type res = 0;
    if (arg_type & 1) res += Proc->code[Proc->ip++];
    if (arg_type & 2) res += Proc->regs[Proc->code[Proc->ip++]];
    if (arg_type & 4) res  = Proc->ram[res];
    return res;
}

type procArgPop (proc_t* Proc) {
    type arg_type = Proc->code[Proc->ip++];
    type res = 0;
    switch (arg_type) {
        case ARG_CONST:
            stackPop(&Proc->Stk);
            break;
        case ARG_REG:
            res = stackPop (&Proc->Stk);
            Proc->regs[Proc->code[Proc->ip++]] = res;
            break;
        case ARG_REG | ARG_RAM:
            res = stackPop (&Proc->Stk);
            Proc->ram[Proc->regs[Proc->code[Proc->ip++]]] = res;
            break;
        case ARG_CONST | ARG_RAM:
            res = stackPop (&Proc->Stk);
            Proc->ram[Proc->code[Proc->ip++]] = res;
            break;
        case ARG_CONST | ARG_REG | ARG_RAM:
            res = stackPop (&Proc->Stk);
            Proc->ram[Proc->code[Proc->ip++] + Proc->regs[Proc->code[Proc->ip++]]] = res;
            break;
        default:
            PRINT_ERROR (stderr, "Syntax error in pop\n");
            break;
    }
    return res;
}

/*type  getArgPop  (proc_t* Proc) {
    type arg_type = Proc->code[Proc->ip++];
    type res = 0;
    type val = stackPop (&Proc->Stk);
    if (arg_type & ARG_CONST) 
}*/

void procInputCmd (proc_t* Proc, FILE* proc) {
    head_bin_proc_t head;

    fread(&head, sizeof(uint64_t), HEAD, proc);
    Proc->code = (type*) calloc (head.size, sizeof(type));
    if (!Proc->code) {
        PRINT_ERROR (stderr, "Unlucky allocating memory\n");
    }
    fread(Proc->code, sizeof(type), head.size, proc);

    Proc->size_code = head.size;
}

int procRunCmd (proc_t* Proc) {
    type a, b;

    while (Proc->ip < Proc->size_code) {  
        if ((Proc->code[Proc->ip] < CMD_ARITHMETIC_START || Proc->code[Proc->ip] > CMD_ARITHMETIC_END) && (Proc->code[Proc->ip] < CMD_ARITHM_JMP_START || Proc->code[Proc->ip] > CMD_ARITHM_JUMP_END))  {  
            switch (Proc->code[Proc->ip++]) {

                case CMD_HLT:
                    return 0;
                    break;
                case CMD_PUSH:
                    a = procArgPush(Proc);
                    if (!a) {
                        PRINT_ERROR(stderr, "Error in push");
                        return 1;
                    } else {
                        stackPush (&Proc->Stk, a);
                    }
                    break;
                case CMD_OUT:
                    printf(SPECIFICATOR "\n", stackPop (&Proc->Stk));
                    break;
                case CMD_JUMP:
                    Proc->ip = Proc->code[Proc->ip];
                    break;
                case CMD_IN:
                    scanf(SPECIFICATOR, &a);
                    stackPush (&Proc->Stk, a);
                    break;
                case CMD_PUT_C:
                    a = procArgPush(Proc);
                    if (!a) {
                        PRINT_ERROR(stderr, "Error in putc");
                        return 1;
                    } else {
                        printf("%c", a);
                    }
                    break;
                    break;
                case CMD_POP:
                    a = procArgPop (Proc);
                    if (!a) {
                        PRINT_ERROR (stderr, "Error in pop");
                        return 1;
                    }

                    break;
                case CMD_CALL:
                    stackPush (&Proc->Stk, Proc->ip + 1);
                    Proc->ip = Proc->code[Proc->ip];
                    break;
                case CMD_RET:
                    a = stackPop (&Proc->Stk);
                    Proc->ip = a;
                    break;

            }
        } else {

            a = stackPop (&Proc->Stk);
            b = stackPop (&Proc->Stk);

            switch (Proc->code[Proc->ip++]) {

                case CMD_JUMP_A:

                    if (b > a) {
                        Proc->ip = Proc->code[Proc->ip];
                    } else {
                        Proc->ip++;
                    }
                    break;  
                case CMD_JUMP_EQ:

                    if (b == a) {
                        Proc->ip = Proc->code[Proc->ip];
                    } else {
                        Proc->ip++;
                    }
                    break; 
                case CMD_JUMP_B: 

                    if (b < a) {
                        Proc->ip = Proc->code[Proc->ip];
                    } else {
                        Proc->ip++;
                    }
                    break;

                case CMD_ADD:
                    stackPush (&Proc->Stk, a + b); 
                    break;    
                case CMD_SUB:    
                    stackPush (&Proc->Stk, b - a); 
                    break;
                case CMD_DIV:
                    stackPush (&Proc->Stk, b / a);  
                    break;
                case CMD_MUL:
                    stackPush (&Proc->Stk, a * b); 
                    break;
                case CMD_POW: 
                    stackPush (&Proc->Stk, (type) pow((double)b, a)); 
                    break;
                case CMD_SQRT: 
                    stackPush (&Proc->Stk, (type) pow((double) b, 1 / (double) a)); 
                    break;
                case CMD_DIV_REM:
                    stackPush (&Proc->Stk, b % a); 
                    break;
                case CMD_BIT_ADD:
                    stackPush (&Proc->Stk, b | a); 
                    break;
                case CMD_BIT_MUL:
                    stackPush (&Proc->Stk, b & a); 
                    break;
                case CMD_BIT_SHIFT_L:
                    stackPush (&Proc->Stk, b << a); 
                    break;
                case CMD_BIT_SHIFT_R:
                    stackPush (&Proc->Stk, b >> a); 
                    break;
            }
        }
    }
    PRINT_ERROR(stderr, "Forced stop of programm. No command hlt\n");
    return 1;
}

void procInit  (proc_t* Proc) {
    Proc->regs = (type*) calloc (NUM_REGS , sizeof(type));
    Proc->ram =  (type*) calloc ( SIZE_RAM, sizeof(type));

    if (!Proc->regs || !Proc->ram) {
        PRINT_ERROR (stderr, "Unlucky allocating memory\n");
    }
    
    Proc->num_cmd = 0;
    Proc->size_code = 0;
    Proc->ip = 0;
    stackInit (&Proc->Stk);
}

void procDtor (proc_t* Proc) {
    free(Proc->code);
    free(Proc->ram);
    free(Proc->regs);

    Proc->size_code = 0;
    Proc->num_cmd = 0;
}
