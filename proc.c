#include <stdio.h>
#include "stack.h"
#include "proc.h"

int main () {
    proc_t Proc = {};
    procInit     (&Proc);
    procInputCmd (&Proc);
    procRunCmd   (&Proc);
    procDtor     (&Proc);
    return 0;
}

void procInputCmd (proc_t* Proc) {
    FILE* proc = fopen ("proc_cmd.txt", "r");
    unsigned int i = 0;
    type val;
    while (fscanf(proc, "%d", &val) != -1) {
        Proc->num_cmd++;
        Proc->code[i++] = val;
        if (val == CMD_PUSH || val == CMD_JUMP_A || val == CMD_JUMP || val == CMD_JUMP_B ||
            val == CMD_POP){
            fscanf(proc, SPECIFICATOR, &val); 
            Proc->code[i++] = val;
        }
    }
    Proc->size_code = i;
    fclose (proc);
}

int procRunCmd (proc_t* Proc) {
    size_t i = 0;
    type a, b;
    while (i < Proc->size_code) {   
        //printf("%u\n", i);           
        switch (Proc->code[i++]) {
        case CMD_HLT:
            return 0;
        case CMD_PUSH:
            switch (Proc->code[i++]) {
                case CONST:
                    stackPush (&Proc->Stk, Proc->code[i++]);
                    break;
                case REG:
                    stackPush (&Proc->Stk, Proc->regs[Proc->code[i++]]);
                    break;
                case CONST | REG:
                    a = Proc->code[i++];
                    b = Proc->regs[Proc->code[i++]];
                    stackPush (&Proc->Stk, a + b);
                    break;
                case RAM | CONST: 
                    if (Proc->code[i] < 0 || Proc->code[i] > SIZE_RAM - 1) {
                        PRINT_ERROR (stderr, "Invalid RAM address");
                        return 1;
                    }
                    stackPush(&Proc->Stk, Proc->ram[Proc->code[i++]]);
                    break;
                case RAM | REG:
                    if (Proc->code[i] < 0 || Proc->code[i] > NUM_REGS) {
                        PRINT_ERROR (stderr, "Invalid register");
                        return 1;
                    }
                    stackPush(&Proc->Stk, Proc->regs[Proc->code[i++]]);
                    break;
                case RAM | REG | CONST: 
                    a = Proc->code[i++];
                    b = Proc->regs[Proc->code[i++]];

                    if (Proc->code[i] < 0 || Proc->code[i] > NUM_REGS) {
                        PRINT_ERROR (stderr, "Invalid register");
                        return 1;
                    }
                    if (a + b < 0 || a + b > SIZE_RAM - 1) {
                        PRINT_ERROR (stderr, "Invalid RAM address");
                    }
                    stackPush (&Proc->Stk, Proc->ram[a + b]);
            }
            break;
        case CMD_ADD:
            a = stackPop (&Proc->Stk); 
            b = stackPop (&Proc->Stk);
            stackPush (&Proc->Stk, a + b); 
            break;    
        case CMD_SUB:    
            a = stackPop (&Proc->Stk);
            b = stackPop (&Proc->Stk);
            stackPush (&Proc->Stk, b - a); 
            break;
        case CMD_DIV:
            a = stackPop (&Proc->Stk);  // % & | << >>
            b = stackPop (&Proc->Stk);
            stackPush (&Proc->Stk, b / a);  
            break;
        case CMD_MUL:
            a = stackPop (&Proc->Stk);
            b = stackPop (&Proc->Stk);
            stackPush (&Proc->Stk, a * b); 
            break;
        case CMD_POW: 
            a = stackPop (&Proc->Stk);
            b = stackPop (&Proc->Stk);
            stackPush (&Proc->Stk, (type) pow((double)b, a)); 
            break;
        case CMD_SQRT: 
            a = stackPop (&Proc->Stk);
            b = stackPop (&Proc->Stk);
            stackPush (&Proc->Stk, (type) pow((double) b, 1 / (double) a)); 
            break;
        case CMD_OUT:
            printf(SPECIFICATOR "\n", stackPop (&Proc->Stk));
            break;
        case CMD_JUMP:
            i = Proc->code[i];
            break;
        case CMD_JUMP_A:
            a = stackPop (&Proc->Stk);
            b = stackPop (&Proc->Stk);
            if (b > a) {
                i = Proc->code[i];
            } else {
                i++;
            }
            break;  
        case CMD_JUMP_B: 
            a = stackPop (&Proc->Stk);
            b = stackPop (&Proc->Stk);
            if (b < a) {
                i = Proc->code[i];
            } else {
                i++;
            }
            break;
        case CMD_POP:
            switch (Proc->code[i++]) {
                case CONST:
                    stackPop(&Proc->Stk);
                    break;
                case REG:
                    a = stackPop (&Proc->Stk);
                    Proc->regs[Proc->code[i++]] = a;
                    break;
                case REG | RAM:
                    a = stackPop (&Proc->Stk);
                    Proc->ram[Proc->regs[Proc->code[i++]]] = a;
                    break;
                case CONST | RAM:
                    a = stackPop (&Proc->Stk);
                    Proc->ram[Proc->code[i++]] = a;
                    break;
                case CONST | REG | RAM:
                    a = stackPop (&Proc->Stk);
                    Proc->ram[Proc->code[i++] + Proc->regs[Proc->code[i++]]] = a;
                default:
                    PRINT_ERROR (stderr, "Error in pop");
            }
            break;
        default: 
            PRINT_ERROR (stderr, "ERROR");
            break;
        }
    }
    PRINT_ERROR(stderr, "Forced stop of programm. No command hlt");
}

void procInit  (proc_t* Proc) {
    Proc->code = (type*) calloc (SIZE_CODE, sizeof(type));
    Proc->regs = (type*) calloc (NUM_REGS , sizeof(type));
    Proc->ram =  (type*) calloc ( SIZE_RAM, sizeof(type));
    if (!Proc->regs || !Proc->code || !Proc->ram) {
        PRINT_ERROR (stderr, "Unlucky allocating memory");
    }
    Proc->num_cmd = 0;
    Proc->size_code = 0;
    stackInit (&Proc->Stk);
}

void procDtor (proc_t* Proc) {
    free(Proc->code);
    free(Proc->ram);
    free(Proc->regs);
    Proc->size_code = 0;
    Proc->num_cmd = 0;
}
