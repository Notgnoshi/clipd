#include "clipboard/clipboard_daemon.h"

#include <clip.h>

#include <iostream>

namespace Clipd::Clipboard
{
void ClipboardDaemon::registerOnTextUpdate( Functor<void( const std::string& )> callback )
{
    m_text_delegate.subscribe( callback );
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

    // This isn't a cryptographically secure hash (it's even worse than MD5), but I believe that it
    // will suffice for now.
    std::hash<std::string> hasher;
    size_t hash = hasher( clipboard_contents );

    if( hash != m_curr_text_hash )
    {
        if( m_verbose )
        {
            std::cout << "Received clipboard update: " << clipboard_contents << std::endl;
        }

        m_curr_text_hash = hash;
        m_text_delegate( clipboard_contents );
    }
}
} // namespace Clipd::Clipboard
