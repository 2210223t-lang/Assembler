#pragma once

#include "../Stack/stack.h"


/**
 * @brief Divides last 2 elements of stk
 *
 * @return Exit status ( Success if OK & Failure otherwise )
 */
int
div( stack* stk );

/**
 * @brief Multiplies last 2 elements of stk
 *
 * @return Exit status ( Success if OK & Failure otherwise )
 */
int
mult( stack* stk );

/**
 * @brief Summarizes last 2 elements of stk
 *
 * @return Exit status ( Success if OK & Failure otherwise )
 */
int
sum( stack* stk );

/**
 * @brief Prints last element of stack
 *
 * @return Exit status ( Success if OK & Failure otherwise )
 */
int
out( stack* stk );

/**
 * @brief Cleans stack
 *
 * @return Exit status ( Success if OK & Failure otherwise )
 */
int
rest( stack* stk );

/**
 * @brief Subtracts last 2 elements of stack
 *
 * @return Exit status ( Success if OK & Failure otherwise )
 */
int
sub( stack* stk );

/**
 * @brief Calculates square root of last variable in stack
 *
 * @return Exit status ( Success if OK & Failure otherwise )
 */
int
sqr( stack* stk );

/**
 * @brief Calculates factorial of last value of stack
 *
 * @return Exit status ( Success if OK & Failure otherwise )
 */
 int
 fact( stack* stk );

/**
 * @brief Pushes value into the stack
 *
 * @return Exit status ( Success if OK & Failure otherwise )
 */
int
push( stack* stk, stack_data value );

/**
 * @brief Pops last value of the stack
 *
 * @return Exit status ( Success if OK & Failure otherwise )
 */
int
pop( stack* stk );

/**
 * @brief Finds sinus of last value in stack
 *
 * @return Exit status ( Success if OK & Failure otherwise )
 */
int
sinus( stack* stk );


/**
 * @brief Jumps to value destination
 *
 * @return Exit status ( Success if OK & Failure otherwise )
 */
int
jmp( size_t* PC, size_t value );
