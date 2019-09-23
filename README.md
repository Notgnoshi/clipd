# Boilerplate C++ Project

This branch is a boilerplate C++ project that includes documentation, unit tests, and an example makefile.

## Dependencies

* `doxygen`
* `graphviz`
* `clang-format` and `clang-tidy`

## Documentation

* Run `make docs` to generate doxygen documentation. HTML documentation can be found in `build/html/index.html`.
* Build and run unit tests with `make runtests`.
* Run code formatter with `make format`. The `clang-format` configuration may be found in `.clang-format`.
* Run code linter with `make lint`. The configuration for the linter may be found in `.clang-tidy`.

## TODO

* Profiling and benchmarking targets.
* Avoid deleting the documentation when running `make clean`?
