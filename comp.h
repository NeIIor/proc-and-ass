#ifndef PROC_H__
#define PROC_H__
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

typedef long int type;
#define SPECIFICATOR "%ld"
#define SIZE_CMD 10
#define MAX_LABEL_NAME 10
#define SIZE_BUF 100
#define SIZE_LABEL 15
#define INVALID_ADDRESS -1
#define PUSH_SUCCESSFULLY true
#define PUSH_FAILED false
#define NUM_COMP_PASSES 2


enum regs {
    RESTRICTED_AREA = 0,
    AX = 1,
    BX = 2,
    CX = 3,
    DX = 4,
};

typedef struct Label {
    long long add;
    char name[MAX_LABEL_NAME];
} label_t;

typedef struct Comp{
    type* code;
    size_t ip;
} comp_t;


enum cmds {
    CMD_HLT = -1,
    CMD_PUSH_V = 1,
    CMD_ADD = 2,
    CMD_SUB = 3,
    CMD_DIV = 4,
    CMD_OUT = 5,
    CMD_JUMP_A = 6,
    CMD_MUL = 7,
    CMD_JUMP = 8,
    CMD_JUMP_B = 9,
    CMD_POP = 10,
    CMD_PUSH_R = 11, 
};

enum regs compFindReg (char* str);
void compRunCmd       (comp_t* Comp, label_t* Label);
void initComp         (comp_t* Comp);
label_t* initLabel    ();
bool pushLabel        (label_t* Label, char* str, size_t size, size_t ip);
size_t findLabel      (label_t* Label, char* str);
void dump             (label_t* Label);
void dtorLabel        (label_t* Label);
void passComp         (comp_t* Comp, label_t* Label);

#endif //PROC_H__