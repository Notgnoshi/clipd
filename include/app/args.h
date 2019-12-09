#pragma once
#include "common.h"

// For some reason the standard feature test macros aren't defined?
// #if defined(__cpp_lib_filesystem)
#if __has_include( <filesystem>)
#include <filesystem>
namespace fs = ::std::filesystem;
// #elif defined(__cpp_lib_experimental_filesystem)
#elif __has_include( <experimental/filesystem>)
#include <experimental/filesystem>
namespace fs = ::std::experimental::filesystem;
#else
#error "Shit's broken, yo. No filesystem for you."
#endif

namespace Clipd::App
{
struct CommandlineArgs_t
{
    bool verbose = false;        //!< Whether to use verbose output
    bool help = false;           //!< Whether user requested the help option.
    uint16_t discovery_port = 0; //!< The port to perform peer discovery on.

    bool generate_certificate = false; //!< Whether to generate a CURVE certificate.
    bool encrypt_traffic = false;      //!< Whether to encrypt traffic with a CURVE certificate.
    fs::path certificate;              //!< The path to the certificate public key.

    std::string session = "global"; //!< The session ID for this peer to join.
};

/**
 * @brief Parse application commandline arguments.
 *
 * @details This function will exit the program in two cases
 * 1. The user passed --help, in which case the help page will be displayed, and the program will
 *    exit with a zero exit status.
 * 2. The parser failed to parse the given arguments, in which case the help page will be displayed,
 *    and the program will exit with a non-zero status.
 *
 * @param argc The number of commandline arguments.
 * @param argv An array of commandline arguments.
 * @return A CommandlineArgs_t struct if the arguments were successfully parsed.
 */
CommandlineArgs_t ParseArgs( int argc, const char** argv );

} // namespace Clipd::App
