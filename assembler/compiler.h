#pragma once

#include <limits.h>

#include "../processor/asm_uitility.h"


/**
 * @brief Translates source code into byte code
 *
 * @param[ in ] filename name of source file
 *
 * @param[ in ] mode Defines should we read from source file or from stdin
 *
 * @return Exit status ( Success if OK & Failure otherwise )
 */
SPU
assemble ( char filename[ PATH_MAX ] );

/**
 * @brief Prints into log-file essential data about unknown command
 */
void
unk_com ( char* comm, stream* assembler, size_t* line );


void
assemble_exit( SPU* spu, labels lab );


void
print_byn ( FILE* list, char* byte_code, instruct* command, double value, char* arg, size_t PC );
