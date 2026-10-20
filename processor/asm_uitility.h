#pragma once

#define ASM_DEBUG

#define REG_AM 10
#define MEMORY_CAPACITY 10

#include <stdlib.h>
#include <limits.h>
#include <stdio.h>

#include "../Stack/utility/errors.h"

///Func optcodes
enum codes
{
    SQRT = -2,
    JMPN = -1,
    HLT  =  0,
    PUSH =  1,
    ADD  =  2,
    DIV  =  3,
    OUT  =  4,
    POP  =  5,
    SUB  =  6,
    FACT =  7,
    MULT =  8,
    REST =  9,
    JMP  = 10,
    SQR  = 11,
    CALL = 12,
    RET  = 13,
    JMPP = 14,
    JMPZ = 15,
};

/// Register optcodes
enum registers
{
    INCORRECT_REGISTER = -1,
    REX = 0,
    RAX = 1,
    RBX = 2,
    RCX = 3,
    RDX = 4,
    RSI = 5,
    RDI = 6,
    RSP = 7,
    RBP = 8,
    RCP = 9,
};

///Command optcode
struct
instruct
{
    char num :5;//< Command's number
    char ram :1;//< Command work with ram
    char con :1;//< Command work with const
    char reg :1;//< Command work with register
};

enum
reading_types
{
    STRING = 1,
    INT64_T = 2,
};


///Stores all usefull info about label
struct
labels
{
    size_t* pc;//< Where to jump
    size_t* jmp_pc;//< Where from jump
    int capacity;//< How many labels
    int quantity;//< Memory allocated for labels
    char** name;//< Label names
};


///Usefull info about stream
struct
stream
{
    size_t           size;//< Size in bytes
    char*        virt_mem;//< Pointer to virtually allocated file
    int              desc;//< File descriptor
    int        comm_count;//< How many command
    char name[ PATH_MAX ];//< Filename
};


///Sofr Processing Unit
struct
SPU
{

    size_t                                              PC;//< PC register
    stack                                              stk;//< Stack with variables
    labels                                             lab;//< Labels
    StkError                                        status;//< Execution status
    stream                                       byte_code;//< What to execute
    stack_data                         registers[ REG_AM ];//< User registers
    stack_data memory[ MEMORY_CAPACITY * MEMORY_CAPACITY ];//< RAM
    stack                                          ret_val;//< Stack to return
    short                                   past_proc_sign;//< Register which stores sign of last process
};


/**
 * @brief Calculates size in bytes of filename
 *
 * @return Size in bytes
 */
uint64_t
Getsize( const char* filename );

/**
 * @brief Scans text string for double variable
 *
 * @details Scans untill the space, \0 or \n
 *
 * @return Exit status
 */
StkError
mscan_double( char* text, double* variable );

/**
 * @brief Creates stream for file "name"
 */
stream
asm_open( char* name );

/**
 * @brief Calculates current offset for PC
 *
 * @param[ in ] assembler where to calculate offset
 */
size_t
calc_offset( stream* assembler, size_t* PC );

/**
 * @brief Moves offset, line till the next "word"
 *
 * @details Skips "\n", space symbols
 */
void
move_offset( stream* assembler, size_t* offset, size_t* line );

/**
 * @brief Compares a and b untill the first space of '\0'
 */
int
my_strcmp( const char* a, const char* b );

/**
 * @brief Dumps essential spu registers
 */
void
spu_dump( SPU* spu );

/**
 * @brief Prints var into log-file till the '\0' or space
 */
void
my_printlg ( char* var );

/**
 * @brief Clean spu for processor exit
 */
void
SPUrun_exit ( SPU* spu );

/**
 * @brief Initializes spu for processor start
 */
StkError
SPUrun_init ( SPU* spu );

/**
 * @brief Initializes spu for assembler
 */
StkError
SPUasm_init ( SPU* spu );

/**
 * @brief Scans text string for long long variable
 *
 * @details Scans untill the space, \0 or \n
 *
 * @return Exit status
 */
StkError
mscan_long ( char* text, int64_t* variable );

/**
 * @brief Labels initializer
 */
StkError
labels_init ( labels* lab );

/**
 * @brief Labels cleaner
 */
void
labels_exit ( labels* lab );

/**
 * @brief Cleans spu struct for assembler exit
 */
void
SPUasm_exit ( SPU* spu );

/**
 * @brief Checks whether char* argument is a register or not
 */
bool
check_arg ( char* assembler );

/**
 * @brief Dumps current RAM status into log-file
 */
void
mem_dump ( SPU* spu );
