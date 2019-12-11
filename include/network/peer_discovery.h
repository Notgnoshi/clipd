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
/**
 * @brief This Daemon manages peer-to-peer discovery and communication.
 *
 * @details The p2p discovery and communication is performed using the ZeroMq Realtime Exchange
 * Protocol (ZRE). This class uses the Zyre implementation of the ZRE protocol, which is built
 * on top of CZMQ, which is in turn built on top of libzmq.
 *
 * @par Peer Discovery
 * Zyre implements two methods for peer discovery, only one of which is specified by the ZRE RFC.
 * The ZRE protocol specifies the use of UDP broadcast beacons on port 5670 (assigned by IANA) to
 * discover peers. Each beacon also contains an ephemeral TCP port that each peer listens for
 * connections on.
 *
 * The discovery is based on UDP broadcasts, but actual messaging between peers is performed over
 * TCP connections.
 *
 * Peer discovery can also be implemented using "gossiping". UDP broadcasts are limited to a peer's
 * subnet, but gossip discovery is performed using an endpoint configured *a priori*. A peer sends
 * a list of all connected hosts to any peer it "gossips" with. This is how you might define a p2p
 * network across multiple subnets; put a "gateway" peer on the edge of each subnet, and configure
 * them to gossip with the other gateway peers.
 *
 * However, the limitation of gossiping is that it is not *ad hoc*, the gossiping must be configured
 * explicitly. A single Zyre node may only use one or the other discovery protocol, but there is no
 * limitation that a single peer implement only one Zyre node.
 *
 * @note This application uses UDP broadcasts, not explicitly configured gossiping.
 *
 * @par Sessions
 *
 * Connected peers are arranged into groups to facilitate different messaging topologies.
 * This application arranges peers into *sessions* to allow multiple users to sync multiple
 * clipboards on the same network segment. Peers from different sessions will still discover each
 * other, but they will only send and receive clipboard updates to peers in the same session.
 *
 * A peer's session can be configured through the application's `--session <session name>` argument.
 * By default, a peer will join the "global" session.
 *
 * @par Peer-to-Peer Messaging
 *
 * The ZRE protocol defines the following message types:
 *
 * * HELLO. The HELLO command initiate a connection. Any messages received before a HELLO command
 *   is received will be discarded.
 * * WHISPER. The WHISPER command is used to message a *single* peer.
 * * SHOUT. The SHOUT command is used to broadcast a message to all peers in a group.
 * * JOIN. A node broadcasts a JOIN command to all its peers when it joins a group.
 * * LEAVE. A node broadcasts a LEAVE command to all its peers when it leaves a group.
 * * PING. A node can explicitly single out and PING a single peer that it hasn't heard a UDP
 *   broadcast recently. If a peer does not respond to a PING, it should be treated as dead.
 * * PING-OK. A node responds to a PING with a PING-OK command.
 *
 * The TCP messaging can be encrypted using a czmq zcert_t, but this requires building libzmq, czmq,
 * and zyre with `--enable-drafts`, and defining the `ZYRE_BUILD_DRAFT_API` preprocessor definition.
 * To encrypt the TCP traffic, generate a CURVE certificate by running the application with the
 * `--generate <path/to/cert>` option. Afterwards, configure a peer to use encrypted traffic by
 * passing the `--encrypt <path/to/cert>` commandline argument.
 *
 * @see https://github.com/zeromq/zyre for an implementation of the ZRE protocol.
 * @see https://github.com/zeromq/czmq for "high level" C bindings for libzmq
 * @see https://rfc.zeromq.org/spec:36/ZRE/ for the technical specification.
 * @see http://hintjens.com/blog:32 for a descriptive blog post by the protocol author.
 */
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
