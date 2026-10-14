#pragma once

#include <stdlib.h>
#include <limits.h>
#include <stdio.h>

#include "Stack/utility/errors.h"

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
};

enum
reading_types
{
    STRING = 1,
    INT64_T = 2,
};


struct
label
{
    char* name;
    size_t line;
};

struct
SPU
{
    int64_t* byte_code;
    size_t position;
    size_t code_size;
    size_t assembler_size;
    int byte_code_desc;
    int assembler_desc;
    char* assembler;
    FILE* byte_code_file;
    label* Labels;
    StkError status;
    stack stk;
};

uint64_t
Getsize( const char* filename );

StkError
mscan_double( char* text, double* variable );

StkError
SPUcomp_init( const char filename[ PATH_MAX ], SPU* spu );

size_t
count_lines( SPU* spu );

void
move_position( SPU* spu );

int
my_strcmp( const char* a, const char* b );

void
spu_dump( SPU* spu );

void
my_printlg( char* var );
