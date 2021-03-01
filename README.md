# ssd1306_ascii_pico

This project is a port of the [Arduino SSD1306Ascii library](https://github.com/greiman/SSD1306Ascii) for a Raspberry Pi Pico.

## Getting Started

The project is configured to build targeting the RP2040 platform which is at the core of the Raspberry PI Pico board.

### Build

The `Makefile` at the project root directory provides targets to create the build docker image and to build the code itself:

```bash
# Create the build docker image
$ make docker
# Build the source code inside a docker container
$ make build
```

The build process runs inside a Docker container allowing isolation and easier dependencies management.
However you can trigger a build in your local environment without using Docker

```bash
$ ./scripts/build.sh
```

### Install

The library is installed in `/usr/local` space and targets are exported to allow CMake projects to find the library with `find_package(ssd1306_ascii_pico CONFIG REQUIRED)`

Install the library with:

```bash
$ make install
```

### Example

Check out the `example` directory which contains a simple CMake configuration and C++ main depending on the library.
