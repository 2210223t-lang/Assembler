#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../processor/asm_uitility.h"
#include "../Stack/utility/log.h"
#include "../assembler/arguments.h"



void
print_reg( FILE* ostream, stack_data reg )
{
    assert( ostream );
    assert( reg );

    switch( reg )
    {
        case RAX:
            fprintf( ostream, " RAX" );
            break;
        case RBX:
            fprintf( ostream, " RBX" );
            break;
        case RCX:
            fprintf( ostream, " RCX" );
            break;
        case RDX:
            fprintf( ostream, " RDX" );
            break;
        case REX:
        case RSI:
        case RSP:
        case RBP:
        case RCP:


    }

}


StkError
disassemble( char* code, size_t size, FILE* ostream )
{
    assert( code );
    assert( ostream );

    size_t PC = 0;
    instruct comm = *( instruct* ) ( code + PC );
    stack_data arg1 = 0;
    stack_data arg2 = 0;

    while ( PC < size )
    {
        switch( comm.num )
        {
            case HLT:
                fprintf( ostream, "HLT\n" );
                return Success;

            case PUSH:
                fprintf( ostream, "PUSH" );
                PC += COMM_SIZE;
                arg1 = 0;
                arg2 = 0;
                if ( comm.ram & 1 )
                {
                    fprintf( ostream, " [" );
                    if ( comm.reg & 1 )
                    {
                        memcpy( &arg1, code + PC, ARG_SIZE );
                        PC += ARG_SIZE;
                        print_reg( ostream, arg1 );
                        memcpy( &arg2, code + PC, ARG_SIZE );
                        PC += ARG_SIZE;
                        fprintf( ostream, " * %lld", arg2 );
                    }
                    if ( comm.con & 1 )
                    {
                        memcpy( &arg2, code + PC, ARG_SIZE );
                        fprintf( ostream, " + %lld", arg2 );
                        PC += ARG_SIZE;
                    }
                    fprintf( ostream, " ]\n" );
                }
                else if ( comm.reg & 1 )
                {
                        memcpy( &arg1, code + PC, ARG_SIZE );
                        PC += ARG_SIZE;
                        print_reg( ostream, arg1 );
                        fprintf( ostream, "\n" );
                }
                else
                {
                    memcpy( &arg1, code + PC, ARG_SIZE );
                    fprintf( ostream, " %lld\n", arg1 );
                    PC += ARG_SIZE;
                }
                break;

            case POP:
                fprintf( ostream, "POP" );
                arg1 = 0;
                arg2 = 0;
                if ( comm.ram & 1 )
                {
                    PC += COMM_SIZE;
                    fprintf( ostream, " [" );
                    if ( comm.reg & 1 )
                    {
                        memcpy( &arg1, code + PC, ARG_SIZE );
                        PC += ARG_SIZE;
                        print_reg( ostream, arg1 );
                        memcpy( &arg2, code + PC, ARG_SIZE );
                        PC += ARG_SIZE;
                        fprintf( ostream, " * %lld", arg2 );
                    }
                    if ( comm.con & 1 )
                    {

                        memcpy( &arg2, code + PC, ARG_SIZE );
                        fprintf( ostream, " + %lld", arg2 );
                        PC += ARG_SIZE;
                    }
                    fprintf( ostream, " ]\n" );
                }
                else if ( comm.reg & 1 )
                {
                    PC += COMM_SIZE;
                    memcpy( &arg1, code + PC, ARG_SIZE );
                    PC += ARG_SIZE;
                    print_reg( ostream, arg1 );
                    fprintf( ostream, "\n" );
                }
                else
                {
                    memcpy( &arg1, code + PC, ARG_SIZE );
                    fprintf( ostream, " %lld\n", arg1 );
                    PC += ARG_SIZE;
                }
                break;

            case ADD:
                fprintf( ostream, "ADD\n" );
                PC += COMM_SIZE;
                break;

            case SUB:
                fprintf( ostream, "SUB\n" );
                PC += COMM_SIZE;
                break;

            case DIV:
                fprintf( ostream, "DIV\n" );
                PC += COMM_SIZE;
                break;

            case OUT:
                fprintf( ostream, "OUT\n" );
                PC += COMM_SIZE;
                break;

            case FACT:
                fprintf( ostream, "FACT\n" );
                PC += COMM_SIZE;
                break;

            case MULT:
                fprintf( ostream, "MULT\n" );
                PC += COMM_SIZE;
                break;

            case REST:
                fprintf( ostream, "REST\n" );
                PC += COMM_SIZE;
                break;

            case JMP:
                PC += COMM_SIZE;
                memcpy( &arg1, code + PC, ARG_SIZE );
                fprintf( ostream, "JMP %lld\n", arg1 );
                break;

            case SQR:
                fprintf( ostream, "SQR\n" );
                PC += COMM_SIZE;
                break;

            case CALL:
                PC += COMM_SIZE;
                memcpy( &arg1, code + PC, ARG_SIZE );
                fprintf( ostream, "CALL %lld\n", arg1 );
                PC += ARG_SIZE;
                break;

            case RET:
                fprintf( ostream, "RET\n" );
                break;

            case SQRT:
                fprintf( ostream, "SQRT\n" );
                PC += COMM_SIZE;
                break;

            case JMPN:
                memcpy( &arg1, code + PC, ARG_SIZE );
                fprintf( ostream, "JMPN %lld\n", arg1 );
                PC += COMM_SIZE + ARG_SIZE;
                break;

            case JMPP:
                memcpy( &arg1, code + PC, ARG_SIZE );
                fprintf( ostream, "JMPP %lld\n", arg1 );
                PC += COMM_SIZE + ARG_SIZE;
                break;

            case JMPZ:
                memcpy( &arg1, code + PC, ARG_SIZE );
                fprintf( ostream, "JMPZ %lld\n", arg1 );
                PC += COMM_SIZE + ARG_SIZE;
                break;

            default:
                printlg(         "Disassembler failure: Unknown comm\n"
                                 "comm id: <%d>, PC %d\n", comm, PC );
                fprintf( stderr, "Disassembler failure: Unknown comm\n" );
                return Failure;

    }
}
