#include "comp.h"
#include "colors.h"

int main() {
    comp_t Comp ={};
    initComp(&Comp);
    label_t* Labels = initLabels();
    if (passComp (&Comp, Labels)) {
        return 1;
    }
    dtorLabels(Labels);
    return 0;
}

int passComp (comp_t* Comp, label_t* Labels) {
    assert(Comp);
    assert(Labels);

    for (int i = 0; i < NUM_COMP_PASSES; i++) {
        if (compRunCmd(Comp, Labels)) {
            return 1;
        }
        Comp->ip = 0;
    }                        
}

label_t* initLabels () {
    label_t* Labels = (label_t*) calloc (MAX_NUM_LABELS, sizeof(label_t));

    for (int i = 0; i < MAX_NUM_LABELS; i++) {
        Labels[i].add = INVALID_ADDRESS;

        for (int j = 0; j < MAX_LABEL_NAME; j++) {
            Labels[i].name[j] = '\0';
        }
    }
    return Labels;
}

void dtorLabels (label_t* Labels) {
    assert(Labels);

    for (int i = 0; i < MAX_NUM_LABELS; i++) {
        Labels[i].add = INVALID_ADDRESS;             
    }
    free (Labels);
}

void dumpLabels (const label_t* Labels) {
    assert(Labels);

    for (int i = 0; i < MAX_NUM_LABELS; i++) {
        printf("Index: %d\tAddress: %u\tName: %s\n", i, Labels[i].add, Labels[i].name);
    }
}

size_t findLabel (const label_t* Labels, const char* str) {
    assert(Labels);
    assert(str);

    char* buf = (char*) calloc (MAX_LABEL_NAME + 1, sizeof(char));
    sscanf(str, "%[^:]", buf);        

    for (int i = 0; i < MAX_NUM_LABELS; i++) {
        if (strcmp (buf, Labels[i].name) == 0) {
            return Labels[i].add;                                     
        }
    }
    return INVALID_ADDRESS;
}

bool pushLabel (label_t* Labels, const char* str, const size_t size, const size_t ip) { 
    assert(Labels);
    assert(str);     

    for (int i = 0; i < MAX_NUM_LABELS; i++) {
        if (Labels[i].add == INVALID_ADDRESS) {
            Labels[i].add = ip;
            memcpy(Labels[i].name, str, size); 
            return PUSH_SUCCESSFULLY;
        }
    }
    return PUSH_FAILED;
}

void initComp (comp_t* Comp) {
    Comp->code = (type*) calloc (SIZE_CODE, sizeof(type));
    Comp->ip = 0;
}

enum regs compFindReg (const char* str) { //const char const *str?
    assert(str);
    if (!strcmp (str, "ax")) {
        return AX;
    } else if (!strcmp (str, "bx")) {
        return BX;
    } else if (!strcmp (str, "cx")) {
        return CX;
    } else if (!strcmp (str, "dx")) {
        return DX;
    }
    return RESTRICTED_AREA;
}

int compRunCmd (comp_t* Comp, label_t* Labels) {
    FILE* cmd = fopen ("cmd.txt", "r");
    FILE* proc_cmd = fopen ("proc_cmd.txt", "wb");

    type num;
    long long val = 0;
    char str[SIZE_CMD] = {0};
    char buf[MAX_LABEL_NAME] = {0};

    if (!str || !buf) {
        printf("Unluck with allocating\n");
        return 1;
    } 

    while (fscanf(cmd, "%s", str) != -1) { 
        if (strchr (str, ':')) {
            val = (size_t) strchr (str, ':');
            size_t size = val - (size_t) str;

            if (!pushLabel(Labels, str, size, Comp->ip)) {
                PRINT_ERROR (stderr, "Sorry, too many labels in one code(\n");
                return 1;
            }
        } 
        
        else if (!strcmp (str, "push")) {

            fscanf (cmd, "%s", buf);
            Comp->code[Comp->ip++] = CMD_PUSH;

            if (compFindReg(buf)) {

                Comp->code[Comp->ip++] = ARG_REG;
                Comp->code[Comp->ip++] = compFindReg(buf);

                if (Comp->code[Comp->ip - 1] == RESTRICTED_AREA) {
                    PRINT_ERROR (stderr, "Invalid name of register\n");
                    return 1;
                }

            } else if ((isdigit(buf[0]) || buf[0] == '-') && !strchr(buf, '+')) {

                int a = atoi(buf);
                Comp->code[Comp->ip++] = ARG_CONST;
                Comp->code[Comp->ip++] = a;

            } else if (isdigit(buf[0]) && strchr(buf, '+')) {

                char buf1[SIZE_CMD];
                char buf2[SIZE_CMD];
                int p;

                sscanf (buf, "%[^+]%n", buf1, &p);
                sscanf (&(buf[p + 1]), "%[abcdx]", buf2); 

                Comp->code[Comp->ip++] = ARG_CONST | ARG_REG;
                Comp->code[Comp->ip++] = atoi(buf1);
                Comp->code[Comp->ip++] = compFindReg(buf2);

                if (!Comp->code[Comp->ip-1]) {
                    PRINT_ERROR(stderr, "Invalid register\n");
                    return 1;
                }

            } else if (buf[0] = '[' && strchr(buf, ']')) {

                sscanf(&(buf[1]), "%[^]]", buf);

                if (compFindReg(buf)) {

                    Comp->code[Comp->ip++] = ARG_REG | ARG_RAM;
                    Comp->code[Comp->ip++] = compFindReg(buf);

                    if (Comp->code[Comp->ip - 1] == RESTRICTED_AREA) {
                        PRINT_ERROR (stderr, "Invalid name of register\n");
                        return 1;
                    }

                } else if ((isdigit(buf[0]) || buf[0] == '-') && !strchr(buf, '+')) {

                    int a = atoi(buf);
                    Comp->code[Comp->ip++] = ARG_CONST | ARG_RAM;
                    Comp->code[Comp->ip++] = a;

                    if (a < 0 || a > SIZE_RAM - 1) {
                        PRINT_ERROR (stderr, "Invalid ARG_RAM address\n"); 
                        return 1;      //?
                    }

                } else if (isdigit(buf[0]) && strchr(buf, '+')) {  

                    char buf1[SIZE_CMD];
                    char buf2[SIZE_CMD];
                    int p;

                    sscanf (buf, "%[^+]%n", buf1, &p);
                    sscanf (&(buf[p + 1]), "%[abcdx]", buf2);

                    Comp->code[Comp->ip++] = ARG_CONST | ARG_REG | ARG_RAM;
                    Comp->code[Comp->ip++] = atoi(buf1);
                    Comp->code[Comp->ip++] = compFindReg(buf2);

                    if (Comp->code[Comp->ip - 1] == RESTRICTED_AREA) {
                        PRINT_ERROR (stderr, "Invalid name of register\n");
                        return 1;
                    }

                    if (Comp->code[Comp->ip - 2] < 0 || Comp->code[Comp->ip - 2] > SIZE_RAM - 1) {
                        PRINT_ERROR (stderr, "Invalid ARG_RAM address\n"); 
                        return 1;      //?
                    }

                } 
            } else {
                    PRINT_ERROR(stderr, "Syntax error in push\n");
                    return 1;
                }
        }
         
        else if (!strcmp(str, "add")) {   
              Comp->code[Comp->ip++] = CMD_ADD;
        } 
        
        else if (!strcmp(str, "sub")) {
              Comp->code[Comp->ip++] = CMD_SUB;
        } 
        
        else if (!strcmp(str, "mul")) {
              Comp->code[Comp->ip++] = CMD_MUL;
        } 
        
        else if (!strcmp(str, "div")) {
              Comp->code[Comp->ip++] = CMD_DIV;
        } 

        else if (!strcmp(str, "pow")) {
              Comp->code[Comp->ip++] = CMD_POW;
        } 

        else if (!strcmp(str, "sqrt")) {
              Comp->code[Comp->ip++] = CMD_SQRT;
        } 
        
        else if (!strcmp(str, "out")) {
              Comp->code[Comp->ip++] = CMD_OUT;
        } 
        
        else if (!strcmp(str, "pop")) {

            fscanf (cmd, "%s", buf);
            Comp->code[Comp->ip++] = CMD_POP;

            if (compFindReg(buf)) {

                Comp->code[Comp->ip++] = ARG_REG;
                Comp->code[Comp->ip++] = compFindReg(buf);

                if (Comp->code[Comp->ip - 1] == RESTRICTED_AREA) {
                        PRINT_ERROR (stderr, "Invalid name of register\n");
                        return 1;
                    }

            } else if (buf[0] = '[' && strchr(buf, ']')) {

                sscanf(&(buf[1]), "%[^]]", buf);

                if (compFindReg(buf)) {

                    Comp->code[Comp->ip++] = ARG_REG | ARG_RAM;
                    Comp->code[Comp->ip++] = compFindReg(buf);

                    if (Comp->code[Comp->ip - 1] == RESTRICTED_AREA) {
                        PRINT_ERROR (stderr, "Invalid name of register\n");
                        return 1;
                    }

                } else if ((isdigit(buf[0]) || buf[0] == '-') && !strchr(buf, '+')) {

                    int a = atoi(buf);
                    Comp->code[Comp->ip++] = ARG_CONST | ARG_RAM;
                    Comp->code[Comp->ip++] = a;

                    if (a < 0 || a > SIZE_RAM - 1) {
                        PRINT_ERROR (stderr, "Invalid ARG_RAM address\n");
                        return 1;      
                    }

                } else if (isdigit(buf[0]) && strchr(buf, '+')) { 

                    char buf1[SIZE_CMD];
                    char buf2[SIZE_CMD];
                    int p;

                    sscanf (buf, "%[^+]%n", buf1, &p);
                    sscanf (&(buf[p + 1]), "%[abcdx]", buf2);

                    Comp->code[Comp->ip++] = ARG_CONST | ARG_REG | ARG_RAM;
                    Comp->code[Comp->ip++] = atoi(buf1);
                    Comp->code[Comp->ip++] = compFindReg(buf2);

                    if (Comp->code[Comp->ip - 1] == RESTRICTED_AREA) {
                        PRINT_ERROR (stderr, "Invalid name of register\n");
                        return 1;
                    }

                    if (Comp->code[Comp->ip - 2] < 0 || Comp->code[Comp->ip - 2] > SIZE_RAM - 1) {
                        PRINT_ERROR (stderr, "Invalid ARG_RAM address\n");
                        return 1;
                    }

                } 
            } else {
                Comp->code[Comp->ip++] = ARG_CONST;
            }
        } 
        
        else if (!strcmp(str, "jmp")) {

            fscanf (cmd, "%s", buf);
            Comp->code[Comp->ip++] = CMD_JUMP;

            if (!strchr(buf, ':')) {       

                val = atoi(buf);
                Comp->code[Comp->ip++] = val;
            } else {

                if ((size_t) strchr(buf, ':') - (size_t) buf > MAX_LABEL_NAME) {

                    PRINT_ERROR (stderr, "Label name too long\n");
                    return 1;
                }
                long long add = findLabel (Labels, buf);

                if (add != INVALID_ADDRESS) {
                    Comp->code[Comp->ip++] = add;
                } else {
                    return 1;
                }
            }
        } 

        else if (!strcmp(str, "ja")) {

            fscanf (cmd, "%s", buf);
            Comp->code[Comp->ip++] = CMD_JUMP_A;

            if (!strchr(buf, ':')) { 

                val = atoi(buf);
                Comp->code[Comp->ip++] = val;

            } else {

                if ((size_t) strchr(buf, ':') - (size_t) buf > MAX_LABEL_NAME) {

                    PRINT_ERROR (stderr, "Label name too long\n");
                    return 1;
                }
                long long add = findLabel (Labels, buf);

                if (add != INVALID_ADDRESS) {
                    Comp->code[Comp->ip++] = add;
                } else {
                    return 1;
                }
            }      
        } 

        else if (!strcmp(str, "jb")) {

            fscanf (cmd, "%s", buf);
            Comp->code[Comp->ip++] = CMD_JUMP_B;
            
            if (!strchr(buf, ':')) {   

                val = atoi(buf);
                Comp->code[Comp->ip++] = val;
            } else {

                if ((size_t) strchr(buf, ':') - (size_t) buf > MAX_LABEL_NAME) {

                    PRINT_ERROR (stderr, "Label name too long\n");
                    return 1;
                }
                long long add = findLabel (Labels, buf);

                if (add != INVALID_ADDRESS) {
                    Comp->code[Comp->ip++] = add;
                } else {
                    return 1;
                }
            }
        } 

        else if (!strcmp(str, "je")) {

            fscanf (cmd, "%s", buf);
            Comp->code[Comp->ip++] = CMD_JUMP_EQ;
            
            if (!strchr(buf, ':')) { 

                val = atoi(buf);
                Comp->code[Comp->ip++] = val;

            } else {

                if ((size_t) strchr(buf, ':') - (size_t) buf > MAX_LABEL_NAME) {

                    PRINT_ERROR (stderr, "Label name too long\n");
                    return 1;
                }
                long long add = findLabel (Labels, buf);

                if (add != INVALID_ADDRESS) {
                    Comp->code[Comp->ip++] = add;          
                } else {
                    return 1;
                }
            }
        } 
        
        else if (!strcmp(str, "hlt")) {
             Comp->code[Comp->ip++] = CMD_HLT;
        } 

        else if (!strcmp(str, "call")) {

            fscanf (cmd, "%s", buf);
            Comp->code[Comp->ip++] = CMD_CALL;

            if (!strchr(buf, ':')) {       

                val = atoi(buf);
                Comp->code[Comp->ip++] = val;
            } else {

                if ((size_t) strchr(buf, ':') - (size_t) buf > MAX_LABEL_NAME) {

                    PRINT_ERROR (stderr, "Label name too long\n");
                    return 1;
                }
                long long add = findLabel (Labels, buf);

                if (add != INVALID_ADDRESS) {
                    Comp->code[Comp->ip++] = add;
                } else {
                    return 1;
                }
            }
        }

        else if (!strcmp(str, "ret")) {
            Comp->code[Comp->ip++] = CMD_RET;
        }

        else if (!strcmp(str, "in")) {
            Comp->code[Comp->ip++] = CMD_IN;
        }

        else if (!strcmp(str, "putc")) {
            fscanf (cmd, "%s", buf);
            Comp->code[Comp->ip++] = CMD_PUT_C;

            if (buf[0] = '[' && strchr(buf, ']')) {

                sscanf(&(buf[1]), "%[^]]", buf);

                if (compFindReg(buf)) {

                    Comp->code[Comp->ip++] = ARG_REG | ARG_RAM;
                    Comp->code[Comp->ip++] = compFindReg(buf);

                    if (Comp->code[Comp->ip - 1] == RESTRICTED_AREA) {
                        PRINT_ERROR (stderr, "Invalid name of register\n");
                        return 1;
                    }

                } else if ((isdigit(buf[0]) || buf[0] == '-') && !strchr(buf, '+')) {

                    int a = atoi(buf);
                    Comp->code[Comp->ip++] = ARG_CONST | ARG_RAM;
                    Comp->code[Comp->ip++] = a;

                    if (a < 0 || a > SIZE_RAM - 1) {
                        PRINT_ERROR (stderr, "Invalid ARG_RAM address\n");   
                        return 1;     //?
                    }

                } else if (isdigit(buf[0]) && strchr(buf, '+')) { 

                    char buf1[SIZE_CMD];
                    char buf2[SIZE_CMD];
                    int p;

                    sscanf (buf, "%[^+]%n", buf1, &p);
                    sscanf (&(buf[p + 1]), "%[abcdx]", buf2);

                    Comp->code[Comp->ip++] = ARG_CONST | ARG_REG | ARG_RAM;
                    Comp->code[Comp->ip++] = atoi(buf1);
                    Comp->code[Comp->ip++] = compFindReg(buf2);

                    if (Comp->code[Comp->ip - 1] == RESTRICTED_AREA) {
                        PRINT_ERROR (stderr, "Invalid name of register\n");
                        return 1;
                    }

                    if (Comp->code[Comp->ip - 2] < 0 || Comp->code[Comp->ip - 2] > SIZE_RAM - 1) {
                        PRINT_ERROR (stderr, "Invalid ARG_RAM address\n");
                        return 1;
                    }

                } 
            } else {
                PRINT_ERROR (stderr, "Putc needs address. This adrress is invalid\n");
                return 1;
            }
        } else {
            PRINT_ERROR (stderr, "Syntax error\n");
            return 1;
        }
    } 

    for (size_t i = 0; i < Comp->ip; i++) {
        PRINT_ERROR(stderr, SPECIFICATOR"\n", Comp->code[i]);
    }  

    head_bin_proc_t head = {0};
    head.sign = 'Egor';
    printf("%s\n", &(head.sign));
    head.vers = 1;
    head.size = Comp->ip;

    fwrite (&head, sizeof(head), 1, proc_cmd);
    fwrite (Comp->code, sizeof(type), Comp->ip,  proc_cmd); 
    
    fclose (cmd);
    fclose (proc_cmd);
}
