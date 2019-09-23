#pragma once

#include "math/complex.h"

#include <optional>

namespace Boilerplate
{
struct CommandlineArgs_t
{
    bool verbose = false; //!< Whether to use verbose output
    Math::Complex value;  //!< value passed from commandline
};

/**
 * @brief Parse server commandline arguments.
 *
 * @param argc The number of commandline arguments.
 * @param argv An array of commandline arguments.
 * @return A CommandlineArgs_t struct if the arguments parsed and --help was not passed. An empty
 * std::optional otherwise
 */
std::optional<CommandlineArgs_t> ParseArgs( int argc, const char** argv );

} // namespace Boilerplate
