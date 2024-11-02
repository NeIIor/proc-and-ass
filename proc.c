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
    size_t i = 0;
    type a, b;

    while (i < Proc->size_code) {  
        if (Proc->code[i] != CMD_ADD         && Proc->code[i] != CMD_SQRT        &&
            Proc->code[i] != CMD_SUB         && Proc->code[i] != CMD_POW         && 
            Proc->code[i] != CMD_DIV         && Proc->code[i] != CMD_MUL         &&
            Proc->code[i] != CMD_DIV_REM     && Proc->code[i] != CMD_BIT_ADD     && 
            Proc->code[i] != CMD_BIT_MUL     && Proc->code[i] != CMD_BIT_SHIFT_L && 
            Proc->code[i] != CMD_BIT_SHIFT_R)  {  
            switch (Proc->code[i++]) {

                case CMD_HLT:
                    return 0;
                    break;
                case CMD_PUSH:

                    switch (Proc->code[i++]) { 
                                                        //3 if-a vmesto switch-ей
                        case ARG_CONST:
                            stackPush (&Proc->Stk, Proc->code[i++]);
                            break;
                        case ARG_REG:
                            if (Proc->code[i] < 0 || Proc->code[i] > NUM_REGS) {
                                PRINT_ERROR (stderr, "Invalid register\n");
                                return 1;
                            }
                            stackPush (&Proc->Stk, Proc->regs[Proc->code[i++]]);
                            break;
                        case ARG_CONST | ARG_REG:
                            a = Proc->code[i++];

                            if (Proc->code[i] < 0 || Proc->code[i] > NUM_REGS) {
                                PRINT_ERROR (stderr, "Invalid register\n");
                                return 1;
                            }

                            b = Proc->regs[Proc->code[i++]];
                            stackPush (&Proc->Stk, a + b);
                            break;
                        case ARG_RAM | ARG_CONST: 

                            if (Proc->code[i] < 0 || Proc->code[i] > SIZE_RAM - 1) {
                                PRINT_ERROR (stderr, "Invalid RAM address\n");
                                return 1;
                            }

                            stackPush(&Proc->Stk, Proc->ram[Proc->code[i++]]);
                            break;
                        case ARG_RAM | ARG_REG:

                            if (Proc->code[i] < 0 || Proc->code[i] > NUM_REGS) {
                                PRINT_ERROR (stderr, "Invalid register\n");
                                return 1;
                            }

                            if (Proc->regs[Proc->code[i++]] < 0 || 
                                Proc->regs[Proc->code[i++]] > SIZE_RAM - 1) {
                                PRINT_ERROR (stderr, "Invalid RAM address\n");
                                return 1;
                            }

                            stackPush(&Proc->Stk, Proc->ram[Proc->regs[Proc->code[i++]]]);
                            break;
                        case ARG_RAM | ARG_REG | ARG_CONST: 

                            a = Proc->code[i++];
                            b = Proc->regs[Proc->code[i++]];

                            if (Proc->code[i] < 0 || Proc->code[i] > NUM_REGS) {
                                PRINT_ERROR (stderr, "Invalid register\n");
                                return 1;
                            }

                            if (a + b < 0 || a + b > SIZE_RAM - 1) {
                                PRINT_ERROR (stderr, "Invalid RAM address\n");
                                return 1;
                            }

                            stackPush(&Proc->Stk, Proc->ram[a + b]);
                            break;
                        default: 
                            PRINT_ERROR (stderr, "Syntax error in push\n");
                            return 1;
                            break;
                    }
                    break;
                case CMD_OUT:
                    printf(SPECIFICATOR "\n", stackPop (&Proc->Stk));
                    break;
                case CMD_JUMP:
                    i = Proc->code[i];
                    break;
                case CMD_IN:
                    scanf(SPECIFICATOR, &a);
                    stackPush (&Proc->Stk, a);
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
                case CMD_JUMP_EQ:

                    a = stackPop (&Proc->Stk);
                    b = stackPop (&Proc->Stk);   

                    if (b == a) {
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
                case CMD_PUT_C:
                    switch (Proc->code[i++]) {
                        case ARG_RAM | ARG_CONST: 

                            if (Proc->code[i] < 0 || Proc->code[i] > SIZE_RAM - 1) {
                                PRINT_ERROR (stderr, "Invalid RAM address\n");
                                return 1;
                            }

                            printf("%c", Proc->ram[Proc->code[i++]]);
                            break;
                        case ARG_RAM | ARG_REG:

                            if (Proc->code[i] < 0 || Proc->code[i] > NUM_REGS) {
                                PRINT_ERROR (stderr, "Invalid register\n");
                                return 1;
                            }

                            if (a + b < 0 || a + b > SIZE_RAM - 1) {
                                PRINT_ERROR (stderr, "Invalid RAM address\n");
                                return 1;
                            }

                            printf("%c", Proc->ram[Proc->regs[Proc->code[i++]]]);
                            break;
                        case ARG_RAM | ARG_REG | ARG_CONST: 

                            a = Proc->code[i++];
                            b = Proc->regs[Proc->code[i++]];

                            if (Proc->code[i] < 0 || Proc->code[i] > NUM_REGS) {
                                PRINT_ERROR (stderr, "Invalid register\n");
                                return 1;
                            }

                            if (a + b < 0 || a + b > SIZE_RAM - 1) {
                                PRINT_ERROR (stderr, "Invalid RAM address\n");
                                return 1;
                            }

                            printf("%c", Proc->ram[a + b]);
                            break;
                        default: 
                            PRINT_ERROR (stderr, "Syntax error in putc\n");
                            return 1;
                            break;
                    }
                    break;
                case CMD_POP:

                    switch (Proc->code[i++]) {

                        case ARG_CONST:
                            stackPop(&Proc->Stk);
                            break;
                        case ARG_REG:
                            a = stackPop (&Proc->Stk);
                            Proc->regs[Proc->code[i++]] = a;
                            break;
                        case ARG_REG | ARG_RAM:
                            a = stackPop (&Proc->Stk);
                            Proc->ram[Proc->regs[Proc->code[i++]]] = a;
                            break;
                        case ARG_CONST | ARG_RAM:
                            a = stackPop (&Proc->Stk);
                            Proc->ram[Proc->code[i++]] = a;
                            break;
                        case ARG_CONST | ARG_REG | ARG_RAM:
                            a = stackPop (&Proc->Stk);
                            Proc->ram[Proc->code[i++] + Proc->regs[Proc->code[i++]]] = a;
                            break;
                        default:
                            PRINT_ERROR (stderr, "Syntax error in pop\n");
                            break;
                    }
                    break;
            }
        } else if (Proc->code[i] == CMD_ADD         || Proc->code[i] == CMD_SQRT        || 
                   Proc->code[i] == CMD_SUB         || Proc->code[i] == CMD_POW         || 
                   Proc->code[i] == CMD_DIV         || Proc->code[i] == CMD_MUL         ||
                   Proc->code[i] == CMD_DIV_REM     || Proc->code[i] == CMD_BIT_ADD     || 
                   Proc->code[i] == CMD_BIT_MUL     || Proc->code[i] == CMD_BIT_SHIFT_L || 
                   Proc->code[i] == CMD_BIT_SHIFT_R){

            a = stackPop (&Proc->Stk);
            b = stackPop (&Proc->Stk);

            switch (Proc->code[i++]) {

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
}

void procInit  (proc_t* Proc) {
    Proc->regs = (type*) calloc (NUM_REGS , sizeof(type));
    Proc->ram =  (type*) calloc ( SIZE_RAM, sizeof(type));

    if (!Proc->regs || !Proc->ram) {
        PRINT_ERROR (stderr, "Unlucky allocating memory\n");
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
