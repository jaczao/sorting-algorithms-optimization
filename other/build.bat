@echo off
setlocal enabledelayedexpansion

REM 1) Pulisce gli artefatti generati
ninja -C build -t clean

REM 2) Ricompila tutto
ninja -C build

REM 3) Esegue il benchmark
build\benchmark_app.exe ^
  --benchmark_out=outputs\results.json

REM 4) Genera i grafici Python
call venv_grafici\Scripts\activate.bat
python benchmarks\generate_graphs.py outputs\results.json
call venv_grafici\Scripts\deactivate.bat