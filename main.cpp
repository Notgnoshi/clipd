#include "args.h"
#include "clipboard/clipboard_daemon.h"
#include "common.h"
#include "daemon.h"

#include <unistd.h>

#include <csignal>
#include <cstdlib>
#include <iostream>
#include <list>

std::list<std::unique_ptr<Clipd::Daemon>> g_daemons;

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
    Clipd::CommandlineArgs_t args = Clipd::ParseArgs( argc, argv );
    //! @todo Determine how to communicate between daemons.
    if( args.clipd )
    {
        auto clipd = std::make_unique<Clipd::Clipboard::ClipboardDaemon>();

        clipd->registerOnTextUpdate(
            Functor<void( const std::string& )>( [&args]( const std::string& update ) {
                if( args.verbose )
                {
                    std::cout << "clipd: clipboard update:" << std::endl;
                    std::cout << update << std::endl;
                }
            } ) );
        g_daemons.push_back( std::move( clipd ) );
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
