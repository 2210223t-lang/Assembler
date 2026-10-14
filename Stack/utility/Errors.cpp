#include <cstdint>
#include <stdio.h>
#include <assert.h>
#include <malloc/malloc.h>

#include "colours.h"
#include "errors.h"
#include "log.h"
#include "Utility.h"
#include "hash.h"


StkError NotNull( stack* stk, const char* filename, const char* function, const int line )
{
    assert( stk );
    assert( filename );
    assert( function );

    if ( stk->data )
    {
        printlg( "Attempt to initialize not ( NULL ) pointer in <%s> file, <%s> function, <%d> line\n",
                  filename, function, line );

        #ifndef NO_DEBUG
        printlg( "---| Variable |---\n" );
        print_metadata( stk );
        stack_status( stk );
        printlg( "Possible issues:\n"
                 "1) Initializer failure.\n"
                 "2) Ignoring destroying function.\n" );
        printlg( "------------------\n\n" );
        #endif

        fprintf( stderr, "Program exited with code - " RED "%d" reset " ( NOT_NULL_INIT )\n"
                         "Check log-file to learn more about failure\n\n", NOT_NULL_INIT );
        return NOT_NULL_INIT;
    }
    return Success;
}

StkError UnfType( stack* stk, const char* filename, const char* function, const int line )
{
    assert( stk );
    assert( filename );
    assert( function );

    if ( !GetType( stk->data ) )
    {
        printlg( "Unforeseen type in <%s> file, <%s> function, <%d> line\n",
                  filename, function, line );

        #ifndef NO_DEBUG
        printlg( "---| Variable |---\n" );
        print_metadata( stk );
        printlg( "Possible issues:\n"
                "1) Initializer failure.\n"
                "2) Incorrect typedef stack_data.\n" );
        printlg( "------------------\n\n" );
        #endif

        fprintf( stderr, "Program exited with code -" RED " %d" reset " ( UNFORSEEN_TYPE )\n"
                         "Check log-file to learn more about failure\n\n", UNFORESEEN_TYPE );
        return UNFORESEEN_TYPE;
    }

    return Success;
}

StkError MemoryLack( stack* stk, stack* backup_stk, const char* filename, const char* function, const int line )
{
    assert( stk );
    assert( backup_stk );
    assert( filename );
    assert( function );

    if ( !stk->data )
    {
        printlg( "Lack of memory in <%s> file, <%s> function, %d line\n",
                  __FILE__, __FUNCTION__, __LINE__ );


        printlg( "---| Variable |---\n");
        print_metadata( stk );
        stack_status( backup_stk );
        printlg( "Possible issues:\n"
                 "1) Incorrect pushing realization\n"
                 "2) Lack of memory, realloc can't find enough space\n\n"
                 "-------------------\n\n" );


        *stk = *backup_stk;

        fprintf( stderr, "Code exited with code " RED "%d" reset " ( MEMORY_LACK )\n"
                         "To learn more, check log-file\n\n", MEMORY_LACK );
        return MEMORY_LACK;
    }
    return Success;
}

StkError StackUnderflw( stack* stk, const char* filename, const char* function, const int line )
{
    assert( stk );
    assert( filename );
    assert( function );

    if ( stk->size == 1 )
    {
        printlg( "STACK_UNDERFLOW in <%s> file, <%s> function, %d line\n",
                 filename, function, line );
        printlg( "--| Variable |---\n");
        print_metadata( stk );
        stack_status( stk );
        printlg( "Possible issue:\n"
                 "Incorrect logic\n"
                 "-----------------\n\n" );
        fprintf( stderr, "Code exited with code " RED "%d" reset " ( STACK_UNDERFLOW )\n"
                         "To learn more, check log - file\n\n", STACK_UNDERFLOW );
        return STACK_UNDERFLOW;
    }
    return Success;
}

StkError IncorrectD( stack* stk, const char* filename, const char* function, const int line )
{
    assert( stk );
    assert( filename );
    assert( function );

    size_t byte_size = 2 + ( sizeof( stack_data ) * stk->capacity + sizeof( uint64_t ) - 1 ) / sizeof( uint64_t );

    //< Checks if stk->data size equals real stored value
    bool condition = ( 8 * byte_size - malloc_size( stk->data ) < 8  &&
                       8 * byte_size - malloc_size( stk->data ) > -8 && stk->data ) ? true : false;

    if ( condition )
        $RT
    if ( stk->size < 0 || stk->capacity <= 0 || condition ) /// stk->data size + canaries sizes
    {
        printlg( "INCORRECT_DIMENSIONS in <%s> file, <%s> func, %d line\n", filename, function, line );
        printlg( "---| Variable |---\n" );
        print_metadata( stk );
        stack_status( stk );
        printlg( "Possible issues:\n"
                 "1) Incorrect push/pop indexation\n"
                 "2) Incorrect initializer\n"
                 "---------------------\n\n" );

        fprintf( stderr, "Program exited with code - " RED "%d" reset " ( INCORRECT_DIMENSIONS )\n"
                         "Check log - file to learn more\n", INCORRECT_DIMENSIONS );
        return INCORRECT_DIMENSIONS;
    }
    return Success;
}

StkError null_pointer( stack* stk, const char* filename, const char* function, const int line )
{
    assert( stk );
    assert( filename );
    assert( function );

    if ( !stk->data )
    {
        printlg( "Null pointer in <%s> file, <%s> function, <%d> line\n", filename, function, line );
        printlg( "---| Variable |---\n" );
        print_metadata( stk );
        stack_status( stk );
        printlg( "Possible issues:\n"
                 "1) Initializer failure.\n"
                 "2) Incorrect struct build.\n"
                 "3) Memory overflow, check pushing and pulling func\n" );
        printlg( "------------------\n\n" );

        fprintf( stderr, "Program exited with code - " RED "%d" reset " ( NULLPTR )\n"
                         "Check log-file to learn more about failure\n\n", NULLPTR );
        return NULLPTR;
    }
    return Success;
}

StkError stack_overflow( stack* stk, const char* filename, const char* function, const int line )
{
    assert( stk );
    assert( filename );
    assert( function );

    if ( stk->size > stk->capacity )
    {

        printlg( "STACK_OVERFLOW in <%s> file, <%s> func, %d line\n", filename, function, line );
        printlg( "---| Variable |---\n");
        print_metadata( stk );
        stack_status( stk );
        printlg( "Possible issues:\n"
                 "1) Incorrect push/pop func\n"
                 "2) Incorrect initializer\n" );
        printlg( "------------------\n\n" );

        fprintf( stderr, "Program exited with code - " RED "%d" reset " ( STACK_OVERFLOW )\n"
                         "Check log- file to learn more about failure\n\n", STACK_OVERFLOW );
        return STACK_OVERFLOW;
    }
    return Success;
}

/**
 * @brief Checks canaries status
 */
StkError pyrrhuloxia_check( stack* stk, const char* filename, const char* function, const int line )
{
    assert( stk );
    assert( filename );
    assert( function );

    uint64_t ins_canary = *( ( uint64_t* ) stk->data + 1 + ( sizeof( stack_data ) * stk->capacity + 7 ) / sizeof( uint64_t ) );

    if ( *( uint64_t* ) stk->data != PYRRHULOXIA || ins_canary != PYRRHULOXIA ||
         stk->pyrrhuloxia1 != PYRRHULOXIA || stk->pyrrhuloxia2 != PYRRHULOXIA )
    {
        printlg( "Canary inconsistency in <%s> file, <%s> function, %d line\n", filename, function, line );
        printlg( "---| Variable |---\n" );
        print_metadata( stk );
        stack_status( stk );
        printlg( "\n------------------\n" );

        fprintf( stderr, "Program exited with code: " RED "%d" reset " ( CANARY_FAULT )\n"
                         "To learn more, check log - file\n", CANARY_FAULT );
        return CANARY_FAULT;
    }

    return Success;
}

/**
 * @brief Checks hash
 */
StkError Hash_Mismatch( stack* stk, const char* filename, const char* function, const int line )
{
    assert( stk );
    assert( filename );
    assert( function );

    uint64_t hash_back = stk->hash;
    stk->hash = 0;
    uint64_t hash = Hash_Calc( stk );

    if ( hash != hash_back )
    {

        printlg( "Hash mismatch in <%s> file, <%s> function, %d line\n", filename, function, line );
        printlg( "Hash exp-> 0x%X VS 0x%X <-Real hash\n", hash, hash_back );
        printlg( "---| Variable |---\n" );
        print_metadata( stk );
        stack_status( stk );
        printlg( "------------------\n" );

        fprintf( stderr, "Program exited with code - " RED"%d" reset " ( HASH_MISMATCH )\n"
                         "To learn more , check log - file\n", HASH_MISMATCH );
        return HASH_MISMATCH;
    }
    stk->hash = hash_back;
    return Success;
}
