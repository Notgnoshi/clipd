#include "math/complex.h"

namespace Math
{
Complex::Complex() : x( 1 ), y( 0 ), r( 1 ), arg( 0 ) {}

Complex::Complex( long double x, long double y ) :
    x( x ),
    y( y ),
    r( std::sqrt( x * x + y * y ) ),
    arg( std::atan2( y, x ) )
{}

bool Complex::operator==( const Complex& other ) const
{
    return x == other.x && y == other.y;
}

bool Complex::operator!=( const Complex& other ) const
{
    return !( *this == other );
}
} // namespace Math
