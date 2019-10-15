#pragma once
#include "common.h"
#include "network/peer.h"
#include "utils/daemon.h"
#include "utils/delegate.h"
#include "utils/functor.h"

#include <czmq.h>

#include <iostream>
#include <memory>
#include <unordered_map>

namespace Clipd::Network
{
static const constexpr uint8_t version = 0x01;
/**
 * @brief Automatically discover connectable peers.
 *
 * @details In general, peer discovery is a hard problem. There are a fixed number of solutions
 * currently available:
 *
 * 1. Use hard-coded endpoints. E.g., `tcp://192.168.1.220:3400`.
 * 2. Read endpoints from a configuration file.
 * 3. Use a message broker. The broker's location is hard-coded. All *messages* are passed to the
 *    broker.
 * 4. Use an addressing broker. The broker's location is hard-coded. Use the broker to find
 *    addresses of connected peers, and message peers directly.
 * 5. Use a helper library to provide DNS without centralized infrastructure.
 * 6. Build system-level discovery with ARP or ICMP ECHO.
 * 7. Build user-level brute-force discovery by attempting to connect to every address in a
 *    configured segment
 * 8. Roll your own UDP broadcast discovery protocol.
 *
 * A worthwhile (subjective) discovery protocol observes the following requirements:
 *
 * 1. The simplest solution that works.
 * 2. Use ephemeral ports to enable multiple peers running on the same host.
 * 3. No root access required.
 * 4. No infrastructure configuration required.
 * 5. No application configuration.
 * 6. Portable
 * 7. WiFi friendly (use small packets).
 * 8. Protocol-neutral
 * 9. Not tied to a specific language
 * 10. Low latency.
 *
 * These requirements rule out every discovery option other than rolling your own UDP discovery
 * protocol.
 */
class PeerDiscoveryDaemon : public Utils::Daemon
{
private:
    /**
     * @brief The discovery broadcast message.
     *
     * @todo Is there any extra metadata needed?
     */
    struct __attribute__( ( packed ) ) PeerDiscoveryMessage_t
    {
        uint8_t protocol[3] = {'C', 'P', 'D'};       //!< The protocol type for filtering.
        uint8_t version = ::Clipd::Network::version; //!< Discovery protocol version
        uint32_t uuid;                               //!< The broadcasting peer's UUID.
        uint16_t msg_port;                           //!< The TCP port the peer listens on.

        PeerDiscoveryMessage_t( uint32_t uuid, uint16_t msg_port ) :
            uuid( uuid ), msg_port( msg_port )
        {}
    };

public:
    /**
     * @brief Construct a new Peer Discovery Daemon object.
     *
     * @details Create a czmq zbeacon publisher that broadcasts a PeerDiscoveryMessage_t
     * periodically.
     *
     * @param this_uuid The UUID of this peer.
     * @param discovery_port The port to broadcast (and listen for) discovery messages on.
     * @param msg_port The TCP messaging port this peer communicates on.
     */
    PeerDiscoveryDaemon( const uint32_t this_uuid, uint16_t discovery_port, uint16_t msg_port ) :
        m_uuid( this_uuid ),
        m_discovery_port( discovery_port ),
        m_msg_port( msg_port ),
        m_beacon( zactor_new( zbeacon, nullptr ) )
    {
        // Temporarily enable verbose logging
        zstr_send( m_beacon, "VERBOSE" );
        zsock_send( m_beacon, "si", "CONFIGURE", m_discovery_port );
        char* hostname = zstr_recv( m_beacon );

        // Ensure UDP broadcasts are supported.
        if( !*hostname )
        {
            std::cerr << "UDP broadcasts are not supported." << std::endl;
            zactor_destroy( &m_beacon );
            m_beacon = nullptr;
        }
        freen( hostname );

        // Publish broadcast_message every so often.
        const PeerDiscoveryMessage_t broadcast_message( m_uuid, m_msg_port );
        zsock_send( m_beacon, "sbi", "PUBLISH", &broadcast_message,
                    sizeof( PeerDiscoveryMessage_t ), 500 );

        // Filter on the first three bytes of the broadcast message.
        zsock_send( m_beacon, "sb", "SUBSCRIBE", &broadcast_message, 3 );
    }

    /**
     * @brief Register a callback to be notified whenever a new peer is found.
     */
    void registerOnPeerAdded( Utils::Functor<void( std::shared_ptr<const Peer> )> callback )
    {
        m_peer_added_delegate.subscribe( callback );
    }

    /**
     * @brief Register a callback to be notified whenever a peer is removed.
     */
    void registerOnPeerRemoved( Utils::Functor<void( std::shared_ptr<const Peer> )> callback )
    {
        m_peer_removed_delegate.subscribe( callback );
    }

    /**
     * @brief Stop the peer discovery daemon.
     *
     * @details Stops the daemon background thread.
     */
    void stop() override
    {
        Utils::Daemon::stop();

        // Do not delete the beacon in the destructor due to weird threading and atexit() issues.
        if( m_beacon )
        {
            zstr_sendx( m_beacon, "SILENCE", NULL );
            zstr_sendx( m_beacon, "UNSUBSCRIBE", NULL );
            zactor_destroy( &m_beacon );
        }
    }

protected:
    //! @todo Determine what should be protected so that the unit tests can override.

private:
    /**
     * @brief Listen for zbeacon messages from other peers.
     *
     * @todo Create a new Peer object when another peer is found.
     * @todo Figure out how to determine if a peer has left the network. Will likely invole timers.
     * @todo Replace with a zpoller or zloop?
     */
    void loop() override
    {
        if( !m_beacon )
        {
            return;
        }

        // Non-blocking receive a two-frame message, where the first frame is the sender IP address,
        // and the second is the data sent.
        zmsg_t* msg = zmsg_recv_nowait( m_beacon );
        if( msg )
        {
            std::cout << "Received message." << std::endl;
            zmsg_destroy( &msg );
        }
    }

private:
    const uint32_t m_uuid;
    uint16_t m_discovery_port;
    uint16_t m_msg_port;
    zactor_t* m_beacon;

    std::unordered_map<uint32_t, std::shared_ptr<const Peer>> m_peers;
    Utils::Delegate<void( std::shared_ptr<const Peer> )> m_peer_added_delegate;
    Utils::Delegate<void( std::shared_ptr<const Peer> )> m_peer_removed_delegate;
};
} // namespace Clipd::Network
