#pragma once

#include "config.h"
#include "utility/errors.h"


/**
 * @brief Destroys choosen stack - variable
 */
void stack_destr( stack* stk );

/**
 * @brief Initializes stack variable
 */
void StackIni( stack* stk,       int   capacity
                ON_DEBUG(, const char*  varname,
                           const char* filename,
                           const char* function,
                           const int       line ) );

/**
 * @brief Pushes stack with temp variable
 *
 * @param[ in ] temp value to be initialized
 *
 * @param[ in out ] stk where add temp
 */
void stack_push( stack* stk, stack_data temp );

/**
 * @brief Standard pop function
 *
 * @return Popped value
 */
stack_data stack_pop( stack* stk );

/**
 * @brief Initializes extra data for debugging mode
 */
void init_info( stack* stk1 ON_DEBUG(, const char*  varname,
                                       const char* filename,
                                       const char* function,
                                       const   int     line ) );

/**
 * @brief Checks stacks for errors
 *
 * @return Error's code
 */
StkError STACK_CHECK( stack* stk, const char* file_call, const char* func_call, const int line_call );

/**
 * @brief StackIni for desc
 */
int StackIniDesc( int* desc, int capacity
                   ON_DEBUG(, const char*  varname,
                              const char* filename,
                              const char* function,
                              const int       line ) );

/**
 * @brief Stack_Pop for desc
 */
void stack_pop_desc( int desc );

/**
 * @brief Stack_push for desc
 */
void stack_push_desc( int desc, stack_data temp );

/**
 * @brief Stack_desc for desc
 */
void stack_destr_desc( int* desc );
