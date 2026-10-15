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
    char** name;
    size_t* line;
    int quantity;
    int capacity;
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

    size_t         PC;
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

size_t
calc_offset( stream* assembler, size_t* PC );

void
move_offset( stream* assembler, size_t* PC, size_t* offset );

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
