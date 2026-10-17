#pragma once

#include <stdlib.h>

#include "../processor/asm_uitility.h"

enum
label_exit
{
    DONT_EXIST = -1,
    ERROR = -2,
};

#define  ARG_SIZE 8
#define COMM_SIZE 8


int64_t
jmp_parse( stream* assembler, int64_t offset, labels* lab, int64_t line_stat, int64_t PC );

StkError
add_tag( int byte_code, char* tag, int64_t PC, labels* lab, int64_t line );

void
jmp_proceed( FILE* ostream_list, SPU* spu, int64_t* PC, int64_t* offset, stream* assembler, int64_t* line, labels* lab  );
