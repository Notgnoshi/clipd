#include "args.h"

#include <clipp.h>

#include <iostream>
#include <string>

namespace Boilerplate
{
std::optional<CommandlineArgs_t> ParseArgs( int argc, const char** argv )
{
    static const std::string description = "\tA boilerplate application.";
    CommandlineArgs_t args;

    long double x = 0, y = 0;
    bool help;

    //! @see https://github.com/muellan/clipp for details.
    auto cli =
        ( clipp::option( "-h", "--help" ).set( help ).doc( "Show this help page." ),
          clipp::option( "-v", "--verbose" )
              .set( args.verbose )
              .doc( "Increase output verbosity." ),
          clipp::option( "-x" ) & clipp::value( "x", x ).doc( "The rectangular x coordinate." ),
          clipp::option( "-y" ) & clipp::value( "y", y ).doc( "The rectangular y coordinate." ) );

    auto display_help = [&]() {
        std::cout
            << clipp::make_man_page( cli, argv[0] ).prepend_section( "DESCRIPTION", description );
    };

    // The clipp parser doesn't like const, so pretend it's not.
    if( !clipp::parse( argc, const_cast<char**>( argv ), cli ) || help )
    {
        display_help();
        return std::nullopt;
    }
    else
    {
        args.value = Math::Complex( x, y );
    }

    return args;
}
} // namespace Boilerplate
