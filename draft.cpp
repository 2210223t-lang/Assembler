#include <stdio.h>
#include <assert.h>

void
mscan_double( char* text, double* variable )
{
    assert( variable );

    *variable = 0;

    while( '0' <= *text && *text <= '9' )
    {
        *variable = *( variable ) * 10 + *text - '0';
        text++;
    }

    int ten_pow = 10;
    if ( *( text++ ) == '.' )
        while( '0' <= *text && *text <= '9' && ten_pow <= 10000 )
        {
            *variable = *( variable ) + ( double ) ( *text - '0' ) / ten_pow;
            text++;
            ten_pow *= 10;
        }
}


int main()
{

    char a[ 1000 ] = " 1243.321";
    double d = 0;
    mscan_double( a, &d );
    printf( "%lf", d );

    return 0;
}
