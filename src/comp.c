#include "comp.h"
#include "colors.h"

cmd_t cmds[] = {{"hlt",            CMD_HLT,               NO_ARG}, 
                {"push",           CMD_PUSH,              PUSH  },
                {"add",            CMD_ADD,               NO_ARG},
                {"sub",            CMD_SUB,               NO_ARG},
                {"div",            CMD_DIV,               NO_ARG},
                {"mul",            CMD_MUL,               NO_ARG},
                {"pow",            CMD_POW,               NO_ARG},
                {"sqrt",           CMD_SQRT,              NO_ARG},
                {"div_r",          CMD_DIV_REM,           NO_ARG},
                {"b_mul",          CMD_BIT_MUL,           NO_ARG},
                {"b_add",          CMD_BIT_ADD,           NO_ARG},
                {"b_shift_l",      CMD_BIT_SHIFT_L,       NO_ARG},
                {"b_shift_r",      CMD_BIT_SHIFT_R,       NO_ARG},
                {"out",            CMD_OUT,               NO_ARG},
                {"j",              CMD_JUMP,              JUMP},
                {"ja",             CMD_JUMP_A,            JUMP},
                {"jb",             CMD_JUMP_B,            JUMP},
                {"je",             CMD_JUMP_EQ,           JUMP},
                {"pop",            CMD_POP,               POP},
                {"in",             CMD_IN,                NO_ARG},
                {"putc",           CMD_PUT_C,             PUSH},
                {"call",           CMD_CALL,              JUMP},
                {"ret",            CMD_RET,               NO_ARG}
};

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
            if (!strcmp (str, cmds[i].name)) {
                Comp->code[Comp->ip++] = cmds[i].num;
                switch (cmds[i].type) {
                    case PUSH:
                        if (compArgPush (Comp        , cmd)) {
                            PRINT_ERROR (stderr, "Error in " "%s\n", cmds[i].name);
                            return 1;
                        }
                        break;
                    case POP:
                        if (compArgPop  (Comp        , cmd)) {
                            PRINT_ERROR (stderr, "Error in " "%s\n", cmds[i].name);
                            return 1;
                        }
                        break;
                    case JUMP:
                        if (compArgJump (Comp, Labels, cmd)) {
                            PRINT_ERROR (stderr, "Error in " "%s\n", cmds[i].name);
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
    head.vers = 8;
    head.size = Comp->ip;

    fwrite (&head, sizeof(head), 1, proc_cmd);
    fwrite (Comp->code, sizeof(type), Comp->ip,  proc_cmd); 
    
    fclose (cmd);
    fclose (proc_cmd);

    return 0;
}
