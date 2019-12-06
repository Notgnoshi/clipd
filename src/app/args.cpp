#include "app/args.h"

#include <clipp.h>

#include <cstdlib>
#include <iostream>
#include <string>

namespace Clipd::App
{
CommandlineArgs_t ParseArgs( int argc, const char** argv )
{
    static const std::string description = "\tPeer-to-peer X11 clipboard synchronization.";
    std::string cert_path = "";
    CommandlineArgs_t args;

    //! @see https://github.com/muellan/clipp for details.
    auto cli = ( clipp::option( "-h", "--help" ).set( args.help ).doc( "Show this help page." ),
                 clipp::option( "-v", "--verbose" )
                     .set( args.verbose )
                     .doc( "Increase output verbosity." ),
                 clipp::option( "-p", "--port" )
                     .set( args.discovery_port )
                     .doc( "The port to use for peer discovery." ),
                 ( clipp::option( "-e", "--encrypt" ).set( args.encrypt_traffic ) &
                   clipp::value( "certificate", cert_path ) ) %
                     "Encrypt traffic using the given certificate.",
                 ( clipp::option( "-g", "--generate" ).set( args.generate_certificate ) &
                   clipp::value( "certificate", cert_path ) ) %
                     "Generate a certificate." );

    auto display_help = [&]() {
        std::cout
            // NOLINTNEXTLINE
            << clipp::make_man_page( cli, argv[0] ).prepend_section( "DESCRIPTION", description );
    };

    // The clipp parser doesn't like const, so pretend it's not.
    if( !clipp::parse( argc, const_cast<char**>( argv ), cli ) ) // NOLINT
    {
        display_help();
        std::exit( 1 );
    }
    // Clipp doesn't seem to play nicely with std::filesystem::path.
    args.certificate = cert_path;

    if( args.help )
    {
        display_help();
        std::exit( 0 );
    }

    if( args.encrypt_traffic )
    {
        if( !fs::exists( args.certificate ) )
        {
            std::cout << "The provided certificate '" << args.certificate << "' does not exist."
                      << std::endl;
            std::exit( 1 );
        }

        fs::path secret = args.certificate;
        secret += "_secret";

        if( !fs::exists( secret ) )
        {
            std::cout << "The provided public key does not have a corresponding '" << secret
                      << "' secret key." << std::endl;
            std::exit( 1 );
        }
    }

    return args;
}
} // namespace Clipd::App
