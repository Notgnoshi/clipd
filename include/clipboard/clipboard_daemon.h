#pragma once
#include "common.h"
#include "utils/daemon.h"
#include "utils/delegate.h"
#include "utils/functor.h"

#include <atomic>
#include <functional>
#include <list>
#include <mutex>
#include <string>
#include <thread>

namespace Clipd::Clipboard
{
class ClipboardDaemon : public Utils::Daemon
{
public:
    /**
     * @brief Register a callback to be called whenever a text update occurs.
     *
     * @param callback The callback to call with any text updates.
     */
    void registerOnTextUpdate( Utils::Functor<void( const std::string& )> callback );

protected:
    /**
     * @brief Get the current X11 clipboard contents as plaintext.
     *
     * @details This method is defined, and is marked as virtual to allow a unit testing
     * framework to mock it, and provide their own values rather than querying the clipboard.
     */
    [[nodiscard]] virtual std::string getClipboardTextContents() const;

private:
    /**
     * @brief The clipboard listener event loop body.
     */
    void loop() override;

private:
    size_t m_curr_text_hash = 0;
    Utils::Delegate<void( const std::string& )> m_text_delegate;
};
} // namespace Clipd::Clipboard
