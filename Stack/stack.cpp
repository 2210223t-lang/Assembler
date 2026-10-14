#include <assert.h>
#include <cstdint>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "utility/errors.h"
#include "config.h"
#include "utility/log.h"
#include "stack.h"
#include "utility/Utility.h"
#include "utility/hash.h"

struct stack_stat
{
    stack storage;
    int status;
};

static stack_stat* stack_array;
static int stack_count = 0;


void StackIni( stack* stk,       int   capacity
                ON_DEBUG(, const char*  varname,
                           const char* filename,
                           const char* function,
                           const int       line
                           /*stack_save     calls*/ ) )
{
    assert( stk );
    int status = 0;

    stk->pyrrhuloxia1 = PYRRHULOXIA;
    stk->pyrrhuloxia2 = PYRRHULOXIA;
    stk->capacity = capacity;
    stk->hash = 0;

    init_info( stk ON_DEBUG(, varname, filename, function, line ) );

    assert( !not_null( stk ) && !unforeseen_type( stk ) && !incorrect_dimensions( stk ) );

    stack backup = *stk;
    size_t byte_size = 2 + ( sizeof( stack_data ) * stk->capacity + sizeof( uint64_t ) - 1 ) / sizeof( uint64_t );

    stk->data = ( stack_data* ) calloc( byte_size, sizeof( uint64_t ) );
    stk->size = 1; /// Not 0, to leave space for canary

    assert( !lack_of_memory( stk, &backup ) );
    uint64_t* ins_canary = ( uint64_t* ) stk->data + byte_size - 1;

    *ins_canary             = PYRRHULOXIA;
    *( uint64_t* )stk->data = PYRRHULOXIA;

    stk->hash = Hash_Calc( stk );

    #ifndef NO_DEBUG
        printlg( "---| Variable initialized with canary |---\n" );
        print_metadata( stk );
        printlg( "Canopy value: 0x%X\n", PYRRHULOXIA );
        printlg( "------------------------------\n\n");
    #endif // NO_DEBUG

    assert( !stack_check( stk ) );
}

void stack_destr( stack* stk )
{
    assert( !pyrrhuloxia_check( stk, __FILE__, __FUNCTION__, __LINE__ ) );

    if ( stk->data )
    {
        free( stk->data );
    }

    #ifndef NO_DEBUG
        printlg( "---| Destroyed variable |---\n" );
        print_metadata( stk );
        printlg( "----------------------------\n\n" );
    #endif // NO_DEBUG
}

void stack_push( stack* stk, stack_data temp )
{
    assert( stk );
    assert( !stack_check( stk ) );

    stack backup = *stk;

    if ( stk->capacity == stk->size )
    {
        uint64_t* canopy = ( uint64_t* ) ( ( char* ) stk->data + sizeof( uint64_t ) + //TODO use ( char* ) with canopy calc
                     sizeof( uint64_t ) * ( ( stk->capacity * sizeof( stack_data ) + 7 ) / sizeof( uint64_t ) ) );
        *canopy = 0;

        stk->capacity *= 2;
        size_t byte_size = 2 + ( sizeof( stack_data ) * stk->capacity + sizeof( uint64_t ) - 1 ) / sizeof( uint64_t );
        stk->data = ( stack_data* ) realloc( stk->data, 8 * byte_size );

        assert ( !lack_of_memory( stk, &backup ) );

        canopy =( uint64_t* ) ( ( char* ) stk->data + sizeof( uint64_t ) +
          sizeof( uint64_t ) * ( ( stk->capacity * sizeof( stack_data ) + 7 ) / sizeof( uint64_t ) ) );

        *canopy = PYRRHULOXIA;
    }
    stack_data* elem = ( stack_data* ) ( ( char* ) stk->data + sizeof( uint64_t ) + ( stk->size - 1 ) * sizeof( stack_data ) );
    *elem = temp;
    stk->size++;

    stk->hash = 0;
    stk->hash = Hash_Calc( stk );

    assert( !stack_check( stk ) );
    // ChangeSave( stk, Pushing, temp );

    #ifndef NO_DEBUG
        printlg( "Pushing success\n"
                 "---| Pushed variable |---\n" );
        print_metadata ( stk );

        if ( backup.capacity != stk->capacity )
            printlg( "Capacity change: [ %d ]->[ %d ]\n", backup.capacity, stk->capacity );
        else
            printlg( "Without changing capacity\n");

        printlg( "Initialized value: ");
        UniPrint( &temp );
        printlg( "\n-------------------------\n\n" );
    #endif

}

stack_data stack_pop( stack* stk )
{
    assert( stk );
    assert( !stack_check( stk ) );
    assert( !stack_underflow( stk ) );


    int backup = stk->capacity;
    stk->size--;
    stack_data* temp = ( stack_data* ) ( ( char* ) stk->data + sizeof( uint64_t ) + sizeof( stack_data ) * ( stk->size - 1 ) );
    stack backup_stk = *stk;
    stack_data temp_d = *temp;

    if ( stk->size < stk->capacity / 4 )
    {
        stk->capacity /= 4;

        size_t byte_size = 2 + ( sizeof( stack_data ) * stk->capacity + sizeof( uint64_t ) - 1 ) / sizeof( uint64_t );

        stk->data = ( stack_data* ) realloc( stk->data, 8 * byte_size );

        assert( !lack_of_memory( stk, &backup_stk ) );

        uint64_t* canary = ( uint64_t* ) ( ( uint64_t* ) stk->data + byte_size - 1 );
        *canary = PYRRHULOXIA;
    }

    *temp = 0;

    stk->hash = 0;
    stk->hash = Hash_Calc( stk );

    assert( !stack_check( stk ) );
    // ChangeSave( stk, Pop,)
    #ifndef NO_DEBUG
        printlg( "Popping success\n"
                 "---| Variable |---\n" );
        print_metadata( stk );

        if ( stk->capacity != backup )
            printlg( "Capacity changed: [ %d ]->[ %d ]\n", backup, stk->capacity );
        else
            printlg( "Without changing capacity\n" );

        printlg( "Popped value: ");
        UniPrint( &temp_d );
        printlg( "\n------------------\n\n" );
    #endif


    return temp_d;
}

void init_info( stack* stk ON_DEBUG(,  const char*  varname,
                                       const char* filename,
                                       const char* function,
                                       const   int     line ) )
{
    assert( stk );

    #ifndef NO_DEBUG

    stk->varname  =  varname;
    stk->filename = filename;
    stk->function = function;
    stk->line = line;

    #endif // NO_DEBUG
}

StkError STACK_CHECK( stack* stk, const char* file_call, const char* func_call, const int line_call )
{
    assert( stk );

    StkError status = Success;

    status = null_pointer( stk, file_call, func_call, line_call );

    if ( status )
        return status;

    status = IncorrectD( stk, file_call, func_call, line_call );

    if ( status )
        return status;

    status = stack_overflow( stk, file_call, func_call, line_call );

    if ( status )
        return status;

    status = pyrrhuloxia_check( stk, file_call, func_call, line_call );

    if ( status )
        return status;

    status = Hash_Mismatch( stk, file_call, func_call, line_call );

    return status;
}

int StackIniDesc( int* desc, int capacity
                   ON_DEBUG(, const char*  varname,
                              const char* filename,
                              const char* function,
                              const int       line ) )
{
    stack_count++;
    if ( !stack_count )
        stack_array = ( stack_stat* ) calloc( 1, sizeof( stack_stat ) );
    else
        stack_array = ( stack_stat* ) realloc( stack_array, sizeof( stack_stat ) );
    assert( stack_array );
    stack_array[ stack_count - 1 ].status = 1;
    StackIni( &stack_array[ stack_count - 1 ].storage, capacity ON_DEBUG( , varname, filename, function, line ) );
    *desc = stack_count - 1;
    return stack_count - 1;
}

void stack_pop_desc( int desc )
{
    assert( desc <= stack_count && stack_array[ desc ].status );
    stack_pop( &( stack_array[ desc ].storage ) );
}

void stack_push_desc( int desc, stack_data temp )
{
    assert( desc <= stack_count && stack_array[ desc ].status );
    stack_push( &stack_array[ desc ].storage, temp );
}

void stack_destr_desc( int* desc )
{
    assert( *desc <= stack_count && stack_array[ *desc ].status );
    stack_destr( &( stack_array[ *desc ].storage ) );
    stack_array[ *desc ].status = 0;
    stack_count--;

    int count = 0;
    for ( int i = 0; i < stack_count; i++ )
        if ( !( stack_array[ i ].status ) )
            count++;

    if ( !count )
        free( stack_array );
    *desc = -1;

}
