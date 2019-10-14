#include "clipboard/clipboard_daemon.h"

#include <chrono>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace std::chrono_literals;

using ::testing::AtLeast;
using ::testing::InSequence;
using ::testing::Return;
using ::testing::StrictMock;

class MockClipboardDaemon : public Clipd::Clipboard::ClipboardDaemon
{
public:
    MOCK_METHOD( std::string, getClipboardTextContents, (), ( const, override ) );
    MOCK_METHOD( void, TextUpdateCallback, (const std::string&));
};

TEST( ClipboardListenerTests, TestUniqueCallbackCalls )
{
    MockClipboardDaemon listener;
    listener.registerOnTextUpdate(
        Functor<void( const std::string& )>( listener, &MockClipboardDaemon::TextUpdateCallback ) );

    // Ensure that the clipboard listener will always return the constant value "v".
    ON_CALL( listener, getClipboardTextContents() ).WillByDefault( Return( "v" ) );

    EXPECT_CALL( listener, getClipboardTextContents() ).Times( AtLeast( 2 ) );
    EXPECT_CALL( listener, TextUpdateCallback( "v" ) ).Times( 1 );

    listener.start();

    // Give the listener thread enough time to start up and run for at least two iterations, with
    // each iteration sleeping for 50ms.
    std::this_thread::sleep_for( 100ms );

    listener.stop();
    listener.join();
}
