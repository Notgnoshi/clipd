#pragma once
#include "common.h"

namespace Clipd::Network
{
/**
 * @brief An object to store details for connected peers in the network.
 */
struct Peer
{
    uint32_t uuid;     //!< The connected Peer's UUID.
    uint16_t msg_port; //!< The connected Peer's TCP port for messaging
    //! @todo Store the peer's endpoint? Or just store the peer's IP address.

    /**
     * @brief Construct a new Peer object.
     *
     * @param uuid The UUID for the connected peer.
     * @param msg_port The TCP port the connected peer uses for messaging.
     */
    Peer( uint32_t uuid, uint16_t msg_port ) : uuid( uuid ), msg_port( msg_port ) {}
};
} // namespace Clipd::Network
