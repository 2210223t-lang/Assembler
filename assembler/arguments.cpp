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
#include "../processor/asm_uitility.h"
#include "arguments.h"
#include "compiler.h"
#include "labels.h"
#include "compiler.h"



void
jmp_proceed ( FILE* ostream_list, SPU* spu, size_t* PC, size_t* offset, stream* assembler, size_t* line, labels* lab  )
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
jmp_parse ( stream* assembler, size_t offset, labels* lab, size_t line_stat, size_t PC )
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

        creat_lab( assembler->virt_mem, lab, PC, offset);
        return DONT_EXIST;
    }

    return ERROR;
}


int64_t
get_register ( char* assembler, size_t line )
{
    assert( assembler );

    if ( !my_strcmp( assembler, "RAX" ) || !my_strcmp( assembler, "RICH" ) )
        return RAX;
    else if ( !my_strcmp( assembler, "RBX" ) || !my_strcmp( assembler, "RUCH" ) )
        return RBX;
    else if ( !my_strcmp( assembler, "RCX" ) || !my_strcmp( assembler, "RACH" ) )
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
mem_proceed ( stream* assembler, size_t* offset, size_t line, FILE* list, char* mem, instruct* com, size_t* PC )
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
        *( ( instruct* ) mem + pc_back ) = *com;

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



void
push_proceed( SPU* spu, size_t* PC, stream* assembler, instruct* comm, size_t* offset, size_t line, FILE* ostream_list )
{
    assert( spu );
    assert( PC );
    assert( assembler );
    assert( comm );
    assert( offset );
    assert( ostream_list );


    double value = 0;


    if ( *( assembler->virt_mem + *offset ) == '[' )
    {
        comm->ram = true;
        if ( mem_proceed( assembler, offset, line, ostream_list, spu->byte_code.virt_mem, comm, PC ) == Failure )
            spu->status = Failure;
    }
    else if ( mscan_double( assembler->virt_mem + *offset, &value ) == Success )
    {
        comm->con = true;
        print_byn( ostream_list, spu->byte_code.virt_mem, comm, value, assembler->virt_mem + *offset, *PC );
        *PC += ARG_SIZE;
    }
    else
    {
        comm->reg = true;
        value = get_register( assembler->virt_mem + *offset, line );
        if ( value == INCORRECT_REGISTER )
            spu->status = Failure;
        print_byn( ostream_list, spu->byte_code.virt_mem, comm, value, assembler->virt_mem + *offset, *PC );
        *PC += ARG_SIZE;
    }
}


void
pop_proceed( SPU* spu, size_t* PC, stream* assembler, instruct* comm, size_t* offset, size_t* line, FILE* ostream_list )
{
    double value = 0;

    if ( check_arg( assembler->virt_mem + *offset ) )
    {
        move_offset( assembler, offset, line );

        if ( assembler->virt_mem[ *offset ] == '[' )
        {
            comm->ram = true;
            if ( mem_proceed( assembler, offset, *line, ostream_list, spu->byte_code.virt_mem, comm, PC ) == Failure )
                spu->status = Failure;
        }
        else
        {
            value = get_register( assembler->virt_mem + *offset, *line );
            if ( value == INCORRECT_REGISTER )
                spu->status = Failure;
            else
                comm->reg = true;
            print_byn( ostream_list, spu->byte_code.virt_mem, comm, value, assembler->virt_mem + *offset, *PC );
            *PC += ARG_SIZE;
        }
    }
    else
        print_byn( ostream_list, spu->byte_code.virt_mem, comm, value, assembler->virt_mem + *offset, *PC );


}
