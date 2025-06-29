#!/usr/bin/env bash
set -euo pipefail

# 1) Pulisce gli artefatti
#    (tolgo solo i file prodotti, lascio la dir e il build.ninja)
ninja -C build -t clean
# oppure:
# cmake --build build --target clean

# 2) Ricompila
ninja -C build
