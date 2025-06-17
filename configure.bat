@echo off
setlocal enabledelayedexpansion

REM 1) Rimuovi la cartella build (quietamente)
rmdir /s /q build 2>nul || (
    REM se non esiste, ignora
    echo Build directory does not exist, skipping removal
)

REM 2) Crea la directory build
mkdir build 2>nul

REM 3) Genera i file Ninja con toolchain Windows
cmake -G "Ninja" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_TOOLCHAIN_FILE="toolchain-windows.cmake" \
  -DBENCHMARK_ENABLE_WERROR=OFF \
  -DCMAKE_CXX_FLAGS="-Wno-old-style-cast -Wno-invalid-offsetof" \
  -DCMAKE_C_COMPILER=/mingw64/bin/clang.exe \
  -DCMAKE_CXX_COMPILER=/mingw64/bin/clang++.exe \
  -B build -S .