#include <limits.h>

#include "Stack/utility/log.h"
#include "Stack/stack.h"
#include "calc.h"
#include "compiler.h"


int
main( int argc, char** argv )
{
    logger_open();

    stack stk = {};
    stack_init( &stk, 4 );

    char input[ PATH_MAX ] = "code/test.txt";

    int mode = Terminal( argc, argv, input );
    SPU spu = compile( input, mode );
    Start_Calc( &stk, spu.status );

    logger_close();
    return 0;
}
