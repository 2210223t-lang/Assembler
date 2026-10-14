#pragma once

#include "../config.h"

enum StkError
{
    Success = 0,
    Failure = 1,
    NULLPTR = 411,
    STACK_OVERFLOW = 412,
    INCORRECT_DIMENSIONS = 413,
    UNFORESEEN_TYPE = 414,
    UNIPRINT_FAILURE = 415,
    NOT_NULL_INIT = 416,
    DATA_SHRINKAGE = 417,
    STACK_UNDERFLOW = 418,
    MEMORY_LACK = 419,
    CANARY_FAULT = 420,
    HASH_MISMATCH = 421,

};

/**
 * @brief Checks if user is trying to initialized not-null variable
 *
 * @details Repo count on using stk_destr function
 */
StkError NotNull( stack* stk, const char* filename, const char* function, const int line );

/**
 * @brief Checks if user defined correct variable type
 */
StkError UnfType( stack* stk, const char* filename, const char* function, const int line );

/**
 * @brief Checks if stk->data was initialized by calloc correctly
 *
 * @param[ in ] stk stack to check
 *
 * @param [ in ] backup_stk stk backup
 */
StkError MemoryLack( stack* stk, stack* backup_stk, const char* filename, const char* function, const int line );

/**
 * @brief Checks if stk->size < 0
 */
StkError StackUnderflw( stack* stk, const char* filename, const char* function, const int line );

/**
 * @brief Checks dimensions correctness
 */
StkError incorrect_dimension( stack* stk, const char* filename, const char* function, const int line );

/**
 * @brief Checks stk pointer correctness
 */
StkError null_pointer( stack* stk, const char* filename, const char* function, const int line );
StkError stack_overflow( stack* stk, const char* filename, const char* function, const int line );
StkError pyrrhuloxia_check( stack* stk, const char* filename, const char* function, const int line );
StkError IncorrectD( stack* stk, const char* filename, const char* function, const int line );
StkError Hash_Mismatch( stack* stk, const char* filename, const char* function, const int line );
