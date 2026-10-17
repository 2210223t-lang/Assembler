#pragma once

#define ASM_DEBUG

#include <stdlib.h>
#include <limits.h>
#include <stdio.h>

#include "../Stack/utility/errors.h"

enum codes
{
    HLT = 0,
    PUSH = 1,
    ADD = 2,
    DIV = 3,
    OUT = 4,
    POP = 5,
    SUB = 6,
    FACT = 7,
    MULT = 8,
    REST = 9,
    JMP = 10,
    SQR = 11,
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
    int64_t* pc;
    int64_t* jmp_pc;
    int capacity;
    int quantity;
    char** name;
};

struct
stream
{
    int64_t           size;
    int              desc;
    int        comm_count;
    char*        virt_mem;
    char name[ PATH_MAX ];
};

struct
SPU
{

    int64_t         PC;
    stack         stk;
    labels        lab;
    StkError   status;
    stream  byte_code;
};


uint64_t
Getsize( const char* filename );

StkError
mscan_double( char* text, double* variable );

stream
asm_open( char* name );

int64_t
calc_offset( stream* assembler, int64_t* PC );

void
move_offset( stream* assembler, int64_t* offset, int64_t* line );

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
