#include <cstdint>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <unistd.h>

#include "Stack/utility/Utility.h"
#include "Stack/utility/errors.h"
#include "Stack/utility/log.h"
#include "Stack/utility/colours.h"
#include "asm_uitility.h"


void
jmp_proceed( FILE* istream, int ostream, label* Labels )
{
    assert( istream );
    assert( Labels );

    double line = 0;
    char* flag = 0;

    // if ( fscanf( istream, "%lf", &line ) == -1 )
    // {
    //     fscanf( istream, "%s" );
    // }

}


void
print_byn( FILE* list, int byte_code, int64_t command, int64_t value )
{
    assert( list );

    write( byte_code, &command, sizeof ( int64_t ) );
    fprintf( list, "%lld", command );

    if ( command == PUSH || command == JMP )
    {
        fprintf( list, " %lld", value * 1000 );
        write( byte_code, &value, sizeof( int64_t ) );
    }

    fprintf( list, "\n" );
}


SPU
compile( const char filename[ PATH_MAX ], int mode )
{
    FILE* ostream_list = fopen( "code/list_code.txt", "w" );
    double value = 0;

    SPU spu = {};

    SPUcomp_init( filename, &spu );
    if ( spu.status == Failure )
        return spu;

    char* comm =spu.assembler + spu.PC;

    printlg( "---| Starting compilation |---\n\n" );

    while ( my_strcmp( comm, "HLT" ) )
    {
        if ( spu.PC >= spu.assembler_size )
        {
            printlg(         "Assembler failure.\n" );
            fprintf( stderr, "Assembler failure, check log - file.\n" );
            spu.status = Failure;
            return spu;
        }

        comm = spu.assembler + spu.PC;

        if ( !my_strcmp( comm, "PUSH" ) )
        {
            mscan_double( spu.assembler + spu.PC, &value );
            print_byn( ostream_list, spu.byte_code_desc, PUSH, value );
            move_PC( &spu );
        }
        else if ( !my_strcmp( comm, "ADD" ) )
            print_byn( ostream_list, spu.byte_code_desc,  ADD, 0 );
        else if ( !my_strcmp( comm, "DIV" ) )
            print_byn( ostream_list, spu.byte_code_desc,  DIV, 0 );
        else if ( !my_strcmp( comm, "OUT" ) )
            print_byn( ostream_list, spu.byte_code_desc,  OUT, 0 );
        else if ( !my_strcmp( comm, "HLT" ) )
            print_byn( ostream_list, spu.byte_code_desc,  HLT, 0 );
        else if ( !my_strcmp( comm, "MULT" ) )
            print_byn( ostream_list, spu.byte_code_desc, MULT, 0 );
        else if ( !my_strcmp( comm, "JMP" ) )
            print_byn( ostream_list, spu.byte_code_desc, SQR, 0 );
        else if ( !my_strcmp( comm, "JMP" ) )
        {
            move_PC( &spu );
            mscan_double( spu.assembler + spu.PC, &value );
            print_byn( ostream_list, spu.byte_code_desc, JMP, value );
        }
        else
        {
            spu.status = Failure;

            printlg( "Compiling failure: unknown command: <" );
            my_printlg( comm );
            printlg( "> in %d line\n", count_lines( &spu ) );
            spu_dump( &spu );
        }
        move_PC( &spu ); //Moves position to the next argument
    }

    if ( spu.status != Success )
        fprintf( stderr, "Compiling failure, check log - file\n" );
    printlg( "---| End of compilation|---\n\n" );

    return spu;
}
