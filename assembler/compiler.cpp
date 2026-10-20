#include <cstdint>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "../Stack/utility/errors.h"
#include "../Stack/utility/log.h"
#include "../processor/asm_uitility.h"
#include "arguments.h"
#include "../Stack/utility/colours.h"



void
unk_com( char* comm, stream* assembler, size_t* line )
{
    assert( comm );
    assert( assembler );
    assert( line );

    printlg( "Compiling failure: unknown command: <" );
    my_printlg( comm );
    printlg( "> in %d line\n", calc_offset( assembler, line ) );
}


void
print_byn( FILE* list, int byte_code_desc, instruct* command, double value, char* arg )
{
    assert( list );
    instruct* comm = ( instruct* ) command;
    int64_t valuei = 0;

    write( byte_code_desc, command, sizeof ( instruct ) );
    fprintf( list, "%d ",  *( unsigned char* ) comm );

    if ( ( comm->num == PUSH || comm->num == POP ) && comm->reg & 1 )
    {

        fprintf( list, " %lf", value );
        valuei = value;
        write( byte_code_desc, &valuei, sizeof( int64_t ) );
    }
    else if ( comm->num == PUSH )
    {
        valuei = value * 1000;
        fprintf( list, " %lf", value );
        write( byte_code_desc, &valuei , sizeof( int64_t ) );
    }

    fprintf( list, "\n" );
}


SPU
assemble( char filename[ PATH_MAX ] )
{
    assert( filename );

    //------------Init start--------------
    double value = 0;
    size_t offset = 0;
    size_t line = 0;
    size_t PC = 0;
    SPU spu = {};
    labels lab = {};
    int64_t temp = 0;
    instruct comm = {};

    stream assembler =  asm_open( filename );
    FILE* ostream_list = fopen( "code/list_code.txt", "w" );

    if ( assembler.desc == -1 || assembler.virt_mem == MAP_FAILED )
        return { .status = Failure };

    SPUasm_init( &spu );

    if ( spu.status == Failure )
        return spu;

    if ( labels_init( &lab ) == Failure )
        return { .status = Failure };

    char* comm_point = assembler.virt_mem;
    //-----------End of init-------------

    printlg( "---| Starting compilation |---\n\n" );

    while ( *comm_point && offset < assembler.size )
    {
        if ( offset >= assembler.size )
        {
            printlg(         "Assembler failure. offset > assembler size\n" );
            fprintf( stderr, "Assembler failure, check log - file.\n" );
            return { .status = Failure };
        }

        comm_point = assembler.virt_mem + offset;

        if ( !my_strcmp( comm_point, "PUSH" ) )
        {
             move_offset( &assembler, &offset, &line );

            comm = { .num = PUSH };
            if ( *( assembler.virt_mem + offset ) == '[' )
            {
                comm.ram = true;
                if ( mem_proceed( &assembler, &offset, line, ostream_list, spu.byte_code.desc, &comm, &PC ) == Failure )
                    spu.status = Failure;
            }
            else if ( mscan_double( assembler.virt_mem + offset, &value ) == Success )
            {
                comm.con = true;
                PC += ARG_SIZE;
                print_byn( ostream_list, spu.byte_code.desc, &comm, value, assembler.virt_mem + offset );
            }
            else
            {
                comm.reg = true;
                value = get_register( assembler.virt_mem + offset, line );
                if ( value == INCORRECT_REGISTER )
                    spu.status = Failure;
                print_byn( ostream_list, spu.byte_code.desc, &comm, value, assembler.virt_mem + offset );
                PC += ARG_SIZE;
            }

        }
        else if ( !my_strcmp( comm_point, "ADD" ) )
        {
            comm = { .num = ADD };
            print_byn( ostream_list, spu.byte_code.desc, &comm, 0, 0 );
        }
        else if ( !my_strcmp( comm_point, "DIV" ) )
        {
            comm = { .num = DIV };
            print_byn( ostream_list, spu.byte_code.desc, &comm, 0, 0 );
        }
        else if ( !my_strcmp( comm_point, "OUT" ) )
        {
            comm = { .num = OUT };
            print_byn( ostream_list, spu.byte_code.desc, &comm, 0, 0 );
        }
        else if ( !my_strcmp( comm_point, "HLT" ) )
        {
            comm = { .num = HLT };
            print_byn( ostream_list, spu.byte_code.desc, &comm, 0, 0 );
        }
        else if ( !my_strcmp( comm_point, "MULT" ) )
        {
            comm = { .num = MULT };
            print_byn( ostream_list, spu.byte_code.desc, &comm, 0, 0 );
        }
        else if ( !my_strcmp( comm_point, "SQR" ) )
        {
            comm = { .num = SQR };
            print_byn( ostream_list, spu.byte_code.desc, &comm, 0, 0 );
        }
        else if ( ( !my_strcmp( comm_point,  "JMP" ) && ( comm = { .num =  JMP } ).num ==  JMP ) ||
                  ( !my_strcmp( comm_point, "JMPP" ) && ( comm = { .num = JMPP } ).num == JMPP ) ||
                  ( !my_strcmp( comm_point, "JMPN" ) && ( comm = { .num = JMPN } ).num == JMPN ) ||
                  ( !my_strcmp( comm_point, "JMPZ" ) && ( comm = { .num = JMPZ } ).num == JMPZ ) )
        {
            PC += ARG_SIZE;
            print_byn( ostream_list, spu.byte_code.desc, &comm, 0, 0 );
            move_offset( &assembler, &offset, &line );
            jmp_proceed( ostream_list, &spu, &PC, &offset, &assembler, &line, &lab );
        }
        else if ( !my_strcmp( comm_point, "RET" ) )
        {
            comm = { .num = RET };
            print_byn( ostream_list, spu.byte_code.desc, &comm, 0, 0 );
        }
        else if ( !my_strcmp( comm_point, "CALL" ) )
        {
            comm = { .num = CALL };
            PC += ARG_SIZE;
            print_byn( ostream_list, spu.byte_code.desc, &comm, 0, 0 );
            move_offset( &assembler, &offset, &line );
            jmp_proceed( ostream_list, &spu, &PC, &offset, &assembler, &line, &lab );
        }
        else if ( !my_strcmp( comm_point, "SQRT" ) )
        {
            comm = { .num = SQRT };
            print_byn( ostream_list, spu.byte_code.desc, &comm, 0, 0 );
        }
        else if ( !my_strcmp( comm_point, "SUB" ) )
        {
            comm = { .num = SUB };
            print_byn( ostream_list, spu.byte_code.desc, &comm, 0, 0 );
        }
        else if ( !my_strcmp( comm_point, "POP" ) )
        {
            comm = { .num = POP };
            if ( check_arg( assembler.virt_mem + offset ) )
            {
                move_offset( &assembler, &offset, &line );

                if ( assembler.virt_mem[ offset ] == '[' )
                {
                    comm.ram = true;
                    if ( mem_proceed( &assembler, &offset, line, ostream_list, spu.byte_code.desc, &comm, &PC ) == Failure )
                        spu.status = Failure;
                }
                else
                {
                    value = get_register( assembler.virt_mem + offset, line );
                    PC += ARG_SIZE;
                    if ( value == INCORRECT_REGISTER )
                        spu.status = Failure;
                    else
                        comm.reg = true;
                    print_byn( ostream_list, spu.byte_code.desc, &comm, value, assembler.virt_mem + offset );
                }
            }
            else
                print_byn( ostream_list, spu.byte_code.desc, &comm, value, assembler.virt_mem + offset );

        }
        else if ( comm_point[ 0 ] == ':' )
        {
            if ( add_tag( spu.byte_code.desc, comm_point, PC, &lab, line ) == Failure )
                return { .status = Failure };
        }
        else
        {
            spu.status = Failure;
            unk_com( comm_point, &assembler, &line );
        }
        if ( *comm_point != ':' )
            PC += COMM_SIZE;

        move_offset( &assembler, &offset, &line ); //Moves position to the next argument
    }

    for ( int i = 0; i < lab.quantity; i++ )
        if ( lab.pc[ i ] == DONT_EXIST )
        {
            printlg( "Assembling failure, label value unresolved.\n"
                     "Label <");
            my_printlg( lab.name[ i ] );
            printlg( ">, defined pc %d\n\n", lab.jmp_pc[ i ] );
            spu.status = Failure;
        }

    if ( spu.status != Success )
        fprintf( stderr, "Compiling failure, check log - file\n" );
    printlg( "---| End of compilation|---\n\n" );

    SPUasm_exit( &spu );
    labels_exit( &lab );
    return spu;
}
