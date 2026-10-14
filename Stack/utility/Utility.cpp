#include <stdio.h>
#include <assert.h>

#include "../config.h"
#include "Utility.h"
#include "log.h"
#include "../stack.h"
#include "errors.h"


/**
 * @ */
int UniPrint( stack_data* data )
{
    int type = GetType( data );

    switch ( type )
    {
        case INT_S:
            printlg( "%d", *data );
            return Success;

        case UINT64_S:
            printlg( "%uld", *data );
            return Success;

        case LONG_LONG_S:
            printlg( "%ld", *data );
            return Success;

        case CHAR_S:
            printlg( "%c", *data );
            return Success;

        case STRING_S:
            printlg( "%s", *data );
            return Success;

        case FLOAT_S:
            printlg( "%f", *data );
            return Success;

        case DOUBLE_S:
            printlg( "%lf", *data );
            return Success;

        case SHORT_S:
            printlg( "%sd", *data );
            return Success;

        case UINT32_S:
            printlg( "%ud", *data );
            return Success;

        case UINT16_S:
            printlg( "%usd", *data );
            return Success;

        case UNKNOWN:
            printlg( "Uniprint failure: unknown type\n" );
            return UNFORESEEN_TYPE;
    }

    return UNIPRINT_FAILURE;
}

int UniPrint2( stack* stk, int index )
{
    if ( index >= stk->size )
        return UNFORESEEN_TYPE;

    printlg( ESCAPE, stk->data[ index ] );
    return Success;
}
