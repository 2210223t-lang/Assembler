#pragma once

#include <limits.h>

#include "Stack/stack.h"


/**
 * @brief Parses command line arguments and checks input
 *
 * @param[ in ] argc - std argc
 *
 * @param[ in ] argv - std argv
 *
 * @param[ out ] input char[ PATH_MAX ] - name of input filestream
 *
 * @return Where from to take source code ( 1 - file, 0 - stdin )
 */
int
Terminal( int argc, char** argv, char input[ PATH_MAX ] );

/**
 * @brief Starts calculation
 *
 * @param[ in ] stk - stack which will be used during calculating
 *
 * @param[ in ] input - name of file where from to read commands
 *
 * @return Exit status ( Success if OK & Failure otherwise )
 */
int
Start_Calc( stack* stk, int status );
