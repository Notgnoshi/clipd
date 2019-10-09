#include "clipboard/clipboard_daemon.h"

#include <clip.h>

#include <iostream>

namespace Clipd::Clipboard
{
void ClipboardDaemon::registerOnTextUpdate( Functor<void( const std::string& )> callback )
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
    std::string clipboard_contents = this->getClipboardTextContents();

    //! @todo Find the right abstraction for calling all of the subscribed delegates.
    std::unique_lock<std::mutex> lock( m_callbacks_mutex );
    for( const auto& callback : m_callbacks )
    {
        //! @todo Call the callbacks only if there's a change in the contents.
        callback( clipboard_contents );
    }
}
} // namespace Clipd::Clipboard
