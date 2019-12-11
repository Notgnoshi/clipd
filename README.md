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
* libx11-dev, libpng-dev, uuid-dev

Install the required dependencies with

```bash
sudo apt install autoconf libtool clang libx11-dev libpng-dev uuid-dev
```

and the optional ones with

```bash
sudo apt install doxygen graphviz clang-format clang-tidy
```

## Documentation

Run `make docs` and `make viewdocs` to build and open the developer documentation.

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
