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
#include "../Stack/utility/colours.h"
#include "../processor/asm_uitility.h"
#include "arguments.h"


void
unk_com( char* comm, stream* assembler, size_t* PC )
{
    assert( comm );
    assert( assembler );
    assert( PC );

    printlg( "Compiling failure: unknown command: <" );
    my_printlg( comm );
    printlg( "> in %d line\n", calc_offset( assembler, PC ) );
}


void
print_byn( FILE* list, int byte_code_desc, int64_t command, int64_t value, char* PC )
{
    assert( list );

    write( byte_code_desc, &command, sizeof ( int64_t ) );
    fprintf( list, "%lld ", command );

    if ( ( command == PUSH || command == JMP ) && *PC != ':' )
    {
        fprintf( list, " %lld", value * 1000 );
        write( byte_code_desc, &value, sizeof( int64_t ) );
    }
    else if ( ( command == PUSH ) || command == JMP )
    {
        int count = 0;
        while( *PC != ' ' && *PC != '\n' && *PC != '\0' )
        {
            count++;
            putc( *PC, list );
            PC++;
        }
        write( byte_code_desc, PC - count, sizeof( char ) * count );
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
    size_t PC = 0;
    SPU spu = {};
    labels lab = {};

    stream assembler =  asm_open( filename );
    FILE* ostream_list = fopen( "code/list_code.txt", "w" );

    if ( assembler.desc == -1 || assembler.virt_mem == MAP_FAILED )
        return { .status = Failure };

    SPUasm_init( &spu );

    if ( spu.status == Failure )
        return spu;

    if ( labels_init( &lab ) == Failure )
        return { .status = Failure };

    char* comm =assembler.virt_mem;

    //-----------End of init-------------

    printlg( "---| Starting compilation |---\n\n" );

    while ( my_strcmp( comm, "HLT" ) )
    {
        if ( offset >= assembler.size )
        {
            printlg(         "Assembler failure. offset > assembler size\n" );
            fprintf( stderr, "Assembler failure, check log - file.\n" );
            return { .status = Failure };
        }

        comm = assembler.virt_mem + PC;

        if ( !my_strcmp( comm, "PUSH" ) )
        {
             move_offset( &assembler, &PC, &offset );
            // if ( mscan_double( assembler.virt_mem + offset, &value ) == Failure )
                // spu.status = check_register( &spu );//TODO add registers

            print_byn( ostream_list, spu.byte_code.desc, PUSH, value, assembler.virt_mem + spu.PC );
        }
        else if ( !my_strcmp( comm, "ADD" ) )
            print_byn( ostream_list, spu.byte_code.desc,  ADD, 0, 0 );
        else if ( !my_strcmp( comm, "DIV" ) )
            print_byn( ostream_list, spu.byte_code.desc,  DIV, 0, 0 );
        else if ( !my_strcmp( comm, "OUT" ) )
            print_byn( ostream_list, spu.byte_code.desc,  OUT, 0, 0 );
        else if ( !my_strcmp( comm, "HLT" ) )
            print_byn( ostream_list, spu.byte_code.desc,  HLT, 0, 0 );
        else if ( !my_strcmp( comm, "MULT" ) )
            print_byn( ostream_list, spu.byte_code.desc, MULT, 0, 0 );
        else if ( !my_strcmp( comm, "SQR" ) )
            print_byn( ostream_list, spu.byte_code.desc,  SQR, 0, 0 );
        else if ( !my_strcmp( comm, "JMP" ) )
        {
            move_offset( &assembler, &PC, &offset );
            if ( jmp_parse( &assembler, offset, &lab, PC ) == ERROR )
                unk_com( assembler.virt_mem + offset, &assembler, &PC );
        }
        else if ( comm[ 1 ] == ':' )//TODO add labels init
        {


        }
        else
        {
            spu.status = Failure;
            unk_com( comm, &assembler, &PC );
        }
        move_offset( &assembler, &PC, &offset ); //Moves position to the next argument
    }

    if ( spu.status != Success )
        fprintf( stderr, "Compiling failure, check log - file\n" );
    printlg( "---| End of compilation|---\n\n" );

    return spu;
}
