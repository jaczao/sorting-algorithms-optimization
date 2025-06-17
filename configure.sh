#!/usr/bin/env bash
set -euo pipefail

rm -rf build
mkdir -p build
cmake \
  -G "Ninja" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_TOOLCHAIN_FILE=toolchain-macos.cmake \
  -B build -S .
