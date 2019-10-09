#include "clipboard/clipboard_daemon.h"

#include <clip.h>

#include <iostream>

namespace Clipd::Clipboard
{
void ClipboardDaemon::registerOnTextUpdate( delegate<void( const std::string& )> callback )
{
    std::unique_lock<std::mutex> lock( m_callbacks_mutex );

    m_callbacks.push_back( callback );
}

std::string ClipboardDaemon::getClipboardTextContents() const
{
    std::string contents;
    clip::get_text( contents );
    return contents;
}

void ClipboardDaemon::loop()
{
    //! @todo Do things.
    (void)m_verbose;
}
} // namespace Clipd::Clipboard
