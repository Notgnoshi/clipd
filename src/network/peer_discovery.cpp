#include "network/peer_discovery.h"

#include "network/message.h"

namespace Clipd::Network
{
PeerDiscoveryDaemon::PeerDiscoveryDaemon( uint16_t discovery_port, zcert_t* certificate,
                                          const std::string& session, bool verbose ) :
    m_discovery_port( discovery_port ),
    m_verbose( verbose ),
    m_session( session ),
    m_zcert( certificate ),
    m_znode( zyre_new( nullptr ) )
{
    if( m_verbose )
    {
        zyre_set_verbose( m_znode );
        zyre_print( m_znode );
    }
    if( m_discovery_port != 0 )
    {
        zyre_set_port( m_znode, m_discovery_port );
    }
    if( m_zcert )
    {
        zyre_set_zcert( m_znode, m_zcert );
    }

    //! @todo Does every node really have to join the GLOBAL group?
    zyre_join( m_znode, "GLOBAL" );
    zyre_join( m_znode, m_session.c_str() );
}

void PeerDiscoveryDaemon::receiveLocalClipboardUpdate( const std::string& contents )
{
    zyre_shouts( m_znode, m_session.c_str(), "%s", contents.data() );
}

void PeerDiscoveryDaemon::registerOnRemoteClipboardUpdate(
    Utils::Functor<void( const std::string& )> callback )
{
    m_remote_update_delegate.subscribe( callback );
}

void PeerDiscoveryDaemon::start()
{
    Utils::Daemon::start();

    int status = zyre_start( m_znode );
    if( m_verbose )
    {
        std::cout << "Starting zyre peer discovery... " << ( status == 0 ? "Success!" : "Failed." )
                  << std::endl;
    }
}

void PeerDiscoveryDaemon::stop()
{
    Utils::Daemon::stop();

    zcert_destroy( &m_zcert );
    zyre_stop( m_znode );
    zyre_destroy( &m_znode );
    m_znode = nullptr;
}

void PeerDiscoveryDaemon::loop()
{
    //! @todo zmsg_recv_nowait is deprecated in favor of zloop or zpoller. Think about using
    //! a better design.
    zmsg_t* msg = zmsg_recv_nowait( zyre_socket( m_znode ) );
    if( msg )
    {
        parseMessage( msg );
        zmsg_destroy( &msg );
    }
}

void PeerDiscoveryDaemon::parseMessage( zmsg_t* msg )
{
    const std::string header = "==============================================================";

    const Messages::MessageType type = Messages::parseMessageType( msg );
    switch( type )
    {
        case Messages::MessageType::Enter: {
            //! @todo Associate a peer's UUID with a host IP.
            //! That way, when multiple nodes are running on the same host, less bad things
            //! happen.
            const Messages::Enter payload( msg );
            std::cout << header << std::endl;
            std::cout << payload << std::endl;
            std::cout << header << std::endl;
            break;
        }
        case Messages::MessageType::Exit: {
            const Messages::Exit payload( msg );
            std::cout << header << std::endl;
            std::cout << payload << std::endl;
            std::cout << header << std::endl;
            break;
        }
        case Messages::MessageType::Evasive: {
            const Messages::Evasive payload( msg );
            break;
        }
        case Messages::MessageType::Join: {
            const Messages::Join payload( msg );
            break;
        }
        case Messages::MessageType::Leave: {
            const Messages::Leave payload( msg );
            break;
        }
        case Messages::MessageType::Whisper: {
            const Messages::Whisper payload( msg );
            break;
        }
        case Messages::MessageType::Shout: {
            const Messages::Shout payload( msg );
            std::cout << header << std::endl;
            std::cout << payload << std::endl;
            std::cout << header << std::endl;
            if( payload.groupname == m_session )
            {
                m_remote_update_delegate( payload.message );
            }
            break;
        }
        case Messages::MessageType::Unknown: {
            break;
        }
    }
}
} // namespace Clipd::Network
