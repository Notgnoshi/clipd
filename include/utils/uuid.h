#pragma once

#include "common.h"

#include <iomanip>
#include <limits>
#include <random>
#include <sstream>
#include <string>

namespace Clipd::Utils
{
/**
 * @brief Generate random integers.
 *
 * @tparam Integral An integer type.
 */
template <typename Integral>
inline Integral randint()
{
    static std::random_device rd;
    static std::mt19937 gen( rd() );
    static std::uniform_int_distribution<Integral> dist( std::numeric_limits<Integral>::min(),
                                                         std::numeric_limits<Integral>::max() );

    return dist( gen );
}

/**
 * @brief Generate a random hex string of the specified number of bytes.
 *
 * @param bytes The number of two-character byte groups to generate.
 * @return A string of random hex characters.
 */
std::string random_hex( size_t bytes )
{
    std::stringstream ss;

    for( size_t i = 0; i < bytes; ++i )
    {
        // While the generated bytes will be in the range `[0, 255]`, cast to
        // `unit16_t` because `std::hex` treats `uint8_t` values differently.
        ss << std::setfill( '0' ) << std::setw( 2 ) << std::hex
           << static_cast<uint16_t>( randint<uint8_t>() );
    }

    return ss.str();
}

} // namespace Clipd::Utils
