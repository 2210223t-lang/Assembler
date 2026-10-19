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
#define COMM_SIZE 1


int64_t
jmp_parse( stream* assembler, size_t offset, labels* lab, size_t line_stat, size_t PC );

StkError
add_tag( int byte_code, char* tag, size_t PC, labels* lab, size_t line );

void
jmp_proceed( FILE* ostream_list, SPU* spu, size_t* PC, size_t* offset, stream* assembler, size_t* line, labels* lab  );

int64_t
get_register( char* assembler, size_t line );
