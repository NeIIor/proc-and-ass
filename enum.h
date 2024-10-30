#ifndef ENUM_H__
#define ENUM_H__

enum regs {
    RESTRICTED_AREA = 0,
    AX = 1,
    BX = 2,
    CX = 3,
    DX = 4,
};

enum cmds {
    CMD_HLT    = -1,
    CMD_PUSH   =  1,
    CMD_ADD    =  2,
    CMD_SUB    =  3,
    CMD_DIV    =  4,      //break backwards coimpatibility in favour of performance (unite aruphmetic CMDS)
    CMD_MUL    =  5,
    CMD_POW    =  6,
    CMD_SQRT   =  7,
    CMD_OUT    =  8, 
    CMD_JUMP   =  9,
    CMD_JUMP_A = 10, 
    CMD_JUMP_B = 11,
    CMD_POP    = 12, 
};

enum masks {
    CONST = 1 << 0,
    REG =   1 << 1,
    RAM =   1 << 2,
};

#endif // ENUM_H__