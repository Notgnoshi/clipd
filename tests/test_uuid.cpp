#include "uuid.h"

#include <list>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::MatchesRegex;
using ::testing::SizeIs;

TEST( UuidTests, TestUuidLength )
{
    constexpr size_t num_examples = 100;
    constexpr size_t base_byte_length = 64;

    for( size_t i = 0; i < num_examples; ++i )
    {
        const size_t num_bytes = base_byte_length + i;
        const std::string uuid = random_hex( num_bytes );
        SCOPED_TRACE( "Matching uuid: " + uuid );
        // There should be exactly 2 characters per byte.
        EXPECT_THAT( uuid, SizeIs( 2 * num_bytes ) );
        EXPECT_THAT( uuid, MatchesRegex( "[0-9a-f]+" ) );
    }
}

TEST( UuidTests, TestUuidUniqueness )
{
    constexpr size_t num_examples = 500;
    constexpr size_t num_bytes = 4;

    std::list<std::string> uuids;

    for( size_t i = 0; i < num_examples; ++i )
    {
        uuids.push_back( random_hex( num_bytes ) );
    }

    uuids.sort();
    uuids.unique();

    EXPECT_THAT( uuids, SizeIs( num_examples ) ) << "Expected no duplicate UUIDS.";
}
