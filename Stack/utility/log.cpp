#include <cstdint>
#include <cstdio>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <stdlib.h>

#include "log.h"
#include "Utility.h"

static FILE* logger_stream = stderr;//< Standard stream, which will be used if user didn't used logger_open


/**
 * @brief Changes output stream to a lo - file
 */
int logger_open()
{
    logger_stream = fopen( logger_name, "w" );
    return ( logger_stream ) ? 0 : -1;
}

/**
 * @brief fprintf( logger_stream ) realization
 */
void printlg( const char* text, ... )
{
    assert( text );

    va_list args;
    va_start( args, text );
    vfprintf( logger_stream, text, args );
    va_end( args );

    fflush( logger_stream );
}

/**
 * @brief Closes logger_stream and sets it to stderr
 */
void logger_close( void )
{
    if ( logger_stream != stderr )
        fclose( logger_stream );

    logger_stream = stderr;
}

/**
 * @brief Prints stack variable metadata into logger_stream
 */
void print_metadata( stack* stk )
{
    #ifndef NO_DEBUG
    assert( stk );
    printlg( "<%s> [ %p ] created by %s in %s:%d\n",
             stk->varname + 1, stk, stk->function, stk->filename, stk->line );

    #endif
}

/**
 * @brief Prints stack status ( if's value ) into logger_stream
 */
void stack_status( stack* stk )
{
    assert( stk );


    printlg( "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    printlg( "hash == 0x%X\n", stk->hash );
    printlg( "pyrrhuloxia1 == 0x%X\n\n", stk->pyrrhuloxia1 );
    printlg( "size == %d\n",     stk->size     );
    printlg( "capacity == %d\n", stk->capacity );
    if ( stk->data )
        printlg( "data == %p\n", stk->data     );
    else
        printlg( "data == NULL\n" );

    int sizetemp = ( stk->size < 0 ) ? 0 : stk->size;
    int captemp = ( stk->capacity < 0 ) ? 0 : stk->capacity;

    int min = sizetemp;
    if ( min > captemp )
         min = captemp;

    if ( stk->data )
    {

        printlg( "\n[ CAN ] == 0x%X ( CANARY )\n", *( (uint64_t* ) stk->data ) );
        for ( int i = 0; i < stk->capacity && i < stk->size - 1; i++ )
        {
            printlg( "\n*-> [ %2d ] == ", i );
            UniPrint( ( stack_data* ) ( ( char* ) stk->data + sizeof( uint64_t ) + i * sizeof( stack_data ) ) );
        }
        if ( stk->capacity > stk->size - 1 )
            for ( int i = stk->size - 1; i < stk->capacity; i++ )
            {
                printlg( "\n    [ %2d ] == ", i );
                UniPrint( ( stack_data* ) ( ( char* ) stk->data + sizeof( uint64_t ) + i * sizeof( stack_data ) ) );
                printlg( "( FARFETCH )" );
            }
        else
            printlg( "\nDimensions fault, can't determine correct area of printing\n ");

        uint64_t ins_canary = *( ( uint64_t* ) stk->data + 1 + ( stk->capacity * sizeof( stack_data ) + 7 ) / 8 );
        printlg( "\n\n[ CAN ] == 0x%X (CANARY)\n", ins_canary );

    }

    printlg( "\npyrrhuloxia2 == 0x%X\n", stk->pyrrhuloxia2 );

    // for ( int i = 0; i < 3; i++ )
    // {
    //     if ( stk->calls.processes[ i ] == Initialization )
    //     {
    //         printlg( "%d) Initialization\n" );
    //     }
    //     else if ( stk->calls.processes[ i ] == Pushing )
    //     {
    //         printlg( "%d) Pushing with " );
    //         UniPrint( stk->calls.values[ i ] );
    //         printlg( "\n" );
    //     }
    //     else if ( stk->calls.processes[ i ] == Popping )
    //     {
    //         printlg( "%d) Popping by : " );
    //         UniPrint( stk->calls.values[ i ] );
    //         printlg( "\n" );
    //     }
    // }


    printlg( "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
}

/**
 * @brief Debugging puts function
 */
void putslg_debug( const char* a )
{

    while ( *a != '\n' && *a )

        switch ( *a )
        {
            case '\n':
                printlg ( "\\n" );
                a++;
                break;

            case '\a':
                printlg( "\\a" );
                a++;
                break;

            case '\t':
                printlg( "\\t" );
                a++;
                break;

            case '\b':
                printlg( "\\b" );
                a++;
                break;

            case '\r':
                printlg( "\\r" );
                a++;
                break;

            default:
                printlg ( "%s", *( a++ ) );
                break;
        };
}
