#include "app/args.h"
#include "app/certs.h"
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

void setSignalHandler()
{
    struct sigaction sigint_handler;           // NOLINT
    sigint_handler.sa_handler = handle_SIGINT; // NOLINT
    sigemptyset( &sigint_handler.sa_mask );
    sigint_handler.sa_flags = 0;
    sigaction( SIGINT, &sigint_handler, nullptr );
}

/**
 * @brief The main application entry point.
 *
 * @return Zero if the application ran successfully, non-zero otherwise.
 */
int main( int argc, const char** argv )
{
    Clipd::App::CommandlineArgs_t args = Clipd::App::ParseArgs( argc, argv );

    if( args.generate_certificate )
    {
        Clipd::App::GenerateCertificate( args.certificate );
        return 0;
    }

    zcert_t* zcert = nullptr;
    if( args.encrypt_traffic )
    {
        zcert = Clipd::App::LoadCertificate( args.certificate );
    }

    //! @todo Create an "Application" object (main() should be as simple and small as possible.)
    //! @note Creating an "Application" object is substantially complicated by the posix signal
    //! handling.
    auto clipd = std::make_unique<Clipd::Clipboard::ClipboardDaemon>();
    auto discoveryd = std::make_unique<Clipd::Network::PeerDiscoveryDaemon>(
        args.discovery_port, zcert, args.session, args.verbose );
    clipd->registerOnTextUpdate(
        Clipd::Utils::Functor<void( const std::string& )>( [&args]( const std::string& update ) {
            if( args.verbose )
            {
                std::cout << "clipd: clipboard update:" << std::endl;
                std::cout << update << std::endl;
            }
        } ) );
    clipd->registerOnTextUpdate( Clipd::Utils::Functor<void( const std::string& )>(
        discoveryd.get(), &Clipd::Network::PeerDiscoveryDaemon::receiveLocalClipboardUpdate ) );

    discoveryd->registerOnRemoteClipboardUpdate( Clipd::Utils::Functor<void( const std::string& )>(
        clipd.get(), &Clipd::Clipboard::ClipboardDaemon::receiveRemoteClipboardUpdate ) );

    g_daemons.push_back( std::move( clipd ) );
    g_daemons.push_back( std::move( discoveryd ) );

    setSignalHandler();

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
