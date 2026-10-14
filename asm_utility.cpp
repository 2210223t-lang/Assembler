#include <cstddef>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <assert.h>
#include <unistd.h>
#include <sys/mman.h>

#include "Stack/utility//log.h"
#include "Stack/utility/Utility.h"
#include "Stack/utility/errors.h"
#include "asm_uitility.h"


uint64_t
Getsize( const char* filename )
{
    struct stat a = {};

    stat( filename, &a );

    return a.st_size;
}

StkError
mscan_double( char* text, double* variable )
{
    assert( variable );
    StkError status = Failure;

    *variable = 0;

    while( '0' <= *text && *text <= '9' )
    {
        *variable = *( variable ) * 10 + *text - '0';
        text++;
        status = Success;
    }

    int ten_pow = 10;
    if ( *( text++ ) == '.' )
        while( '0' <= *text && *text <= '9' && ten_pow <= 10000 )
        {
            *variable = *( variable ) + ( double ) ( *text - '0' ) / ten_pow;
            text++;
            ten_pow *= 10;
            status = Success;
        }

    return status;
}


size_t
count_lines( SPU* spu )
{
    assert( spu );

    size_t count = 0;

    for ( int i = 0; i < spu->position; i++ )
        if( spu->assembler[ i ] == '\n' )
            count++;

    return count;
}

void
move_position( SPU* spu )
{
    assert( spu );

    while ( spu->assembler[ spu->position ] != ' ' && spu->assembler[ spu->position ] != '\n' &&
            spu->assembler[ spu->position ] != '\0' )
        spu->position++;

    while( spu->assembler[ spu->position ] == ' ' || spu->assembler[ spu->position ] == '\n' )
        spu->position++;
}

int
my_strcmp( const char* a, const char* b )
{
    assert( a );
    assert( b );

    while ( *a != '\n' && *a != ' ' && *a &&
            *b != '\n' && *b != ' ' && *b )
    {
        if ( *a != *b )
            return *a - *b;
        a++;
        b++;
    }

    if ( ( *a == '\n' || *a == ' ' || !( *a ) ) &&
         ( *b == '\n' || *b == ' ' || !( *b ) ) )
        return 0;

    return *a - *b;
}

StkError
comp_exit( SPU* spu )
{
    assert( spu );

    if ( !spu->assembler_desc || !spu->assembler || spu->position < 0 || !spu->byte_code_desc )
    {
        printlg(         "Attempt to proceed comp_exit with incorrect dimensions.\n" );
        spu_dump( spu );
        fprintf( stderr, "Attempt to proceed comp_exit with incorrect dimensions.\n Check log-file\n" );
        spu->status = Failure;
        return Failure;
    }
    close( spu->assembler_desc );
    munmap( spu->assembler, spu->assembler_size );
    spu->assembler_size = 0;
    spu->position = 0;
    close( spu->byte_code_desc );
    spu->byte_code_desc = 0;
    spu->assembler_desc = 0;

    return Success;
}

StkError
SPUcomp_init( const char filename[ PATH_MAX ], SPU* spu )
{
    assert( spu );


    spu->assembler_desc = open( filename, O_RDONLY );
    spu->assembler = ( char* ) mmap( 0, Getsize( filename ), PROT_READ, MAP_SHARED, spu->assembler_desc, 0 );


    if ( spu->assembler_desc == -1 || spu->assembler == MAP_FAILED )
    {
        printlg(         "Failed to open <%s> file in spu initialization\n", filename );
        fprintf( stderr, "Failed to open <%s> file in spu initialization\n", filename );
        spu->status = Failure;
        return Failure;
    }

    spu->assembler_size = Getsize( filename );
    spu->position = 0;
    spu->byte_code_desc = open( "code/byte_code.txt", O_WRONLY | O_CREAT );

    if ( spu->byte_code_desc == -1 )
    {
        printlg(         "Failed to open <code/byte_code.txt> in spu initialization func.\n Line: %d\n", __LINE__ );
        fprintf( stderr, "Failed to open <code/byte_code.txt> in spu initialization func.\n" );
        spu->status = Failure;
        return Failure;
    }
    spu->status = Success;
    return Success;
}

void
spu_dump( SPU* spu )
{
    assert( spu );

    printlg( "---| Current SPU values |---\n\n"
             "byte_code = 0x%X\n"
             "position = %zd\n"
             "code_size = %zd\n"
             "assembler_size = %zd\n"
             "byte_code_desc = %d\n"
             "assembler_desc %d\n"
             "assembler = 0x%X\n"
             "byte_code_file = 0x%X\n"
             "status = %d\n\n",
             spu->byte_code,       spu->position,  spu->code_size,      spu->assembler_size, spu->byte_code_desc,
             spu-> assembler_desc, spu->assembler, spu->byte_code_desc, spu->byte_code_file, spu->status );

    printlg( "----------------------------\n\n" );

}

void
my_printlg( char* var )
{
    assert( var );

    while ( *var && *var != '\n' && *var != '\0' && *var != ' ' )
        printlg( "%c", *( var++ ) );
}
