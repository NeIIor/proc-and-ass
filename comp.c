#include "comp.h"
#include "colors.h"

int main() {
    comp_t Comp ={};
    initComp(&Comp);
    label_t* Label = initLabel();
    passComp (&Comp, Label);
    dtorLabel(Label);
    return 0;
}

void passComp (comp_t* Comp, label_t* Label) {
    assert(Comp);
    assert(Label);

    for (int i = 0; i < NUM_COMP_PASSES; i++) {
        compRunCmd(Comp, Label);
        Comp->ip = 0;
    }                        
}

label_t* initLabel () {
    label_t* Label = (label_t*) calloc (SIZE_LABEL, sizeof(label_t));
    for (int i = 0; i < SIZE_LABEL; i++) {
        Label[i].add = INVALID_ADDRESS;
        for (int j = 0; j < MAX_LABEL_NAME; j++) {
            Label[i].name[j] = '\0';
        }
    }
    return Label;
}

void dtorLabel (label_t* Label) {
    assert(Label);

    for (int i = 0; i < SIZE_LABEL; i++) {
        Label[i].add = INVALID_ADDRESS;             
    }
    free (Label);
}

void dumpLabel (const label_t* Label) {
    assert(Label);

    for (int i = 0; i < SIZE_LABEL; i++) {
        printf("Index: %d\tAddress: %u\tName: %s\n", i, Label[i].add, Label[i].name);
    }
}

size_t findLabel (const label_t* Label, const char* str) {
    assert(Label);
    assert(str);

    char* buf = (char*) calloc (MAX_LABEL_NAME + 1, sizeof(char));
    sscanf(str, "%[^:]", buf);              
    for (int i = 0; i < SIZE_LABEL; i++) {
        if (strcmp (buf, Label[i].name) == 0) {
            return Label[i].add;                                     
        }
    }
    return INVALID_ADDRESS;
}

bool pushLabel (label_t* Label, const char* str, const size_t size, const size_t ip) { 
    assert(Label);
    assert(str);     

    for (int i = 0; i < SIZE_LABEL; i++) {
        if (Label[i].add == INVALID_ADDRESS) {
            Label[i].add = ip;
            memcpy(Label[i].name, str, size);            //head my proc_cmd sabcription + version ...
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

int compRunCmd (comp_t* Comp, label_t* Label) {
    FILE* cmd = fopen ("cmd.txt", "r");
    FILE* proc_cmd = fopen ("proc_cmd.txt", "wb");
    type num;
    long long val = 0;
    char* str = (char*) calloc (SIZE_CMD, sizeof(char));
    char* buf = (char*) calloc (MAX_LABEL_NAME + 1, sizeof(char));

    if (!str || !buf) {
        printf("Unluck with allocating");
    } 

    while (fscanf(cmd, "%s", str) != -1) { 
        if (strchr (str, ':')) {
            val = (size_t) strchr (str, ':');
            size_t size = val - (size_t) str;

            if (!pushLabel(Label, str, size, Comp->ip)) {
                PRINT_ERROR (stderr, "Sorry, too many labels in one code(");
            }
        } 
        
        else if (!strcmp (str, "push")) {
            fscanf (cmd, "%s", buf);

            Comp->code[Comp->ip++] = CMD_PUSH;
            if (compFindReg(buf)) {

                Comp->code[Comp->ip++] = REG;
                Comp->code[Comp->ip++] = compFindReg(buf);

            } else if ((isdigit(buf[0]) || buf[0] == '-') && !strchr(buf, '+')) {
                int a = atoi(buf);

                Comp->code[Comp->ip++] = CONST;
                Comp->code[Comp->ip++] = a;

            } else if (isdigit(buf[0]) && strchr(buf, '+')) {
                char buf1[SIZE_CMD];
                char buf2[SIZE_CMD];
                int p;

                sscanf (buf, "%[^+]%n", buf1, &p);
                sscanf (&(buf[p + 1]), "%[abcdx]", buf2); 

                Comp->code[Comp->ip++] = CONST | REG;
                Comp->code[Comp->ip++] = atoi(buf1);
                Comp->code[Comp->ip++] = compFindReg(buf2);

                if (Comp->code[Comp->ip-1]) {
                    PRINT_ERROR(stderr, "Invalid register");
                    break;
                }

            } else if (buf[0] = '[' && strchr(buf, ']')) {
                sscanf(&(buf[1]), "%[^]]", buf);
                if (compFindReg(buf)) {
                    Comp->code[Comp->ip++] = REG | RAM;
                    Comp->code[Comp->ip++] = compFindReg(buf);


                } else if ((isdigit(buf[0]) || buf[0] == '-') && !strchr(buf, '+')) {
                    int a = atoi(buf);

                    Comp->code[Comp->ip++] = CONST | RAM;
                    Comp->code[Comp->ip++] = a;

                    if (a < 0) {
                        PRINT_ERROR (stderr, "Invalid RAM address");        //?
                    }

                } else if (isdigit(buf[0]) && strchr(buf, '+')) {  
                    char buf1[SIZE_CMD];
                    char buf2[SIZE_CMD];
                    int p;

                    sscanf (buf, "%[^+]%n", buf1, &p);
                    sscanf (&(buf[p + 1]), "%[abcdx]", buf2);

                    Comp->code[Comp->ip++] = CONST | REG | RAM;
                    Comp->code[Comp->ip++] = atoi(buf1);
                    Comp->code[Comp->ip++] = compFindReg(buf2);

                    if (Comp->code[Comp->ip - 1] == RESTRICTED_AREA) {
                        PRINT_ERROR (stderr, "Invalid name of register");
                        break;
                    }

                } 
            } else {
                    PRINT_ERROR(stderr, "Syntax error in push");
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
                Comp->code[Comp->ip++] = REG;
                Comp->code[Comp->ip++] = compFindReg(buf);

            } else if (buf[0] = '[' && strchr(buf, ']')) {
                sscanf(&(buf[1]), "%[^]]", buf);
                if (compFindReg(buf)) {
                    Comp->code[Comp->ip++] = REG | RAM;
                    Comp->code[Comp->ip++] = compFindReg(buf);


                } else if ((isdigit(buf[0]) || buf[0] == '-') && !strchr(buf, '+')) {
                    int a = atoi(buf);

                    Comp->code[Comp->ip++] = CONST | RAM;
                    Comp->code[Comp->ip++] = a;

                    if (a < 0) {
                        PRINT_ERROR (stderr, "Invalid RAM address");        //?
                    }

                } else if (isdigit(buf[0]) && strchr(buf, '+')) {  
                    char buf1[SIZE_CMD];
                    char buf2[SIZE_CMD];
                    int p;

                    sscanf (buf, "%[^+]%n", buf1, &p);
                    sscanf (&(buf[p + 1]), "%[abcdx]", buf2);

                    Comp->code[Comp->ip++] = CONST | REG | RAM;
                    Comp->code[Comp->ip++] = atoi(buf1);
                    Comp->code[Comp->ip++] = compFindReg(buf2);

                    if (Comp->code[Comp->ip - 1] == RESTRICTED_AREA) {
                        PRINT_ERROR (stderr, "Invalid name of register");
                        break;
                    }

                } 
            } else {
                Comp->code[Comp->ip++] = CONST;
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
                    PRINT_ERROR (stderr, "Label name too long");
                    break;
                }

                long long add = findLabel (Label, buf);
                if (add != INVALID_ADDRESS) {
                    Comp->code[Comp->ip++] = add;
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
                    PRINT_ERROR (stderr, "Label name too long");
                    break;
                }
                
                long long add = findLabel (Label, buf);
                if (add != INVALID_ADDRESS) {
                    Comp->code[Comp->ip++] = add;
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
                    PRINT_ERROR (stderr, "Label name too long");
                    break;
                }
                
                long long add = findLabel (Label, buf);
                if (add != INVALID_ADDRESS) {
                    Comp->code[Comp->ip++] = add;
                }
            }
        } 

        else if (!strcmp(str, "je")) {
            fscanf (cmd, "%s", buf);
            Comp->code[Comp->ip++] = CMD_JUMP_B;
            
            if (!strchr(buf, ':')) {                     
                val = atoi(buf);
                Comp->code[Comp->ip++] = val;
            } else {
                if ((size_t) strchr(buf, ':') - (size_t) buf > MAX_LABEL_NAME) {
                    PRINT_ERROR (stderr, "Label name too long");
                    break;
                }
                
                long long add = findLabel (Label, buf);
                if (add != INVALID_ADDRESS) {
                    Comp->code[Comp->ip++] = add;           //push symbol
                }
            }
        } 
        
        else if (!strcmp(str, "hlt")) {
             Comp->code[Comp->ip++] = CMD_HLT;
        } 

        else if (!strcmp(str, "in")) {
            Comp->code[Comp->ip++] = CMD_IN;
        }

        else {
            PRINT_ERROR (stderr, "Syntax error\n");
            break;
        }
    }   
    for (size_t i = 0; i < Comp->ip; i++) {
        PRINT_ERROR(stderr, SPECIFICATOR"\n", Comp->code[i]);
    }  
    head_t head = {0};
    head.sign = 'Egor';
    printf("%s\n", &(head.sign));
    head.vers = 1;
    head.size = Comp->ip;
    fwrite (&head, sizeof(head), 1, proc_cmd);
    fwrite (Comp->code, sizeof(type), Comp->ip,  proc_cmd); 
    free (str);
    free(buf);
    fclose (cmd);
    fclose (proc_cmd);
}
