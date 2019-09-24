#include "args.h"

#include <clipp.h>

#include <cstdlib>
#include <iostream>
#include <string>

namespace Clipd
{
CommandlineArgs_t ParseArgs( int argc, const char** argv )
{
    static const std::string description = "\tPeer-to-peer X11 clipboard synchronization.";
    CommandlineArgs_t args;

    //! @see https://github.com/muellan/clipp for details.
    auto cli = ( clipp::option( "-h", "--help" ).set( args.help ).doc( "Show this help page." ),
                 clipp::option( "-v", "--verbose" )
                     .set( args.verbose )
                     .doc( "Increase output verbosity." ),
                 clipp::option( "-c", "--clipd" )
                     .set( args.clipd )
                     .doc( "Listen for clipboard changes in the background." ) );

    auto display_help = [&]() {
        std::cout
            << clipp::make_man_page( cli, argv[0] ).prepend_section( "DESCRIPTION", description );
    };

    // The clipp parser doesn't like const, so pretend it's not.
    if( !clipp::parse( argc, const_cast<char**>( argv ), cli ) )
    {
        display_help();
        std::exit( 1 );
    }
    else if( args.help )
    {
        display_help();
        std::exit( 0 );
    }

    return args;
}
} // namespace Clipd
