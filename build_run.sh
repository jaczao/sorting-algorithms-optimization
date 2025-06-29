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

./build/benchmark_app --benchmark_out=./outputs/results1.json \
  int32 3,4,5,6,7,8,16,32,64,128 AlphaDev SelectionSort,BubbleSort,InsertionSort 1 FullType Random,NaturalRuns

./build/benchmark_app --benchmark_out=./outputs/results2.json \
  int32 10000 AlphaDev,BranchlessSN QuickSort,IntroSort,TimSort 3,4,5,6,7,8 FullType Random

./build/benchmark_app --benchmark_out=./outputs/results3.json \
  int32 10000 AlphaDev,InsertionSort QuickSort,IntroSort,TimSort 8,16,32,64 FullType Random

./build/benchmark_app --benchmark_out=./outputs/results4.json \
  int64,float 10000 BranchlessSN,InsertionSort QuickSort,IntroSort,TimSort 8,16,32,64 FullType Random

./build/benchmark_app --benchmark_out=./outputs/results5.json \
  int32 10,100,1000,10000,100000,1000000 AlphaDev StdSort,IntroSort,TimSort,QuickSort,MergeSort 1,64 FullType Random

./build/benchmark_app --benchmark_out=./outputs/results6.json \
  int64 10,100,1000,10000,100000,1000000 BranchlessSN,InsertionSort StdSort,IntroSort,TimSort,QuickSort,MergeSort 1,64 FullType Random

./build/benchmark_app --benchmark_out=./outputs/results7.json \
  float 10,100,1000,10000,100000 BranchlessSN,InsertionSort StdSort,IntroSort,TimSort,QuickSort,MergeSort 1,64 FullType Random

./build/benchmark_app --benchmark_out=./outputs/results8.json \
  int32 10,100,1000,10000,100000,1000000 AlphaDev StdSort,IntroSort,TimSort,MergeSort 1,64 FullType Ascending,Descending,PipeOrgan,Constant,MostlySorted,NaturalRuns

