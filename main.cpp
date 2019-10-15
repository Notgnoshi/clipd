#include "app/args.h"
#include "clipboard/clipboard_daemon.h"
#include "common.h"
#include "network/peer_discovery.h"
#include "utils/daemon.h"
#include "utils/uuid.h"

#include <unistd.h>

#include <csignal>
#include <cstdlib>
#include <iostream>
#include <list>

std::list<std::unique_ptr<Clipd::Utils::Daemon>> g_daemons;

/**
 * @brief Handle the SIGINT in a posix compliant manner to stop all of the background daemons.
 */
void handle_SIGINT( int ) // NOLINT
{
    for( const auto& daemon : g_daemons )
    {
        daemon->stop();
    }
}

/**
 * @brief The main application entry point.
 *
 * @return Zero if the application ran successfully, non-zero otherwise.
 */
int main( int argc, const char** argv )
{
    Clipd::App::CommandlineArgs_t args = Clipd::App::ParseArgs( argc, argv );
    if( args.clipd )
    {
        if( args.verbose )
        {
            std::cout << "Listening for clipboard updates." << std::endl;
        }

        auto clipd = std::make_unique<Clipd::Clipboard::ClipboardDaemon>();

        clipd->registerOnTextUpdate( Clipd::Utils::Functor<void( const std::string& )>(
            [&args]( const std::string& update ) {
                if( args.verbose )
                {
                    std::cout << "clipd: clipboard update:" << std::endl;
                    std::cout << update << std::endl;
                }
            } ) );
        g_daemons.push_back( std::move( clipd ) );
    }

    //! @note Peers are responsible for generating their own UUIDs and responding to the peer
    //! discovery machanism with their UUID as a part of the discovery protocol.
    const uint32_t uuid = Clipd::Utils::randint<uint32_t>();
    if( args.verbose )
    {
        std::cout << "Starting peer with UUID: " << uuid << std::endl;
    }

    if( args.discovery )
    {
        if( args.verbose )
        {
            std::cout << "Starting peer discovery" << std::endl;
        }

        //! @todo Use whatever TCP port is available (not a user configurable parameter).
        auto discoveryd =
            std::make_unique<Clipd::Network::PeerDiscoveryDaemon>( uuid, args.discovery_port, 420 );

        discoveryd->registerOnPeerAdded(
            [&args]( std::shared_ptr<const Clipd::Network::Peer> peer ) {
                if( args.verbose )
                {
                    std::cout << "discoveryd: added peer: " << peer->uuid;
                }
            } );
        discoveryd->registerOnPeerRemoved(
            [&args]( std::shared_ptr<const Clipd::Network::Peer> peer ) {
                if( args.verbose )
                {
                    std::cout << "discoveryd: removed peer: " << peer->uuid;
                }
            } );

        g_daemons.push_back( std::move( discoveryd ) );
    }

    struct sigaction sigint_handler;           // NOLINT
    sigint_handler.sa_handler = handle_SIGINT; // NOLINT
    sigemptyset( &sigint_handler.sa_mask );
    sigint_handler.sa_flags = 0;
    sigaction( SIGINT, &sigint_handler, nullptr );

    for( const auto& daemon : g_daemons )
    {
        daemon->start();
    }

    for( const auto& daemon : g_daemons )
    {
        daemon->join();
    }

    return 0;
}
