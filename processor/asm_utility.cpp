#include <cstddef>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <assert.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>

#include "../Stack/utility//log.h"
#include "../Stack/utility/errors.h"
#include "asm_uitility.h"

#define ASM_FARFETCH LONG_MAX

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
    short sign = 1;

    if ( *text == '-' )
    {
        text++;
        sign = -1;
    }

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

    *variable *= sign;

    return status;
}


StkError
mscan_long( char* text, int64_t* variable )
{
    assert( text );
    assert( variable );

    StkError status = Failure;

        while( '0' <= *text && *text <= '9' )
    {
        *variable = *( variable ) * 10 + *text - '0';
        text++;
        status = Success;
    }

    return status;
}


void
labels_exit( labels* lab )
{
    assert( lab );

    lab->quantity = 0;

    free( lab->pc     );
    free( lab->jmp_pc );
    free( lab->name   );
    lab->capacity = 0;

}


StkError
labels_init( labels* lab )
{
    assert( lab );

    lab->capacity = 4;
    lab->quantity = 0;
    lab->pc =     ( size_t* ) calloc( lab->capacity, sizeof( size_t ) );
    lab->name =   ( char**  ) calloc( lab->capacity, sizeof( char*  ) );
    lab->jmp_pc = ( size_t* ) calloc( lab->capacity, sizeof( size_t ) );

    if ( !lab->pc || !lab->name || !lab->jmp_pc )
    {
        fprintf( stderr, "Assembling failure: can't allocate labels variable.\n" );
        printlg( "Assembling failure: can't allocate labels variable.\n"
                 "In <%s> file, <%s> func, %d line\n\n", __FILE__, __FUNCTION__, __LINE__ );
        return Failure;
    }
    return Success;
}


size_t
calc_offset( stream* assembler, size_t* PC )
{
    assert( assembler );
    assert( PC );

    size_t count = 0;
    int j = 0;

    for ( int i = 0; i < *PC; i++ )
    {
        j = 0;
        while( assembler->virt_mem[ j++ ] != '\n' )
            count++;
        count++;
    }

    return count;
}


void
move_offset( stream* assembler, size_t* offset, size_t* line )
{
    assert( assembler );
    assert( offset );
    assert( line );

    while ( assembler->virt_mem[ *offset ] != ' ' && assembler->virt_mem[ *offset ] != '\n' &&
            assembler->virt_mem[ *offset ] != '\0' )
        ( *offset )++;

    while ( assembler->virt_mem[ *offset ] == ' ' || assembler->virt_mem[ *offset ] == '\n' )
    {
        if ( assembler->virt_mem[ *offset ] == '\n' )
            ( *line )++;

        ( *offset )++;
    }
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


stream
asm_open( char* name )
{
    assert( name );

    stream assembler = {};

    strcpy( assembler.name, name );

    assembler.desc = open( name, O_RDONLY );
    assembler.virt_mem = ( char* ) mmap( 0, Getsize( assembler.name ), PROT_READ, MAP_SHARED, assembler.desc, 0 );


    if ( assembler.desc == -1 || assembler.virt_mem == MAP_FAILED )
    {
        printlg(         "Failed to open <%s> file in spu initialization\n", assembler.name );
        fprintf( stderr, "Failed to open <%s> file in spu initialization\n", assembler.name );
        return assembler;
    }

    assembler.comm_count = 1;
    assembler.size = Getsize( assembler.name );

    #ifdef ASM_DEBUG

    for ( int i = 0; i < assembler.size; i++ )
        if ( assembler.virt_mem[ i ] == '\n' )
            assembler.comm_count++;

    #endif

    return assembler;
}

void
spu_dump( SPU* spu )//TODO enhance dump
{
    assert( spu );

    printlg( "---| Current SPU values |---\n\n"
             "byte_code = 0x%X\n"
             "PC = %zd\n"
             "code_size = %zd\n"
             "byte_code_desc = %d\n"
             "assembler = 0x%X\n"
             "status = %d\n\n",
             spu->byte_code,       spu->PC,             spu->byte_code.size,
             spu->byte_code.desc,  spu->byte_code.desc, spu->status         );

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

    spu->byte_code.size = Getsize( "code/byte_code.txt" );

    spu->byte_code.desc = open( "code/byte_code.txt", O_RDONLY );
    spu->byte_code.virt_mem = ( char* ) mmap( 0, spu->byte_code.size, PROT_READ, MAP_SHARED, spu->byte_code.desc, 0 );

    stack_init( &spu->ret_val, 4 );

    if ( spu->byte_code.desc == -1 || spu->byte_code.virt_mem == MAP_FAILED )
    {
        printlg(         "Failed to open <code/byte_code.txt> file in spu initialization\n" );
        fprintf( stderr, "Failed to open <code/byte_code.txt> file in spu initialization\n" );
        spu->status = Failure;
        return Failure;
    }

    for ( int i = 0; i < MEMORY_CAPACITY * MEMORY_CAPACITY; i++ )
        spu->memory[ i ] = ASM_FARFETCH;

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

    munmap( spu->byte_code.virt_mem, spu->byte_code.size );
    close( spu->byte_code.desc );
    stack_destr( &spu->stk );
    stack_destr( &spu->ret_val );

    spu->byte_code.desc = 0;
    spu->byte_code.size = 0;
    spu->byte_code.size = 0;
    spu->status =   Success;
    spu->PC =             0;

}


StkError
SPUasm_init( SPU* spu )
{
    assert( spu );

    strcpy( spu->byte_code.name, "code/byte_code.txt" );

    spu->byte_code.desc = open( spu->byte_code.name, O_CREAT | O_WRONLY | O_TRUNC, 0666 );
    spu->byte_code.size = Getsize( spu->byte_code.name );

    if ( spu->byte_code.desc == -1 )
    {
        spu->status = Failure;
        return Failure;
    }

    spu->status = Success;
    return Success;
}


void
SPUasm_exit( SPU* spu )
{
    assert( spu );

    munmap( spu->byte_code.virt_mem, spu->byte_code.size );
    close( spu->byte_code.desc );
}


bool
check_arg ( char* assembler )
{
    assert( assembler );

    while( *assembler != ' ' && *assembler != '\n' && *assembler )
        assembler++;
    while( *assembler == ' ' )
        assembler++;

    return ( *assembler != '\n' ) ? true : false;
}


void
mem_dump ( SPU* spu )
{
    assert( spu );

    printlg( "---| Memory status |---\n\n   " );

    for ( int i = 0; i < MEMORY_CAPACITY; i++ )
        printlg( "[%d]", i );
    printlg( "\n" );

    for ( int i = 0; i < MEMORY_CAPACITY; i++ )
    {
        printlg( "[%d]", i );
        for ( int j = 0; j < MEMORY_CAPACITY; j++ )
            if ( spu->memory[ i * 10 + j ] == ASM_FARFETCH )
                printlg( "   " );
            else
                printlg( "<*>" );

        printlg( "\n" );
    }
    printlg( "-----------------------\n\n" );
}
