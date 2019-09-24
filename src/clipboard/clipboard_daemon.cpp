#include "clipboard/clipboard_daemon.h"

#include <iostream>

namespace Clipd::Clipboard
{
void ClipboardDaemon::registerOnTextUpdate( const std::function<void( const std::string& )>& callback )
{
    std::unique_lock<std::mutex> lock( m_callbacks_mutex );

    m_callbacks.push_back( callback );
}

void ClipboardDaemon::loop()
{
    //! @todo Do things.
    (void)m_verbose;
}
} // namespace Clipd::Clipboard
