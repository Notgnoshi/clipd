#include "args.h"
#include "common.h"

/**
 * @brief The main application entry point.
 *
 * @return Zero if the application ran successfully, non-zero otherwise.
 */
int main( int argc, const char** argv )
{
    Clipd::CommandlineArgs_t args = Clipd::ParseArgs( argc, argv );

    (void)args;

    return 0;
}
