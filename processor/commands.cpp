#include <cstdint>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <string.h>

#include "../Stack/config.h"
#include "../Stack/utility/errors.h"
#include "../Stack/utility/log.h"
#include "../Stack/stack.h"
#include "asm_uitility.h"

#define DEPTH 6



int
factorial( int limit )
{
    int result = 1;
    for ( int i = 2; i <= limit; i++ )
        result *= i;
    return result;
}


int
div( stack* stk, short* past_proc_sign )
{
    assert( stk );
    assert( past_proc_sign );

    printlg( "---DIV instruction---\n\n" );

    double temp1 = ( double ) stack_pop( stk ) / 1000;
    double temp2 = ( double ) stack_pop( stk ) / 1000;
    int64_t result = ( temp2 / temp1 );
    stack_push( stk, result * 1000 );

    if ( result > 0 )
        *past_proc_sign = 1;
    else if ( result == 0 )
        *past_proc_sign = 0;
    else
        *past_proc_sign = -1;

    printlg( "-----end of DIV-----\n\n" );
    return Success;
}


int
mult( stack* stk, short* past_proc_sign )
{
    assert( stk );
    assert( past_proc_sign );

    printlg( "---MULT instruction---\n\n" );

    double temp1 = ( double ) stack_pop( stk ) / 1000;
    double temp2 = ( double ) stack_pop( stk ) / 1000;
    int64_t result = temp1 * temp2;
    stack_push( stk, result * 1000 );

    if ( result > 0 )
        *past_proc_sign = 1;
    else if ( result == 0 )
        *past_proc_sign = 0;
    else
        *past_proc_sign = -1;

    printlg( "-----end of MULT-----\n\n" );
    return Success;
}


int
sum( stack* stk, short* past_proc_sign )
{
    assert( stk );
    assert( past_proc_sign );

    printlg( "---SUM instruction---\n\n" );

    int64_t result = stack_pop( stk ) + stack_pop( stk );

    stack_push( stk , result );

    if ( result > 0 )
        *past_proc_sign = 1;
    else if ( result == 0 )
        *past_proc_sign = 0;
    else
        *past_proc_sign = -1;

    printlg( "-----end of SUM-----\n\n" );
    return Success;
}


int
out( stack* stk )
{
    assert( stk );

    printlg( "---OUT instruction---\n\n" );

    double result = ( double ) stack_pop( stk ) / 1000;
    printf( "%lf\n", result );
    stack_push( stk, result * 1000 );

    printlg( "-----end of OUT-----\n\n" );
    return Success;
}


int
rest( stack* stk )
{
    assert( stk );

    printlg( "---REST instuction---\n\n" );

    stack_destr( stk );
    stack_init( stk, 4 );

    printlg( "-----end of REST-----\n\n" );
    return Success;
}


int
sub( stack* stk, short* past_proc_sign )
{
    assert( stk );
    assert( past_proc_sign );

    printlg( "---SUB instruction---\n\n" );

    int64_t temp1 = stack_pop( stk );
    int64_t temp2 = stack_pop( stk );
    int64_t result = ( temp2 - temp1 );
    stack_push( stk, result );

    if ( result > 0 )
        *past_proc_sign = 1;
    else if ( result == 0 )
        *past_proc_sign = 0;
    else
        *past_proc_sign = -1;

    printlg( "-----end of SUB-----\n\n" );
    return Success;
}


int
sqr( stack* stk, short* past_proc_sign )
{
    assert( stk );
    assert( past_proc_sign );

    printlg( "---SQR instruction---\n\n" );

    double temp = ( double ) stack_pop( stk ) / 1000;

    uint64_t result = temp * temp;

    if ( result == 0 )
        *past_proc_sign = 0;
    else
        *past_proc_sign = 1;

    stack_push( stk, result * 1000 );

    printlg( "-----end of SQR-----\n\n" );
    return Success;
}


int
fact( stack* stk )
{
    assert( stk );

    printlg( "---FACT instruction---\n\n" );

    uint64_t temp = stack_pop( stk );

    if ( temp < 0 || temp % 1000 )
    {
        printlg( "Attempt to calculate factorial of negative of not - integer variable\n" );
        return Failure;
    }

    uint64_t result = 1;

    result = factorial( temp / 1000 );

    stack_push( stk, result * 1000 );

    printlg( "-----end of FACT-----\n\n" );
    return Success;
}


int
sinus( stack* stk, short* past_proc_sign )
{
    assert( stk );
    assert( past_proc_sign );

    printlg( "---SINUS instruction---\n\n" );

    double temp = ( double ) stack_pop( stk ) / 1000;

    double result = 0;

    for ( int i = 0; i < DEPTH; i++ )
        result += pow( -1, i ) * pow( temp, 2 * i + 1 ) / factorial( 2 * i + 1 );

    if ( result > 0 )
        *past_proc_sign = 1;
    else if ( result == 0 )
        *past_proc_sign = 0;
    else
        *past_proc_sign = -1;

    stack_push( stk, result * 1000 );

    printlg( "-----end of SINUS-----\n\n" );
    return Success;
}


int
push( stack* stk, stack_data* value, short* past_proc_sign  )
{
    assert( stk );
    assert( value );


    printlg( "---PUSH instruction---\n\n" );

    stack_push( stk, ( *value ) );
    if ( *value > 0 )
        *past_proc_sign = 1;
    else if ( *value == 0 )
        *past_proc_sign = 0;
    else
        *past_proc_sign = -1;

    printlg( "-----end of PUSH-----\n\n" );
    return Success;
}


int
pop( stack* stk, stack_data* storage )
{
    assert( stk );

    printlg( "---| POP instruction |---\n\n" );

    stack_data temp = stack_pop( stk );

    if ( storage )
        *storage = temp;

    printlg( "-----end of POP-----\n\n" );
    return Success;
}


int
jmp( size_t* PC, size_t* value )
{
    assert( PC );
    assert( value );

    if ( *value < 0 )
    {
        printlg( "Incorrect jmp destination in line %zd\n", *PC );
        fprintf( stderr, "Incorrect jmp destination in line %zd\n", *PC );
        return Failure;
    }

    printlg( "---| Jumping instruction |---\n\n" );

    memcpy( PC, value, sizeof( size_t ) );

    printlg( "\n-----------------------------\n\n" );
    return Success;
}


int
call ( SPU* spu, size_t dest )
{
    assert( spu );

    printlg( "---| Call instruction |---\n\n" );

    stack_push( &spu->ret_val, spu->PC );
    jmp( &spu->PC, &dest );

    printlg( "--------------------------\n\n" );

    return Success;
}


int
ret ( SPU* spu )
{
    assert( spu );

    printlg( "---| Ret instruction |---\n\n" );

    size_t temp = stack_pop( &spu->ret_val );
    jmp( &spu->PC, &temp );

    printlg( "-------------------------\n\n" );

    return Success;
}


int
root ( SPU* spu )
{
    assert( spu );

    printlg( "---| Root instruction |---\n\n" );

    double result = ( double ) stack_pop( &spu->stk ) / 1000;

    if ( result < 0 )
    {
        printlg( "Processing failure.\n"
                 "Attempt to calculate negative root. PC = %zd\n\n", spu->PC );
        fprintf( stderr, "Processing failure.\n\n" );
        return Failure;
    }

    result = sqrt( result ) * 1000;

    stack_push( &spu->stk, result );

    if ( result > 0 )
        spu->past_proc_sign = 1;
    else if ( result == 0 )
        spu->past_proc_sign = 0;
    else
        spu->past_proc_sign = -1;

    printlg( "---| End of root instruction |---\n\n" );

    return Success;
}
