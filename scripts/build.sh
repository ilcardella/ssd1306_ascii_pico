#!/usr/bin/env bash

set -euo pipefail

# Build the automated test suite
mkdir -p build
cd build
cmake -DCMAKE_INSTALL_PREFIX:PATH=/tmp/ssd1306_ascii_pico ..
cmake --build . --config Release -- -j $(nproc)

# Test install target
cmake --build . --config Release --target install -- -j $(nproc)
