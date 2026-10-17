#include <stdio.h>
#include <cstdint>
#include <string.h>
#include <assert.h>

#include "../config.h"


/**
 * @brief Standard djb2 hashing function
 */
uint64_t hash_djb2( unsigned char *str, int64_t size )
{
    uint64_t hash = 5381;

    for ( int64_t i = 0; i < size; i++ )
        hash = ( ( hash << 5 ) + hash ) + *( str++ ); /* hash * 33 + c */

    return hash;
}

/**
 * @brief Hashs stk variable with one of algorithms
 */
uint64_t Hash_Calc( stack* stk )
{
    assert( stk );
    assert( stk->data );
    #ifndef NO_DEBUG
    assert( stk->filename );
    assert( stk->function );
    assert( stk->varname );
    #endif

    uint64_t hash = hash_djb2( ( unsigned char* ) stk, sizeof( stack ) );
   hash = ( ( hash << 5 ) + hash ) + hash_djb2( ( unsigned char* ) stk->data, stk->capacity * sizeof( stack_data ) );

    return hash;
}
