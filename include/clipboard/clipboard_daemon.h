#pragma once
#include "common.h"
#include "daemon.h"
#include "delegate.h"

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
     * @param callback The callback to call with any text updates.
     */
    void registerOnTextUpdate( delegate<void( const std::string& )> callback );

protected:
    /**
     * @brief Get the current X11 clipboard contents as plaintext.
     *
     * @details This method is defined, and is marked as virtual to allow a unit testing
     * framework to mock it, and provide their own values rather than querying the clipboard.
     */
    virtual std::string getClipboardTextContents() const;

private:
    /**
     * @brief The clipboard listener event loop body.
     */
    void loop() override;

private:
    const bool m_verbose;
    std::mutex m_callbacks_mutex;
    std::list<delegate<void( const std::string& )>> m_callbacks;
};
} // namespace Clipd::Clipboard
