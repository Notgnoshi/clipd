#include "args.h"
#include "common.h"
#include "math/complex.h"

#include <iostream>

/**
 * @brief The main application entry point.
 *
 * @return int
 */
int main( int argc, const char** argv )
{
    std::optional<Boilerplate::CommandlineArgs_t> args = Boilerplate::ParseArgs( argc, argv );

    if( !args )
    {
        return 0;
    }

    Math::Complex d( 1, 1 );

    std::cout << ( args->verbose ? std::boolalpha : std::noboolalpha ) << ( args->value == d )
              << std::endl;

    return 0;
}
