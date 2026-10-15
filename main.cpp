#include <limits.h>

#include "Stack/utility/log.h"
#include "processor/calc.h"
#include "assembler/compiler.h"


int
main( int argc, char** argv )
{
    logger_open();

    stack stk = {};
    stack_init( &stk, 4 );

    char input[ PATH_MAX ] = "code/test.txt";

    Terminal( argc, argv, input );
    SPU spu = assemble( input );
    Start_Calc( &spu );

    logger_close();
    return 0;
}
