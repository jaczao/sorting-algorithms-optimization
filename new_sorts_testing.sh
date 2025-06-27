#!/usr/bin/env bash
set -euo pipefail

#clang++ -std=c++20 benchmarks/benchmark_sorts.cpp src/branchless_fixed_sorts.cpp src/alpha_dev_sorts_arm64.cpp src/sorts.cpp -Iinclude -I/opt/homebrew/include -L/opt/homebrew/lib -lbenchmark -lpthread -O3 -o benchmark

clang++ -std=c++20 tests/test_sorts.cpp src/branchless_fixed_sorts.cpp src/alpha_dev_sorts_arm64.cpp src/sorts.cpp -Iinclude -I/opt/homebrew/include -L/opt/homebrew/lib -lgtest -lgtest_main -pthread -O3 -o run_tests

./run_tests

#./benchmark --benchmark_out=./outputs/results_new_sorts_testing.json \
#  int32,float 1000,100000 BranchlessSN,InsertionSort TimSort 32,64 FullType Random

#./benchmark --benchmark_out=./outputs/results_new_sorts_testing.json \
#  int32,float 1000,100000 BranchlessSN,InsertionSort TimSort 32,64 FullType NaturalRuns