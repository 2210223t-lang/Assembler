#include <assert.h>

#include "../processor/asm_uitility.h"
#include "../Stack/utility/log.h"
#include "arguments.h"
#include "labels.h"



int
creat_lab ( char* code, labels* lab, size_t PC, size_t offset )
{
    assert( code );
    assert( lab );

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
        lab->name  [ lab->quantity ] = code + offset;
        lab->jmp_pc[ lab->quantity ] = PC;
        lab->quantity++;
        return DONT_EXIST;
}


StkError
add_tag ( char* byte_code, char* tag, size_t PC, labels* lab, size_t line )
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
            if ( save_change( lab, byte_code, i, PC, &was_init ) == Failure )
                return Failure;

    if ( was_init )
        return Success;

    creat_lab( byte_code, lab, PC, tag - byte_code );

    lab->pc[ lab->quantity - 1 ] = PC;

    printlg( "---| End of tagging |---\n\n" );

    return Success;
}


StkError
save_change( labels* lab, char* code, int count, size_t PC, bool* was_init )
{
    assert( lab );
    assert( code );
    assert( was_init );

    if ( lab->jmp_pc[ count ] < 0 )
    {
        printlg( "Incorrect jmp_pc value in <%s> file, <%s> func, %d line.\n"
                 "Value is: %d", __FILE__, __FUNCTION__, __LINE__, lab->jmp_pc[ count ] );
        return Failure;
    }
    lab->pc[ count ] = PC;
    *was_init = true;

    int64_t arg = PC;

    *( ( stack_data* ) ( code + lab->jmp_pc[ count ] ) ) = arg;
    printlg( "Tag initialized.\n"
             "Detected jmp forward to this tag %d-->%d\n\n", lab->jmp_pc[ count ], lab->pc[ count ] );

    return Success;
}
