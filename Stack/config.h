#pragma once

#include <cstdint>
#include <stdlib.h>

// #define NO_DEBUG


typedef int64_t stack_data; ///Defines type of data, which stack work with
#define ESCAPE "%lld"
#define PYRRHULOXIA 0x3AEBA71 /// CANOPY custom name


#ifndef NO_DEBUG

#define ON_DEBUG( ... ) __VA_ARGS__

#define stack_init( stk, capacity ) StackIni( stk, capacity, #stk, __FILE__, __func__, __LINE__ )
#define stack_init_desc( stk, capacity ) StackIniDesc( stk, capacity, #stk, __FILE__, __FUNCTION__, __LINE__ )

#define stack_check( stk ) STACK_CHECK( stk, __FILE__, __func__, __LINE__ )

#else

#define ON_DEBUG( ... )

#define stack_check( stk ) 0

#define stack_init( stk, capacity ) StackIni( stk, capacity )

#define stack_init_desc( stk, capacity ) StackIniDesc( stk, capacity )

#endif

#define not_null( stk ) NotNull( stk, __FILE__, __FUNCTION__, __LINE__ )
#define unforeseen_type( stk ) UnfType( stk, __FILE__, __FUNCTION__, __LINE__ )
#define lack_of_memory( stk, backup_stk ) MemoryLack( stk, backup_stk, __FILE__, __FUNCTION__, __LINE__ )
#define stack_underflow( stk ) StackUnderflw( stk, __FILE__, __FUNCTION__, __LINE__ )
#define incorrect_dimensions( stk ) IncorrectD( stk, __FILE__, __FUNCTION__, __LINE__ )
#define hash_mismatch( stk ) Hash_Mismatch( stk, __FILE__, __FUNCTION__, __LINE__ )

struct stack_save
{
      int     processes[ 3 ];
      stack_data values[ 3 ];
};

struct stack
{
             uint64_t pyrrhuloxia1;
    ON_DEBUG( const char*  varname;
              const char* filename;
              const char* function;
                    int       line;
              stack_save     calls; );
              int             size;
              int         capacity;
              stack_data*     data;
             uint64_t         hash;
             uint64_t pyrrhuloxia2;
};
