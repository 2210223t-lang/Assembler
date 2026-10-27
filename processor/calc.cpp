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
#include "asm_uitility.h"
#include "commands.h"
#include "calc.h"
#include "../assembler/arguments.h"



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
print_byte_code( instruct* code, size_t size, size_t current_command )
{
    assert( code );

    printlg( "---| Code status |---\n\n" );

    printlg( "Size: %zd bytes\n"
             "Current_command: %zd\n", size, current_command );

    for ( int i = 0; i < size; i++ )
        if ( code[ i ].num == PUSH || code[ i ].num == JMP )
        {
            printlg( " ____________ " );
            i += ARG_SIZE;
        }
        else
            printlg( "      " );

    printlg( "\n" );

    for ( int i = 0; i < size; i++ )
        if ( code[ i ].num == PUSH || code[ i ].num == JMP )
        {
            printlg( "[ %2d ][ %4d ]", *( ( char* ) code + i ), *( int64_t* ) ( code + 1 + i ) );
            i += ARG_SIZE;
        }
        else
            printlg( "[ %2d ]", *( ( char* ) code + i ) );

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

    int64_t arg1 = 0;
    int64_t arg2 = 0;
    instruct* code = ( instruct* )spu->byte_code.virt_mem;
    instruct command = { .num = 1 };


    while ( command.num != HLT )
    {
        memcpy( &command, code + spu->PC, COMM_SIZE );
        /// Log - output debugging function
        print_byte_code( code, spu->byte_code.size, spu->PC );

        switch( command.num )
        {
            case HLT:
                printlg(         "Killing process\n" );
                fprintf( stderr, "Killing process\n" );
                return Success;

            case PUSH:
                spu->PC += COMM_SIZE;
                arg1 = 0;
                arg2 = 0;
                if ( command.ram & 1 )
                {
                    if ( command.reg & 1 )
                    {
                        memcpy( &arg1, code + spu->PC, ARG_SIZE );
                        spu->PC += ARG_SIZE;
                        arg1 = spu->registers[ arg1 ] / 1000;
                        memcpy( &arg2, code + spu->PC, ARG_SIZE );
                        spu->PC += ARG_SIZE;
                        arg1 *= arg2;
                    }
                    if ( command.con & 1 )
                    {
                        memcpy( &arg2, code + spu->PC, ARG_SIZE );
                        arg1 += arg2;
                        spu->PC += ARG_SIZE;
                        }
                    if ( arg1 > 0 && arg1 < MEMORY_CAPACITY * MEMORY_CAPACITY )
                        push( &spu->stk, spu->memory + arg1, &spu->past_proc_sign );
                    else
                    {
                        printlg( "Memory overflow.\n"
                                "Attempt to allocate element with adress %uld\n\n", arg1 );
                        return Failure;
                    }
                    mem_dump( spu );
                }
                else if ( command.reg & 1 )
                {
                    push( &spu->stk, spu->registers + *( ( stack_data* ) ( code + spu->PC ) ), &spu->past_proc_sign );
                    spu->PC += ARG_SIZE;
                }
                else
                {
                    push( &spu->stk, ( stack_data* ) ( code + spu->PC ), &spu->past_proc_sign );
                    spu->PC += ARG_SIZE;
                }
                break;

            case POP:
                spu->PC += COMM_SIZE;
                arg1 = 0;
                arg2 = 0;

                if ( command.ram & 1 )
                {
                    if ( command.reg & 1 )
                    {
                        memcpy( &arg1, code + spu->PC, ARG_SIZE );
                        spu->PC += ARG_SIZE;
                        arg1 = spu->registers[ arg1 ] / 1000;
                        memcpy( &arg2, code + spu->PC, ARG_SIZE );
                        spu->PC += ARG_SIZE;
                        arg1 *= arg2;
                    }
                    if ( command.con & 1 )
                    {
                        memcpy( &arg2, code + spu->PC, ARG_SIZE );
                        arg1 += arg2;
                        spu->PC += ARG_SIZE;
                    }
                    if ( arg1 > 0 && arg1 < MEMORY_CAPACITY * MEMORY_CAPACITY )
                        pop( &spu->stk, spu->memory + arg1  );
                    else
                    {
                        printlg( "Memory overflow.\n"
                                "Attempt to allocate element with adress %uld\n\n", arg1 );
                        return Failure;
                    }
                    mem_dump( spu );
                }
                else if ( command.reg & 1 )
                {
                    pop( &spu->stk, spu->registers + *( ( stack_data* ) ( code + spu->PC ) ) );
                    spu->PC += ARG_SIZE;
                }
                else
                    pop( &spu->stk, 0 );
                break;

            case ADD:
                sum( &spu->stk, &spu->past_proc_sign );
                spu->PC += COMM_SIZE;
                break;

            case SUB:
                sub( &spu->stk, &spu->past_proc_sign );
                spu->PC += COMM_SIZE;
                break;

            case DIV:
                div( &spu->stk, &spu->past_proc_sign );
                spu->PC += COMM_SIZE;
                break;

            case OUT:
                out( &spu->stk );
                spu->PC += COMM_SIZE;
                break;

            case FACT:
                fact( &spu->stk );
                spu->PC += COMM_SIZE;
                break;

            case MULT:
                mult( &spu->stk, &spu->past_proc_sign );
                spu->PC += COMM_SIZE;
                break;

            case REST:
                rest( &spu->stk );
                spu->PC += COMM_SIZE;
                break;

            case JMP:
                jmp( &( spu->PC ), ( size_t* ) ( code + spu->PC + 1 ) );
                break;

            case SQR:
                sqr( &spu->stk, &spu->past_proc_sign );
                spu->PC += COMM_SIZE;
                break;

            case CALL:
                spu->PC += COMM_SIZE;
                spu->PC += ARG_SIZE;
                call( spu, *( ( size_t* ) ( code + spu->PC - ARG_SIZE ) ) );
                break;

            case RET:
                ret( spu );
                break;

            case SQRT:
                root( spu );
                spu->PC += COMM_SIZE;
                break;

            case JMPN:
                if ( spu->past_proc_sign == -1 )
                    jmp( &spu->PC, ( size_t* ) ( code + spu->PC + 1 ) );
                else
                    spu->PC += COMM_SIZE + ARG_SIZE;
                break;

            case JMPP:
                if ( spu->past_proc_sign == 1 )
                    jmp( &spu->PC, ( size_t* ) ( code + spu->PC + 1 ) );
                else
                    spu->PC += COMM_SIZE + ARG_SIZE;
                break;

            case JMPZ:
                if ( spu->past_proc_sign == 0 )
                    jmp( &spu->PC, ( size_t* ) ( code + spu->PC + 1 ) );
                else
                    spu->PC += COMM_SIZE + ARG_SIZE;
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
