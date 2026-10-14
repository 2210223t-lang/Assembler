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
#include "asm_uitility.h"


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
    assert( size );

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

    size_t filesize = Getsize( "code/byte_code.txt" );

    int istream = open( "code/byte_code.txt", O_RDONLY );
    int64_t* code = ( int64_t* ) mmap( 0, filesize, PROT_READ, MAP_SHARED, istream, 0 );

    if ( istream == -1 || code ==  MAP_FAILED )//< Checks opening file with byte code
    {
        $RT
        printlg(         "Failed to open byte-code file in Start_Calc function\n" );
        fprintf( stderr, "Failed to open byte-code file in Start_Calc function\n" );
        return Failure;
    }

    double temp = 0;
    size_t PC = 0;
    int64_t command = code[ PC ];
    int current_line = 0;


    while ( command != HLT )
    {
        command = code[ PC ];
        print_byte_code( code, filesize / sizeof( int64_t ), PC );//< Log - output debugging function

        switch( command )
        {
            case HLT:
                printlg(         "Killing process\n" );
                fprintf( stderr, "Killing process\n" );
                return Success;

            case PUSH:
                PC++;
                push( stk, code[ PC ] );
                PC++;
                current_line++;
                break;

            case POP:
                pop( stk );
                PC++;
                current_line++;
                break;

            case ADD:
                sum( stk );
                PC++;
                current_line++;
                break;

            case SUB:
                sub( stk );
                PC++;
                current_line++;
                break;

            case DIV:
                div( stk );
                PC++;
                current_line++;
                break;

            case OUT:
                out( stk );
                PC++;
                current_line++;
                break;

            case FACT:
                fact( stk );
                PC++;
                current_line++;
                break;

            case MULT:
                mult( stk );
                PC++;
                current_line++;
                break;

            case REST:
                rest( stk );
                PC++;
                current_line++;
                break;

            case JMP:
                jmp( &PC, code[ PC + 1 ] );
                break;

            default:
                printlg(         "Processor failure: Unknown command\n"
                                 "Command id: <%d> in line %d\n", command, current_line );
                fprintf( stderr, "Processor failure: Unknown command\n" );
                return Failure;
        }
    }


    printlg( "Ended running, hope that you're satisfied with the result\n" );
    close( istream );
    munmap( code, filesize );
    return Success;
}
