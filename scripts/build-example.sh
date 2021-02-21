#!/usr/bin/env bash

set -euo pipefail

# Install the library
make install

# Build the example project
cd example
mkdir -p build
cd build
cmake ..
cmake --build . --config Release -- -j $(nproc)
