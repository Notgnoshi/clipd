#pragma once
#include "common.h"
#include "network/message.h"
#include "utils/daemon.h"
#include "utils/delegate.h"
#include "utils/functor.h"

#include <zyre.h>

#include <iostream>

namespace Clipd::Network
{

//! @todo Discuss how Zyre implements its peer discovery and messaging.
//! See http://hintjens.com/blog:32 and https://rfc.zeromq.org/spec:36/ZRE/
class PeerDiscoveryDaemon : public Utils::Daemon
{
public:
    PeerDiscoveryDaemon( uint16_t discovery_port, bool verbose = false ) :
        m_discovery_port( discovery_port ),
        m_verbose( verbose ),
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

        zyre_join( m_znode, "GLOBAL" );
        zyre_join( m_znode, "CLIPD" );
    }

    ~PeerDiscoveryDaemon() {}

    void receiveLocalClipboardUpdate( const std::string& contents )
    {
        //! @todo Consider encrypting TCP traffic.
        zyre_shouts( m_znode, "CLIPD", "%s", contents.data() );
    }

    void registerOnRemoteClipboardUpdate( Utils::Functor<void( const std::string& )> callback )
    {
        m_remote_update_delegate.subscribe( callback );
    }

    void start() override
    {
        Utils::Daemon::start();

        int status = zyre_start( m_znode );
        if( m_verbose )
        {
            std::cout << "Starting zyre peer discovery... "
                      << ( status == 0 ? "Success!" : "Failed." ) << std::endl;
        }
    }

    void stop() override
    {
        Utils::Daemon::stop();

        zyre_stop( m_znode );
        zyre_destroy( &m_znode );
        m_znode = nullptr;
    }

protected:
    void loop() override
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

    //! @todo Move implementations to .cpp file now that PoC is done.
    //! @todo UNIT TESTS.
    void parseMessage( zmsg_t* msg )
    {
        //! @todo Find a less boilerplate-y method of parsing messages.
        Messages::MessageType type = Messages::parseMessageType( msg );
        switch( type )
        {
            case Messages::MessageType::Enter:
            {
                //! @todo Associate a peer's UUID with a host IP.
                //! That way, when multiple nodes are running on the same host, less bad things
                //! happen.
                Messages::Enter payload( msg );
                std::cout << payload << std::endl;
                break;
            }
            case Messages::MessageType::Exit:
            {
                Messages::Exit payload( msg );
                std::cout << payload << std::endl;
                break;
            }
            case Messages::MessageType::Evasive:
            {
                Messages::Evasive payload( msg );
                break;
            }
            case Messages::MessageType::Join:
            {
                Messages::Join payload( msg );
                break;
            }
            case Messages::MessageType::Leave:
            {
                Messages::Leave payload( msg );
                break;
            }
            case Messages::MessageType::Whisper:
            {
                Messages::Whisper payload( msg );
                break;
            }
            case Messages::MessageType::Shout:
            {
                Messages::Shout payload( msg );
                std::cout << payload << std::endl;
                //! @todo Use a session token as the group name. Enable one peer to be in multiple
                //! sessions.
                if( payload.groupname == "CLIPD" )
                {
                    m_remote_update_delegate( payload.message );
                }
                break;
            }
            case Messages::MessageType::Unknown:
            {
                break;
            }
        }
    }

private:
    uint16_t m_discovery_port;
    bool m_verbose;
    zyre_t* m_znode;

    Utils::Delegate<void( const std::string& )> m_remote_update_delegate;
};
} // namespace Clipd::Network
