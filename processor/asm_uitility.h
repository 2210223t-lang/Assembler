#pragma once

#define ASM_DEBUG

#define REG_AM 10

#include <stdlib.h>
#include <limits.h>
#include <stdio.h>

#include "../Stack/utility/errors.h"

enum codes
{
    SQRT = -2,
    JMPN = -1,
    HLT  =  0,
    PUSH =  1,
    ADD  =  2,
    DIV  =  3,
    OUT  =  4,
    POP  =  5,
    SUB  =  6,
    FACT =  7,
    MULT =  8,
    REST =  9,
    JMP  = 10,
    SQR  = 11,
    CALL = 12,
    RET  = 13,
    JMPP = 14,
    JMPZ = 15,
};

enum registers
{
    INCORRECT_REGISTER = -1,
    REX = 0,
    RAX = 1,
    RBX = 2,
    RCX = 3,
    RDX = 4,
    RSI = 5,
    RDI = 6,
    RSP = 7,
    RBP = 8,
    RCP = 9,
};

struct instruct
{
    char num :5;
    char ram :1;
    char con :1;
    char reg :1;
};

enum
reading_types
{
    STRING = 1,
    INT64_T = 2,
};

struct
labels
{
    size_t* pc;
    size_t* jmp_pc;
    int capacity;
    int quantity;
    char** name;
};

struct
stream
{
    size_t           size;
    int              desc;
    int        comm_count;
    char*        virt_mem;
    char name[ PATH_MAX ];
};

struct
SPU
{

    size_t                      PC;
    stack                      stk;
    labels                     lab;
    StkError                status;
    stream               byte_code;
    stack_data registers[ REG_AM ];
    stack                  ret_val;
    short           past_proc_sign;
};


uint64_t
Getsize( const char* filename );

StkError
mscan_double( char* text, double* variable );

stream
asm_open( char* name );

size_t
calc_offset( stream* assembler, size_t* PC );

void
move_offset( stream* assembler, size_t* offset, size_t* line );

int
my_strcmp( const char* a, const char* b );

void
spu_dump( SPU* spu );

void
my_printlg( char* var );

void
SPUrun_exit( SPU* spu );

StkError
SPUrun_init( SPU* spu );

StkError
SPUasm_init( SPU* spu );

StkError
mscan_long( char* text, int64_t* variable );

StkError
labels_init( labels* lab );

void
labels_exit( labels* lab );

void
SPUasm_exit( SPU* spu );

bool
check_register( char* assembler );
