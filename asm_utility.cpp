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

    for ( int i = 0; i < spu->PC; i++ )
        if( spu->assembler[ i ] == '\n' )
            count++;

    return count;
}

void
move_PC( SPU* spu )
{
    assert( spu );

    while ( spu->assembler[ spu->PC ] != ' ' && spu->assembler[ spu->PC ] != '\n' &&
            spu->assembler[ spu->PC ] != '\0' )
        spu->PC++;

    while( spu->assembler[ spu->PC ] == ' ' || spu->assembler[ spu->PC ] == '\n' )
        spu->PC++;
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

    if ( !spu->assembler_desc || !spu->assembler || spu->PC < 0 || !spu->byte_code_desc )
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
    spu->PC = 0;
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
    spu->PC = 0;
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
             "PC = %zd\n"
             "code_size = %zd\n"
             "assembler_size = %zd\n"
             "byte_code_desc = %d\n"
             "assembler_desc = %d\n"
             "assembler = 0x%X\n"
             "byte_code_file = 0x%X\n"
             "status = %d\n\n",
             spu->byte_code,       spu->PC,        spu->code_size,      spu->assembler_size, spu->byte_code_desc,
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

StkError
SPUrun_init( SPU* spu )
{
    assert( spu );

    if ( spu->status != Success )
        return Failure;

    stack_init( &spu->stk, 4 );

    spu->code_size = Getsize( "code/byte_code.txt" );

    spu->byte_code_desc = open( "code/byte_code.txt", O_RDONLY );
    spu->byte_code = ( int64_t* ) mmap( 0, spu->code_size, PROT_READ, MAP_SHARED, spu->byte_code_desc, 0 );


    if ( spu->assembler_desc == -1 || spu->assembler == MAP_FAILED )
    {
        printlg(         "Failed to open <code/byte_code.txt> file in spu initialization\n" );
        fprintf( stderr, "Failed to open <code/byte_code.txt> file in spu initialization\n" );
        spu->status = Failure;
        return Failure;
    }

    spu->PC = 0;
    spu->status = Success;

    return Success;
}

void
SPUrun_exit( SPU* spu )
{
    assert( spu );

    if ( spu->status != Success )
        return;

    munmap( spu->byte_code, spu->code_size );
    close( spu->byte_code_desc );
    stack_destr( &spu->stk );

    spu->byte_code_desc = 0;
    spu->code_size =      0;
    spu->code_size =      0;
    spu->status =   Success;
    spu->PC =             0;

}
