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


int
jmp_parse( stream* assembler, size_t offset, labels* lab, size_t PC )
{
    assert( assembler );

    printlg( ">>Parsing jmp argument on line: %d<<\n\n", PC );

    int64_t line = 0;
    char* flag = 0;

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

    if ( flag )
    {
        flag = assembler->virt_mem + offset;

        for ( int i = 0; i < lab->quantity; i++ )
            if ( lab->name[ i ] == flag )
                return lab->line[ i ];

        if ( ++( lab->quantity ) > lab->capacity )
        {
            lab->capacity *= 2;
            lab->line = ( size_t* ) realloc( lab->line, lab->capacity * sizeof ( size_t ) );
            lab->name = ( char**  ) realloc( lab->name, lab->capacity * sizeof (  char* ) );

            if ( !lab->line || !lab->name )
            {
                printlg(         "Realloc failure in <%s> file, <%s> func, %d line\n", __FILE__, __FUNCTION__, __LINE__ );
                fprintf( stderr, "Realloc failure in <%s> file, <%s> func, %d line\n", __FILE__, __FUNCTION__, __LINE__ );
                return ERROR;
            }
        }

        lab->line[ lab->quantity ] = DONT_EXIST;
        lab->name[ lab->quantity ] = assembler->virt_mem + offset;
        return DONT_EXIST;
    }

    return ERROR;
}
