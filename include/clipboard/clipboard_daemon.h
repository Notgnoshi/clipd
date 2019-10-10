#pragma once
#include "common.h"
#include "daemon.h"
#include "delegate.h"
#include "functor.h"

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
    explicit ClipboardDaemon( bool verbose = false ) : m_verbose( verbose ), m_curr_text_hash( 0 )
    {}

    /**
     * @brief Register a callback to be called whenever a text update occurs.
     *
     * @param callback The callback to call with any text updates.
     */
    void registerOnTextUpdate( Functor<void( const std::string& )> callback );

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
    size_t m_curr_text_hash;
    Delegate<void( const std::string& )> m_text_delegate;
};
} // namespace Clipd::Clipboard
