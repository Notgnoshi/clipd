#pragma once
#include "common.h"
#include "utils/daemon.h"
#include "utils/delegate.h"
#include "utils/functor.h"

#include <czmq.h>
#include <zcert.h>
#include <zyre.h>

namespace Clipd::Network
{
//! @todo Discuss how Zyre implements its peer discovery and messaging.
//! See http://hintjens.com/blog:32 and https://rfc.zeromq.org/spec:36/ZRE/
class PeerDiscoveryDaemon : public Utils::Daemon
{
public:
    /**
     * @brief Construct a new Peer Discovery Daemon object.
     *
     * @note The discovery port must match for peers to be discovered. The TCP
     * port used to communicate between discovered peers is automatically chosen.
     *
     * The constructor creates the Zyre node, and configures the port, session, and certificate
     * used, but defers starting peer discovery until the start() method has been called.
     *
     * @param discovery_port The port to perform UDP peer discovery broadcasts on.
     * @param certificate The CURVE certificate to use for encrypting the TCP
     * traffic between hosts. If null, no encryption will be used.
     * @param session The session ID to use for this peer.
     * @param verbose Whether to enable more verbose output.
     */
    PeerDiscoveryDaemon( uint16_t discovery_port, zcert_t* certificate, const std::string& session,
                         bool verbose = false );

    /**
     * @brief Destroy the Peer Discovery Daemon object
     *
     * @note **Important!** The stop() method **must** be called to perform cleanup.
     * There are important threading implications that I do not completely understand.
     * All I can say is that putting the cleanup in the destructor runs at the wrong time,
     * and possibly in the wring thread.
     */
    ~PeerDiscoveryDaemon() {}

    /**
     * @brief Notify the networking component of this peer that the local clipboard has changed.
     *
     * @param contents The new plaintext contents of the local clipboard.
     */
    void receiveLocalClipboardUpdate( const std::string& contents );
    /**
     * @brief Register a callback to receive clipboard updates from a connected remote host.
     *
     * @param callback The callback to receive updates.
     */
    void registerOnRemoteClipboardUpdate( Utils::Functor<void( const std::string& )> callback );

    /**
     * @brief Start the Zyre node listening for p2p communications.
     */
    void start() override;
    /**
     * @brief Stop the Zyre node, and perform cleanup.
     *
     * @see ~PeerDiscoveryDaemon() for details on why this is not done in the destructor.
     */
    void stop() override;

protected:
    /**
     * @brief The method to run in the event loop for this thread.
     *
     * Roughly, this thread blocks and listens for messages from peers. These fall into several
     * categories:
     * * **ENTER** the network.
     * * The node can be marked as **EVASIVE**, indicating it hasn't been heard from recently.
     * * **EXIT** the network.
     * * **JOIN** a group in the network.
     * * **LEAVE** a group in the network.
     * * **WHISPER** a message to a single peer.
     * * **SHOUT** a message to all connected peers in the specified group.
     */
    void loop() override;
    /**
     * @brief Receive a message from a connected peer, parse, and dispatch on its type.
     *
     * @param msg The message received from the connected peer.
     */
    void parseMessage( zmsg_t* msg );

private:
    uint16_t m_discovery_port;
    const bool m_verbose;
    const std::string& m_session;
    zcert_t* m_zcert;
    zyre_t* m_znode;

    Utils::Delegate<void( const std::string& )> m_remote_update_delegate;
};
} // namespace Clipd::Network
