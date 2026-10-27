#include <cstddef>
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
#include "../Stack/utility/Utility.h"
#include "../processor/asm_uitility.h"
#include "arguments.h"
#include "compiler.h"



StkError
add_tag( char* byte_code, char* tag, size_t PC, labels* lab, size_t line )
{
    assert( tag );
    assert( lab );

    bool was_init = false;

    printlg( "---| Adding new tag <" );
    my_printlg( tag );
    printlg( "> |---\n\n" );

    printlg( "Destination -->%zd\n\n", PC );

    for ( int i = 0; i < lab->quantity; i++ )
        if ( !my_strcmp( tag, lab->name[ i ] ) && lab->pc[ i ] != DONT_EXIST )
        {
            printlg( "Assembling failure\n."
                     "Attempt to reinitialize label in line: %d\n", line );
            return Failure;
        }
        else if ( lab->pc[ i ] == DONT_EXIST && !my_strcmp( tag, lab->name[ i ] ) )
        {
            if ( lab->jmp_pc[ i ] < 0 )
            {
                printlg( "Incorrect jmp_pc value in <%s> file, <%s> func, %d line.\n"
                         "Value is: %d", __FILE__, __FUNCTION__, __LINE__, lab->jmp_pc[ i ] );
                return Failure;
            }
            lab->pc[ i ] = PC;
            was_init = true;

            int64_t arg = PC;

            *( ( stack_data* ) ( byte_code + lab->jmp_pc[ i ] ) ) = arg;
            printlg( "Tag initialized.\n"
                     "Detected jmp forward to this tag %d-->%d\n\n", lab->jmp_pc[ i ], lab->pc[ i ] );
        }
    if ( was_init )
        return Success;

    if ( lab->capacity <= 1 + lab->quantity )
    {
        lab->capacity *= 2;

        lab->pc   =   ( size_t* ) realloc( lab->pc,     lab->capacity * sizeof( size_t ) );
        lab->name =   (  char** ) realloc( lab->name,   lab->capacity * sizeof(  char* ) );
        lab->jmp_pc = ( size_t* ) realloc( lab->jmp_pc, lab->capacity * sizeof( size_t ) );

        if ( !lab->pc || !lab->name )
        {
            fprintf( stderr, "Assembling failure: allocation fault.\n" );
            printlg(         "Assembling failure: allocation fault.\n"
                             "File <%s>, function <%s>, line: %d\n\n", __FILE__, __FUNCTION__, __LINE__ );
            return Failure;
        }
    }


    lab->pc    [ lab->quantity ] = PC;
    lab->name  [ lab->quantity ] = tag;
    lab->jmp_pc[ lab->quantity ] = DONT_EXIST;
    lab->quantity++;

    printlg( "---| End of tagging |---\n\n" );

    return Success;
}


void
jmp_proceed( FILE* ostream_list, SPU* spu, size_t* PC, size_t* offset, stream* assembler, size_t* line, labels* lab  )
{
    assert( ostream_list );
    assert( spu );
    assert( PC );
    assert( offset );
    assert( assembler );
    assert( line );
    assert( lab );

    int64_t temp = 0;

    if ( ( temp = jmp_parse( assembler, *offset, lab, *line, *PC ) ) == ERROR )
    {
        unk_com( assembler->virt_mem + *offset, assembler, line );
        spu->status = Failure;
    }
    else
    {
        if ( temp != DONT_EXIST )
            printlg( "Jumping %zd-->%lld\n\n", *PC, temp );

        *( ( stack_data* ) ( spu->byte_code.virt_mem + *PC + COMM_SIZE - ARG_SIZE ) ) = temp;
        fprintf( ostream_list, "%lld", temp );
    }
}


int64_t
jmp_parse( stream* assembler, size_t offset, labels* lab, size_t line_stat, size_t PC )
{
    assert( assembler );
    assert( lab );

    printlg( ">>Parsing jmp argument on line: %d<<\n\n", line_stat );

    int64_t line = 0;
    char* flag = 0;

    PC = PC - ARG_SIZE + COMM_SIZE;

    if ( mscan_long( assembler->virt_mem + offset, &line ) == Success )
    {
        #ifdef ASM_DEBUG
        if ( line < 0 || line >= assembler->comm_count )
        {
            fprintf( stderr, "Incorrect JMP parameter. Check log-file\n" );
            printlg( "Assembling failure: incorrect JMP parameter.\n %d on %d line of code\n", line, PC );
            return ERROR;
        }
        #endif

        return line;
    }

    flag = assembler->virt_mem + offset;
    printlg( "Proceeding label: <" );
    my_printlg( flag );
    printlg( ">\n" );

    if ( flag )
    {

        for ( int i = 0; i < lab->quantity; i++ )
            if ( !my_strcmp ( lab->name[ i ], flag ) && lab->pc[ i ] != DONT_EXIST )
                return lab->pc[ i ];

        if ( lab->quantity >= lab->capacity )
        {
            lab->capacity *= 2;
            lab->pc =     ( size_t* ) realloc( lab->pc,     lab->capacity * sizeof ( size_t ) );
            lab->name =   ( char**  ) realloc( lab->name,   lab->capacity * sizeof (  char* ) );
            lab->jmp_pc = ( size_t* ) realloc( lab->jmp_pc, lab->capacity * sizeof ( size_t ) );

            if ( !lab->pc || !lab->name || !lab->jmp_pc )
            {
                printlg(         "Realloc failure in <%s> file, <%s> func, %d line\n", __FILE__, __FUNCTION__, __LINE__ );
                fprintf( stderr, "Realloc failure in <%s> file, <%s> func, %d line\n", __FILE__, __FUNCTION__, __LINE__ );
                return ERROR;
            }
        }

        lab->pc    [ lab->quantity ] = DONT_EXIST;
        lab->name  [ lab->quantity ] = assembler->virt_mem + offset;
        lab->jmp_pc[ lab->quantity ] = PC;
        lab->quantity++;
        return DONT_EXIST;
    }

    return ERROR;
}


int64_t
get_register( char* assembler, size_t line )
{
    assert( assembler );

    if ( !my_strcmp( assembler, "RAX" ) )
        return RAX;
    else if ( !my_strcmp( assembler, "RBX" ) )
        return RBX;
    else if ( !my_strcmp( assembler, "RCX" ) )
        return RCX;
    else if ( !my_strcmp( assembler, "RDX" ) )
        return RDX;
    else if ( !my_strcmp( assembler, "RPI" ) )
        return RSI;
    else if ( !my_strcmp( assembler, "RDI" ) )
        return RDI;
    else if ( !my_strcmp( assembler, "RSP" ) )
        return RSP;
    else if ( !my_strcmp( assembler, "RBP" ) )
        return RBP;
    else if ( !my_strcmp( assembler, "RCP" ) )
        return RCP;
    else if ( !my_strcmp( assembler, "REX" ) )
        return REX;
    else
    {
        printlg( "Assembling failure. Incorrect register name in line %zd.\n\n", line );
        return INCORRECT_REGISTER;
    }
}


StkError
mem_proceed( stream* assembler, size_t* offset, size_t line, FILE* list, char* mem, instruct* com, size_t* PC )
{
    assert( assembler );
    assert( offset );
    assert( list );
    assert( PC );

    //-----Initializing
    int64_t con = 0;
    int64_t coef = 1;
    int64_t reg = 0;
    size_t line_b = line;
    size_t pc_back = *PC;
    //-----End of init

    move_offset( assembler, offset, &line );

    if ( line > line_b )
    {
        printlg( "Assembling failure.\nIncorrect formula on line %zd\n\n", line );
        return Failure;
    }



        if ( assembler->virt_mem[ *offset ] == 'R' )
        {
            reg = get_register( assembler->virt_mem + *offset, line );
            if ( reg == INCORRECT_REGISTER )
            {
                printlg( "Assembling failure.\nIncorrect memory formula on line %zd.\n( Unknown register )\n\n", line );
                fprintf( stderr, "Assembling failure.\n\n" );
                return Failure;
            }
            *PC += 2 * ARG_SIZE;
            com->reg = true;

            move_offset( assembler, offset, &line );
            if ( line > line_b )
            {
                printlg( "Assembling failure.\nIncorrect formula on line %zd\n\n", line );
                return Failure;
            }
            if ( assembler->virt_mem[ *offset ] == '*'  )
            {
                move_offset( assembler, offset, &line );
                if ( line > line_b || mscan_long( assembler->virt_mem + *offset, &coef ) == Failure )
                {
                    printlg( "Assembling failure.\nIncorrect formula on line %zd\n\n", line );
                    return Failure;
                }
                move_offset( assembler, offset, &line );
                if ( line > line_b )
                {
                    printlg( "Assembling failure.\nIncorrect formula on line %zd\n\n", line );
                    return Failure;
                }
                if ( assembler->virt_mem[ *offset ] == '+' )
                    move_offset( assembler, offset, &line );
            }
            else if ( assembler->virt_mem[ *offset ] == '+' )
            {
                coef = 1;
                move_offset( assembler, offset, &line );
                if ( line > line_b )
                {
                    printlg( "Assembling failure.\nIncorrect formula on line %zd\n\n", line );
                    return Failure;
                }
            }
            else if ( assembler->virt_mem[ *offset ] != ']' )
            {
                printlg( "Assembling failure.\nIncorrect formula on line %zd\n\n", line );
                return Failure;
            }
        }

    if ( mscan_long( assembler->virt_mem + *offset, &con ) == Failure && assembler->virt_mem[ *offset ] != ']' )
    {
        printlg( "Assembling failure.\nIncorrect formula on line %zd\n\n", line );
        return Failure;
    }
    else if ( !com->reg )
    {
        com->con = true;
        *PC += ARG_SIZE;
        move_offset( assembler, offset, &line );
    }

    if ( line > line_b || assembler->virt_mem[ *offset ] != ']' )
    {
        printlg( "Assembling failure.\nIncorrect formula on line %zd\n\n", line );
        return Failure;
    }

    if ( com->ram & 1 )
    {
        *( (   instruct* ) mem + pc_back ) = *com;

        if ( com->reg & 1 )
        {
            *( stack_data* ) ( mem + pc_back + COMM_SIZE ) = reg;
            *( stack_data* ) ( mem + pc_back + COMM_SIZE + ARG_SIZE ) = coef;
            pc_back += 2 * ARG_SIZE;
            fprintf( list, " %lld * %lld", reg, coef );
        }

        if ( com->con & 1 )
        {
            *( stack_data* )( mem + pc_back + COMM_SIZE ) = con;
            fprintf( list, " %lld", con );
        }
    }
    fprintf( list, "\n" );

    return Success;
}
