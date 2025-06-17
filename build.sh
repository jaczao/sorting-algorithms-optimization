#!/usr/bin/env bash
set -euo pipefail

# 1) Pulisce gli artefatti
#    (tolgo solo i file prodotti, lascio la dir e il build.ninja)
ninja -C build -t clean
# oppure:
# cmake --build build --target clean

# 2) Ricompila
ninja -C build

# 3) Esegue il benchmark
./build/benchmark_app \
  --benchmark_format=json \
  --benchmark_out=./outputs/results.json

# 4) Genera i grafici
source venv_grafici/bin/activate
python ./benchmarks/generate_graphs.py ./outputs/results.json
deactivate
