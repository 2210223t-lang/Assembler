#pragma once

#include <stdlib.h>

#include "../processor/asm_uitility.h"

enum
label_exit
{
    DONT_EXIST = -1,
    ERROR = -2,
};


int
jmp_parse( stream* assembler, size_t offset, labels* lab, size_t PC );
