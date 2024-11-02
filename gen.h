#ifndef ENUM_H__
#define ENUM_H__
#define HEAD 3
#include <stdint.h>
#define SIZE_CODE 1000
#define SIZE_RAM 100


enum regs {
    RESTRICTED_AREA = 0,
    AX = 1,
    BX = 2,
    CX = 3,
    DX = 4,
};

enum cmds {
    CMD_HLT              = -1,
    CMD_PUSH             =  1,
    CMD_ARITHMETIC_START =  2,
    CMD_ADD              =  2,
    CMD_SUB              =  3,
    CMD_DIV              =  4,      
    CMD_MUL              =  5,
    CMD_POW              =  6,
    CMD_SQRT             =  7,
    CMD_DIV_REM          =  8,
    CMD_BIT_MUL          =  9,
    CMD_BIT_ADD          = 10,
    CMD_BIT_SHIFT_L      = 11,
    CMD_BIT_SHIFT_R      = 12,
    CMD_ARIPHMETIC_END   = 12,
    CMD_OUT              = 13, 
    CMD_JUMP             = 14,
    CMD_JUMP_A           = 15, 
    CMD_JUMP_B           = 16,
    CMD_JUMP_EQ          = 17,
    CMD_POP              = 18, 
    CMD_IN               = 19,
    CMD_PUT_C            = 20,
    CMD_CALL             = 21,
    CMD_RET              = 22,  
};

typedef struct {
    uint64_t sign;
    uint64_t vers;
    uint64_t size;
} head_bin_proc_t;

enum arg_mask {
    ARG_CONST = 1 << 0,
    ARG_REG =   1 << 1,
    ARG_RAM =   1 << 2,
};

#endif // ENUM_H__