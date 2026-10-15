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

#include "Stack/utility/errors.h"
#include "Stack/utility/log.h"
#include "commands.h"
#include "Stack/utility/Utility.h"
#include "calc.h"


int
Terminal( int argc, char** argv, char input[ PATH_MAX ] )
{
    assert( argv );
    assert( input );

    struct option long_options[] = { {  "input", required_argument, 0, 'i' },
                                     {        0,                 0, 0,  0  } };

    int check = 0;
    bool keepgoing = true;
    int stat = 0;

    while ( keepgoing && ( check = getopt_long_only( argc, argv, "io", long_options, NULL ) ) != -1 )
    {

        switch ( check )
        {
            case 'i':
                strcpy( input, optarg );
                stat = 1;
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
    return stat;
}

void
print_byte_code( int64_t* code, size_t size, size_t current_command )
{
    assert( code );

    printlg( "---| Code status |---\n\n" );

    for ( int i = 0; i < size; i++ )
        if ( code[ i ] == PUSH )
        {
            printlg( " ____________ " );
            i++;
        }
        else
            printlg( "       " );

    printlg( "\n" );

    for ( int i = 0; i < size; i++ )
        printlg( "[ %3lld ]", code[ i ] );

    printlg( "\n" );

    for ( int i = 0; i < size; i++ )
        if ( i == current_command )
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
    int64_t command = spu->byte_code[ spu->PC ];
    int current_line = 0;


    while ( command != HLT )
    {
        command = spu->byte_code[ spu->PC ];
        print_byte_code( spu->byte_code, spu->code_size / sizeof( int64_t ), spu->PC );//< Log - output debugging function

        switch( command )
        {
            case HLT:
                printlg(         "Killing process\n" );
                fprintf( stderr, "Killing process\n" );
                return Success;

            case PUSH:
                spu->PC++;
                push( &spu->stk, spu->byte_code[ spu->PC ] );
                spu->PC++;
                current_line++;
                break;

            case POP:
                pop( &spu->stk );
                spu->PC++;
                current_line++;
                break;

            case ADD:
                sum( &spu->stk );
                spu->PC++;
                current_line++;
                break;

            case SUB:
                sub( &spu->stk );
                spu->PC++;
                current_line++;
                break;

            case DIV:
                div( &spu->stk );
                spu->PC++;
                current_line++;
                break;

            case OUT:
                out( &spu->stk );
                spu->PC++;
                current_line++;
                break;

            case FACT:
                fact( &spu->stk );
                spu->PC++;
                current_line++;
                break;

            case MULT:
                mult( &spu->stk );
                spu->PC++;
                current_line++;
                break;

            case REST:
                rest( &spu->stk );
                spu->PC++;
                current_line++;
                break;

            case JMP:
                jmp( &( spu->PC ), spu->byte_code[ spu->PC + 1 ] );
                break;

            case SQR:
                sqr( &spu->stk );
                break;

            default:
                printlg(         "Processor failure: Unknown command\n"
                                 "Command id: <%d> in line %d\n", command, current_line );
                fprintf( stderr, "Processor failure: Unknown command\n" );
                return Failure;
        }
    }


    printlg( "Ended running, hope that you're satisfied with the result\n" );
    SPUrun_exit( spu );
    return Success;
}
