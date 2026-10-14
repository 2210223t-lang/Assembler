#pragma once

#include <stdio.h>
#include "../stack.h"

#define $RT fprintf( stderr, "\e[0;95m RTRTRTRTRTRT!!!!!\n\e[0m" );

void puts_debug( const char* a, FILE* ostream );

enum TYPES
{
    UNKNOWN     = 0,
    FLOAT_S     = 1,
    DOUBLE_S    = 2,
    STRING_S    = 3,
    INT_S       = 4,
    LONG_LONG_S = 5,
    UINT64_S    = 6,
    CHAR_S      = 7,
    SHORT_S     = 8,
    UINT32_S    = 9,
    UINT16_S    = 10,
};

//TODO delete generic
#define GetType( variable ) _Generic( variable, float* : FLOAT_S, double* : DOUBLE_S, char** : STRING_S,\
                                                int* : INT_S, long long* : LONG_LONG_S, unsigned long long* : UINT64_S,\
                                                char* : CHAR_S, short* : SHORT_S, unsigned short* : UINT16_S,\
                                                unsigned int* : UINT32_S, default : UNKNOWN )

int UniPrint( stack_data* stk );
