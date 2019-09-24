#pragma once
#include "common.h"
#include "daemon.h"

#include <atomic>
#include <functional>
#include <list>
#include <mutex>
#include <string>
#include <thread>

namespace Clipd::Clipboard
{
class ClipboardDaemon : public Daemon
{
public:
    explicit ClipboardDaemon( bool verbose ) : m_verbose( verbose ) {}

    /**
     * @brief Register a callback to be called whenever a text update occurs.
     *
     * @note Due to std::function implementation details, it is not possible to unregister a
     * callback once it has been registered.
     *
     * @param callback The callback to call with any text updates.
     */
    void registerOnTextUpdate( const std::function<void( const std::string& )>& callback );

private:
    /**
     * @brief The clipboard listener event loop body.
     */
    void loop() override;

private:
    const bool m_verbose;
    std::mutex m_callbacks_mutex;
    std::list<std::function<void( const std::string& )>> m_callbacks;
};
} // namespace Clipd::Clipboard
