#include <cstdint>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <getopt.h>
#include <limits.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "../Stack/utility/errors.h"
#include "../Stack/utility/log.h"
#include "commands.h"
#include "calc.h"



void
Terminal( int argc, char** argv, char input[ PATH_MAX ] )
{
    assert( argv );
    assert( input );

    struct option long_options[] = { {  "input", required_argument, 0, 'i' },
                                     {        0,                 0, 0,  0  } };

    int check = 0;
    bool keepgoing = true;

    while ( keepgoing && ( check = getopt_long_only( argc, argv, "io", long_options, NULL ) ) != -1 )
    {

        switch ( check )
        {
            case 'i':
                strcpy( input, optarg );
                break;

            case '?':
                printf( "Incorrect parsing flag in getopt\n" );
                keepgoing = false;

            default:
                fprintf( stderr, "Getopt failure\n" );
                keepgoing = false;
        }
    }
    if ( keepgoing )
        printlg( "Ended parsing command line\nInput filename - <%s>\n\n", input );
}


void
print_byte_code( char* code, int64_t size, int64_t current_command )
{
    assert( code );

    printlg( "---| Code status |---\n\n" );

    for ( int i = 0; i * 8 < size; i++ )
        if ( ( int64_t ) code[ i * 8 ] == PUSH || ( int64_t ) code[ i * 8 ] == JMP )
        {
            printlg( " ____________ " );
            i++;
        }
        else
            printlg( "       " );

    printlg( "\n" );

    for ( int i = 0; i * 8 < size; i++ )
        printlg( "[ %3lld ]", ( int64_t ) code[ i * 8 ] );

    printlg( "\n" );

    for ( int i = 0; i < size; i++ )
        if ( i * 8 == current_command )
            printlg( "  ^^^  " );
        else
            printlg( "       " );

    printlg( "\n---------------------\n\n" );
}


int
Start_Calc( SPU* spu )
{
    assert( spu );

    if ( spu->status != Success )//< Checks if compilation passed successfully
    {
        printlg(         "\nCompilation error, avoiding calculation of invalid code\n" );
        fprintf( stderr, "\nCompilation error, avoiding calculation of invalid code\n" );
        return spu->status;
    }

    SPUrun_init( spu );

    double temp = 0;
    char* code = spu->byte_code.virt_mem;
    int64_t command = 1;


    while ( command != HLT )
    {
        memcpy( &command, code + spu->PC, sizeof( int64_t ) );
        /// Log - output debugging function
        print_byte_code( code, spu->byte_code.size, spu->PC );

        switch( command )
        {
            case HLT:
                printlg(         "Killing process\n" );
                fprintf( stderr, "Killing process\n" );
                return Success;

            case PUSH:
                spu->PC += 8;
                push( &spu->stk, ( stack_data* ) ( code + spu->PC ) );
                spu->PC += 8;
                break;

            case POP:
                pop( &spu->stk );
                spu->PC += 8;
                break;

            case ADD:
                sum( &spu->stk );
                spu->PC += 8;
                break;

            case SUB:
                sub( &spu->stk );
                spu->PC += 8;
                break;

            case DIV:
                div( &spu->stk );
                spu->PC += 8;
                break;

            case OUT:
                out( &spu->stk );
                spu->PC += 8;
                break;

            case FACT:
                fact( &spu->stk );
                spu->PC += 8;
                break;

            case MULT:
                mult( &spu->stk );
                spu->PC += 8;
                break;

            case REST:
                rest( &spu->stk );
                spu->PC += 8;
                break;

            case JMP:
                jmp( &( spu->PC ), ( int64_t* ) ( code + spu->PC + 8 ) );
                break;

            case SQR:
                sqr( &spu->stk );
                spu->PC += 8;
                break;

            default:
                printlg(         "Processor failure: Unknown command\n"
                                 "Command id: <%d>, PC %d\n", command, spu->PC );
                fprintf( stderr, "Processor failure: Unknown command\n" );
                return Failure;
        }
    }


    printlg( "Ended running, hope that you're satisfied with the result\n" );
    SPUrun_exit( spu );
    return Success;
}
