#pragma once

#include <limits.h>

#include "../Stack/stack.h"
#include "asm_uitility.h"


/**
 * @brief Parses command line arguments and checks input
 *
 * @param[ in ] argc - std argc
 *
 * @param[ in ] argv - std argv
 *
 * @param[ out ] input char[ PATH_MAX ] - name of input filestream
 */
void
Terminal( int argc, char** argv, char input[ PATH_MAX ] );

/**
 * @brief Starts calculation
 *
 * @param[ in ] spu - soft_processing unit data
 *
 * @return Exit status ( Success if OK & Failure otherwise )
 */
int
Start_Calc( SPU* spu );
