#include "comp.h"
#include "colors.h"

int main() {
    comp_t Comp = {};
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
        if (compRunComp(Comp, Labels)) {
            return 1;
        }
        Comp->ip = 0;
    }       
    return 0;                 
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

    char buf[MAX_LABEL_NAME + 1] = {};
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
    Comp->cmds[ 0].name = "hlt";
    Comp->cmds[ 0].num  = CMD_HLT;
    Comp->cmds[ 0].type = NO_ARG;

    Comp->cmds[ 1].name = "push";
    Comp->cmds[ 1].num  = CMD_PUSH;
    Comp->cmds[ 1].type = PUSH;

    Comp->cmds[ 2].name = "add";
    Comp->cmds[ 2].num  = CMD_ADD;
    Comp->cmds[ 2].type = NO_ARG;

    Comp->cmds[ 3].name = "sub";
    Comp->cmds[ 3].num  = CMD_SUB;
    Comp->cmds[ 3].type = NO_ARG;

    Comp->cmds[ 4].name = "div";
    Comp->cmds[ 4].num  = CMD_DIV;
    Comp->cmds[ 4].type = NO_ARG;

    Comp->cmds[ 5].name = "mul";
    Comp->cmds[ 5].num  = CMD_MUL;
    Comp->cmds[ 5].type = NO_ARG;

    Comp->cmds[ 6].name = "pow";
    Comp->cmds[ 6].num  = CMD_POW;
    Comp->cmds[ 6].type = NO_ARG;

    Comp->cmds[ 7].name = "sqrt";
    Comp->cmds[ 7].num  = CMD_SQRT;
    Comp->cmds[ 7].type = NO_ARG;

    Comp->cmds[ 8].name = "div_r";
    Comp->cmds[ 8].num  = CMD_DIV_REM;
    Comp->cmds[ 8].type = NO_ARG;

    Comp->cmds[ 9].name = "b_mul";
    Comp->cmds[ 9].num  = CMD_BIT_MUL;
    Comp->cmds[ 9].type = NO_ARG;

    Comp->cmds[10].name = "b_add";
    Comp->cmds[10].num  = CMD_BIT_ADD;
    Comp->cmds[10].type = NO_ARG;

    Comp->cmds[11].name = "b_shift_l";
    Comp->cmds[11].num  = CMD_BIT_SHIFT_L;
    Comp->cmds[11].type = NO_ARG;

    Comp->cmds[12].name = "b_shift_r";
    Comp->cmds[12].num  = CMD_BIT_SHIFT_R;
    Comp->cmds[12].type = NO_ARG;

    Comp->cmds[13].name = "out";
    Comp->cmds[13].num  = CMD_OUT;
    Comp->cmds[13].type = NO_ARG;

    Comp->cmds[14].name = "j";
    Comp->cmds[14].num  = CMD_JUMP;
    Comp->cmds[14].type = JUMP;

    Comp->cmds[15].name = "ja";
    Comp->cmds[15].num  = CMD_JUMP_A;
    Comp->cmds[15].type = JUMP;

    Comp->cmds[16].name = "jb";
    Comp->cmds[16].num  = CMD_JUMP_B;
    Comp->cmds[16].type = JUMP;

    Comp->cmds[17].name = "je";
    Comp->cmds[17].num  = CMD_JUMP_EQ;
    Comp->cmds[17].type = JUMP;
    
    Comp->cmds[18].name = "pop";
    Comp->cmds[18].num  = CMD_POP;
    Comp->cmds[18].type = POP;

    Comp->cmds[19].name = "in";
    Comp->cmds[19].num  = CMD_IN;
    Comp->cmds[19].type = NO_ARG;

    Comp->cmds[20].name = "putc";
    Comp->cmds[20].num  = CMD_PUT_C;
    Comp->cmds[20].type = PUSH;

    Comp->cmds[21].name = "call";
    Comp->cmds[21].num  = CMD_CALL;
    Comp->cmds[21].type = JUMP;

    Comp->cmds[22].name = "ret";
    Comp->cmds[22].num  = CMD_RET;
    Comp->cmds[22].type = NO_ARG;
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

int compArgPush (comp_t* Comp, FILE* cmd) {
    char buf[MAX_LABEL_NAME] = {0};
    fscanf (cmd, "%s", buf);

    if (compFindReg(buf)) {

        Comp->code[Comp->ip++] = ARG_REG;
        Comp->code[Comp->ip++] = compFindReg(buf);

        if (Comp->code[Comp->ip - 1] == RESTRICTED_AREA) {
            PRINT_ERROR (stderr, "Invalid name of register\n");
            return 1;
        }

        return 0;

    } else if ((isdigit(buf[0]) || buf[0] == '-') && !strchr(buf, '+')) {

        int a = atoi(buf);
        Comp->code[Comp->ip++] = ARG_CONST;
        Comp->code[Comp->ip++] = a;

        return 0;

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

        return 0;

    } else if (buf[0] = '[' && strchr(buf, ']')) {

        sscanf(&(buf[1]), "%[^]]", buf);

        if (compFindReg(buf)) {

            Comp->code[Comp->ip++] = ARG_REG | ARG_RAM;
            Comp->code[Comp->ip++] = compFindReg(buf);

            if (Comp->code[Comp->ip - 1] == RESTRICTED_AREA) {
                PRINT_ERROR (stderr, "Invalid name of register\n");
                return 1;
            }

            return 0;

        } else if ((isdigit(buf[0]) || buf[0] == '-') && !strchr(buf, '+')) {

            int a = atoi(buf);
            Comp->code[Comp->ip++] = ARG_CONST | ARG_RAM;
            Comp->code[Comp->ip++] = a;

            if (a < 0 || a > SIZE_RAM - 1) {
                PRINT_ERROR (stderr, "Invalid ARG_RAM address\n"); 
                return 1;    
            }

            return 0;

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

            return 0;

        } 
    } else {
            PRINT_ERROR(stderr, "Syntax error\n");
            return 1;
    }
}

int compArgPop (comp_t* Comp, FILE* cmd) {
    char buf[MAX_LABEL_NAME] = {0};
    fscanf (cmd, "%s", buf);

    if (compFindReg(buf)) {

        Comp->code[Comp->ip++] = ARG_REG;
        Comp->code[Comp->ip++] = compFindReg(buf);

        if (Comp->code[Comp->ip - 1] == RESTRICTED_AREA) {
                PRINT_ERROR (stderr, "Invalid name of register\n");
                return 1;
            }
        
        return 0;

    } else if (buf[0] = '[' && strchr(buf, ']')) {

        sscanf(&(buf[1]), "%[^]]", buf);

        if (compFindReg(buf)) {

            Comp->code[Comp->ip++] = ARG_REG | ARG_RAM;
            Comp->code[Comp->ip++] = compFindReg(buf);

            if (Comp->code[Comp->ip - 1] == RESTRICTED_AREA) {
                PRINT_ERROR (stderr, "Invalid name of register\n");
                return 1;
            }

            return 0;

        } else if ((isdigit(buf[0]) || buf[0] == '-') && !strchr(buf, '+')) {

            int a = atoi(buf);
            Comp->code[Comp->ip++] = ARG_CONST | ARG_RAM;
            Comp->code[Comp->ip++] = a;

            if (a < 0 || a > SIZE_RAM - 1) {
                PRINT_ERROR (stderr, "Invalid ARG_RAM address\n");
                return 1;      
            }

            return 0;

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

            return 0;

        } 
    } else {
        PRINT_ERROR(stderr, "Care that wrong argument is identified like just pop from Stk\n");
        Comp->code[Comp->ip++] = ARG_CONST;
    }
}

int compArgJump (comp_t* Comp, label_t* Labels, FILE* cmd) {
    long long val = 0;
    char buf[MAX_LABEL_NAME] = {0};
    fscanf (cmd, "%s", buf);

    if (!strchr(buf, ':')) {       

        val = atoi(buf);
        Comp->code[Comp->ip++] = val;

        return 0;

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

        return 0;
    }
} 


int compRunComp (comp_t* Comp, label_t* Labels) {
    FILE* cmd = fopen ("cmd.txt", "r");
    FILE* proc_cmd = fopen ("proc_cmd.txt", "wb");

    type num;
    long long val = 0;
    char str[SIZE_CMD] = {0};

    while (fscanf(cmd, "%s", str) != -1) { 
        printf("%s\n", str);
        for (int i = 0; i < NUM_CMDS; i++) {
            if (!strcmp (str, Comp->cmds[i].name)) {
                Comp->code[Comp->ip++] = Comp->cmds[i].num;
                switch (Comp->cmds[i].type) {
                    case PUSH:
                        if (compArgPush (Comp        , cmd)) {
                            PRINT_ERROR (stderr, "Error in " "%s\n", Comp->cmds[i].name);
                            return 1;
                        }
                        break;
                    case POP:
                        if (compArgPop  (Comp        , cmd)) {
                            PRINT_ERROR (stderr, "Error in " "%s\n", Comp->cmds[i].name);
                            return 1;
                        }
                        break;
                    case JUMP:
                        if (compArgJump (Comp, Labels, cmd)) {
                            PRINT_ERROR (stderr, "Error in " "%s\n", Comp->cmds[i].name);
                            return 1;
                        }
                        break;
                }
            }
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

    return 0;
}
