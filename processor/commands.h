#pragma once

#include "../Stack/stack.h"
#include "asm_uitility.h"


/**
 * @brief Divides last 2 elements of stk
 *
 * @return Exit status ( Success if OK & Failure otherwise )
 */
int
div ( stack* stk, short* past_proc_sign );

/**
 * @brief Multiplies last 2 elements of stk
 *
 * @return Exit status ( Success if OK & Failure otherwise )
 */
int
mult ( stack* stk, short* past_proc_sign );

/**
 * @brief Summarizes last 2 elements of stk
 *
 * @return Exit status ( Success if OK & Failure otherwise )
 */
int
sum ( stack* stk, short* past_proc_sign );

/**
 * @brief Prints last element of stack
 *
 * @return Exit status ( Success if OK & Failure otherwise )
 */
int
out ( stack* stk );

/**
 * @brief Cleans stack
 *
 * @return Exit status ( Success if OK & Failure otherwise )
 */
int
rest ( stack* stk );

/**
 * @brief Subtracts last 2 elements of stack
 *
 * @return Exit status ( Success if OK & Failure otherwise )
 */
int
sub ( stack* stk, short* past_proc_sign );

/**
 * @brief Calculates square root of last variable in stack
 *
 * @return Exit status ( Success if OK & Failure otherwise )
 */
int
sqr ( stack* stk, short* past_proc_sign );

/**
 * @brief Calculates factorial of last value of stack
 *
 * @return Exit status ( Success if OK & Failure otherwise )
 */
int
fact ( stack* stk );

/**
 * @brief Pushes value into the stack
 *
 * @return Exit status ( Success if OK & Failure otherwise )
 */
int
push ( stack* stk, stack_data* value, short* past_proc_sign );

/**
 * @brief Pops last value of the stack
 *
 * @return Exit status ( Success if OK & Failure otherwise )
 */
int
pop ( stack* stk, stack_data* storage );

/**
 * @brief Finds sinus of last value in stack
 *
 * @return Exit status ( Success if OK & Failure otherwise )
 */
int
sinus ( stack* stk, short* past_proc_sign );

/**
 * @brief Jumps to value destination
 *
 * @return Exit status ( Success if OK & Failure otherwise )
 */
int
jmp ( size_t* PC, size_t* value );

/**
 * @brief return to a dest
 *
 * @return Exit status ( Success if OK & Failure otherwise )
 */
int
ret ( SPU* spu );

/**
 * @brief calls function
 *
 * @return Exit status ( Success if OK & Failure otherwise )
 */
int
call ( SPU* spu, size_t dest );

/**
 * @brief calculates square root of the last element in stack
 *
 * @return Exit status ( Success if OK & Failure otherwise )
 */
int
root ( SPU* spu );

/**
 * @brief Takes double value from the console
 *
 * @return Exit status ( Success if OK & Failure otherwise )
 */
StkError
in ( SPU* spu );

/**
 * @brief Proceeds pop command
 *
 * @details Parses instruct of current pop for ram, constant either register usage
 *
 * @return Exit status
 */
int
pop_execute ( instruct command, instruct* code, SPU* spu );

/**
 * @brief Proceeds push command
 *
 * @details Parses instruct of current push for ram, constant either register usage
 *
 * @return Exit status
 */
int
push_execute ( instruct command, instruct* code, SPU* spu );

void
music ();
