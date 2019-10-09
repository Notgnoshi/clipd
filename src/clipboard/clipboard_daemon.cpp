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

    m_text_delegate( clipboard_contents );
}
} // namespace Clipd::Clipboard
