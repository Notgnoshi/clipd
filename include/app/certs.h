#pragma once

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
void GenerateCertificate( fs::path cert );
}
