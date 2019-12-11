# Clipd

Peer-to-peer X11 clipboard synchronization.

## Dependencies

Many of the dependencies are included as project submodules in `depends/`.
**Thus, you must clone with `git clone --recurse-submodules https://github.com/Notgnoshi/clipd.git`**, or clone as usual, followed by `git submodule update --init --recursive`.

Other non-vendored dependencies are

* doxygen
* graphviz
* autoconf, libtool
* clang, clang-format, and clang-tidy
* libx11-dev, libpng-dev, uuid-dev

Install the required dependencies with

```bash
sudo apt install autoconf libtool clang libx11-dev libpng-dev uuid-dev
```

and the optional ones with

```bash
sudo apt install doxygen graphviz clang-format clang-tidy
```

The project is built by running `make`.
This project *does* build on the Opp Lab machines, if the compiler version and dependencies preclude building locally.
The project statically links against the vendored dependencies, but dynamically links the libraries installed through `apt`.

The makefile places the built executable at `build/main`.

## Documentation

Run `make docs` and `make viewdocs` to build and open the developer documentation.

You may also run `make help` to display all of the (documented) make targets.

## Usage

See the following usage.

```default
$ build/main --help
DESCRIPTION
    Peer-to-peer X11 clipboard synchronization.

SYNOPSIS
        build/main [-h] [-v] [-p] [-e <certificate>] [-g <certificate>] [-s <ID>]

OPTIONS
        -h, --help  Show this help page.
        -v, --verbose
                    Increase output verbosity.

        -p, --port  The port to use for peer discovery.
        -e, --encrypt <certificate>
                    Encrypt traffic using the given certificate.

        -g, --generate <certificate>
                    Generate a certificate.

        -s, --session <ID>
                    The session ID to join for this peer.
```

## Network Architecture

@see Clipd::Network::PeerDiscoveryDaemon for details on the peer discovery and messaging protocol.
