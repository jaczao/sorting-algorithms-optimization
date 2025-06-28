#!/usr/bin/env bash
set -euo pipefail

rm -rf build
mkdir -p build
cmake \
  -G "Ninja" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_TOOLCHAIN_FILE=toolchain-windows.cmake \
  -DBENCHMARK_ENABLE_WERROR=OFF \
  -DCMAKE_CXX_FLAGS="-Wno-old-style-cast -Wno-invalid-offsetof" \
  -DCMAKE_C_COMPILER=C:/dev-tools/msys64/mingw64/bin/clang.exe \
  -DCMAKE_CXX_COMPILER=C:/dev-tools/msys64/mingw64/bin/clang++.exe \
  -B build -S .
