# Clipd

Peer-to-peer X11 clipboard synchronization.

## Dependencies

Many of the dependencies are included as project submodules in `depends/`.
The dependencies can be manually build by running `make depends`, or automatically as a part of building the main application with just `make`.

Other non-vendored dependencies are

* doxygen
* graphviz
* autoconf, libtool
* clang, clang-format, and clang-tidy
* libx11-dev

## Documentation

Run `make docs` and `make viewdocs` to build and open the developer documentation.

## Usage

@todo Add commandline usage.

## Network Architecture

@todo Expound on the peer-to-peer discovery and message transmission.
