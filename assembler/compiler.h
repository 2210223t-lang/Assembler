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
assemble( char filename[ PATH_MAX ] );
