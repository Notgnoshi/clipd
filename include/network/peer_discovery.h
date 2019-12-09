#pragma once
#include "common.h"
#include "utils/daemon.h"
#include "utils/delegate.h"
#include "utils/functor.h"

#include <czmq.h>
#include <zcert.h>
#include <zyre.h>

#include <iostream>

namespace Clipd::Network
{
//! @todo Discuss how Zyre implements its peer discovery and messaging.
//! See http://hintjens.com/blog:32 and https://rfc.zeromq.org/spec:36/ZRE/
class PeerDiscoveryDaemon : public Utils::Daemon
{
public:
    PeerDiscoveryDaemon( uint16_t discovery_port, zcert_t* certificate, const std::string& session,
                         bool verbose = false );
    ~PeerDiscoveryDaemon() {}

    void receiveLocalClipboardUpdate( const std::string& contents );
    void registerOnRemoteClipboardUpdate( Utils::Functor<void( const std::string& )> callback );

    void start() override;
    void stop() override;

protected:
    void loop() override;
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
