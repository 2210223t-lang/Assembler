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


/**
 * @brief Parses jmp arguments
 *
 * @return JMP destination
 */
int64_t
jmp_parse( stream* assembler, size_t offset, labels* lab, size_t line_stat, size_t PC );

/**
 * @brief Adds tag into labels
 */
StkError
add_tag( int byte_code, char* tag, size_t PC, labels* lab, size_t line );

/**
 * @brief Prints jmp into byte_code and also parse assembler
 */
void
jmp_proceed( FILE* ostream_list, SPU* spu, size_t* PC, size_t* offset, stream* assembler, size_t* line, labels* lab  );

/**
 * @brief Associates char* with register number
 */
int64_t
get_register( char* assembler, size_t line );

/**
 * @brief Parses argument line as memory equation
 */
StkError
mem_proceed( stream* assembler, size_t* offset, size_t line, FILE* list, int desc, instruct* com, size_t* PC );
