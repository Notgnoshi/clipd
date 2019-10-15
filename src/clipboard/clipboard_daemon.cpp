#include "clipboard/clipboard_daemon.h"

#include <clip.h>

#include <chrono>
#include <thread>

namespace Clipd::Clipboard
{
void ClipboardDaemon::registerOnTextUpdate( Utils::Functor<void( const std::string& )> callback )
{
    m_text_delegate.subscribe( std::move( callback ) );
}

std::string ClipboardDaemon::getClipboardTextContents() const
{
    std::string contents;
    clip::get_text( contents );
    return contents;
}

void ClipboardDaemon::loop()
{
    using namespace std::chrono_literals;
    std::string clipboard_contents = this->getClipboardTextContents();

    // This isn't a cryptographically secure hash (it's even worse than MD5), but I believe that it
    // will suffice for now.
    std::hash<std::string> hasher;
    size_t hash = hasher( clipboard_contents );

    if( hash != m_curr_text_hash )
    {
        m_curr_text_hash = hash;
        m_text_delegate( clipboard_contents );
    }

    // Limit how quickly we query the X11 clipboard.
    std::this_thread::sleep_for( 50ms );
}
} // namespace Clipd::Clipboard
