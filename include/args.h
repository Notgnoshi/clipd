#pragma once

namespace Clipd
{
struct CommandlineArgs_t
{
    bool verbose = false; //!< Whether to use verbose output
    bool help = false;    //!< Whether user requested the help option.
    bool clipd = false;   //!< Whether to listen to changes in the X11 clipboard.
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

} // namespace Clipd
