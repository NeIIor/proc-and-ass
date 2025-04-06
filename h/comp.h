#ifndef PROC_H__
#define PROC_H__
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <ctype.h>
#include "gen.h"

typedef long int type;
#define SPECIFICATOR "%ld"
#define SIZE_CMD 15
#define MAX_LABEL_NAME 30
#define MAX_NUM_LABELS 15
#define INVALID_ADDRESS -1
#define PUSH_SUCCESSFULLY true
#define PUSH_FAILED false
#define NUM_COMP_PASSES 2

typedef struct Label {
    long long add;
    char name[MAX_LABEL_NAME];
} label_t;

typedef struct {
    char* name;
    int num;
    int type;
} cmd_t;

typedef struct Comp{
    type* code;
    size_t ip;
} comp_t;

enum CMD_TYPE {
    NO_ARG = 0,
    PUSH   = 1,
    POP    = 2,
    JUMP   = 3,
};

enum regs compFindReg (const char* str);
int compRunComp       (comp_t* Comp, label_t* Label);
void initComp         (comp_t* Comp);
label_t* initLabels   ();
bool pushLabel        (label_t* Labels, const char* str, const size_t size, const size_t ip);
size_t findLabel      (const label_t* Labels, const char* str);
void dumpLabels       (const label_t* Labels);
void dtorLabels       (label_t* Labels);
int passComp          (comp_t* Comp, label_t* Labels);
int compArgPush       (comp_t* Comp, FILE* cmd);
int compArgPop        (comp_t* Comp, FILE* cmd);
int compArgJump       (comp_t* Comp, label_t* Labels, FILE* cmd);
#endif //PROC_H__