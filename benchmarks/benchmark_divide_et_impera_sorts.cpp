#include <benchmark/benchmark.h>
#include <algorithm>
#include <random>
#include <vector>
#include "divide_et_impera_sorts.h"

using namespace std;

// --- helper per riempire ---
static void fill_random(vector<int>& buf, mt19937_64& rng) {
    static uniform_int_distribution<int> dist(-1'000'000, 1'000'000);
    for (auto& x : buf) x = dist(rng);
}

// Template per benchmark di MergeSort
static void BM_MergeSort(benchmark::State& state, int maxSmall, SmallSortStrategy strat) {
    size_t N = state.range(0);
    vector<int> original(N);
    mt19937_64 rng(123456);
    fill_random(original, rng);
    for (auto _ : state) {
        // copia per nondisturbare l'input originale
        vector<int> buf = original;
        mergeSort(buf.data(), 0, static_cast<int>(buf.size()) - 1, maxSmall, strat);
    }
    state.SetComplexityN(N);
}

// Template per benchmark di QuickSort
static void BM_QuickSort(benchmark::State& state, int maxSmall, SmallSortStrategy strat) {
    size_t N = state.range(0);
    vector<int> original(N);
    mt19937_64 rng(654321);
    fill_random(original, rng);
    for (auto _ : state) {
        vector<int> buf = original;
        quickSort(buf.data(), 0, static_cast<int>(buf.size()) - 1, maxSmall, strat);
    }
    state.SetComplexityN(N);
}

// Registrazione dei benchmark
// Array sizes da testare
static const int sizes[] = {32, 1024}; //{1,3,4,5,6,7,8,16,64,256,1024,16384,262144};
// Tutti i valori di maxSmall
static const int maxSmalls[] = {1,2,3,4,5,6,7,8};
// Strategie disponibili
static const SmallSortStrategy strategies[] = {
    SmallSortStrategy::AlphaDev
    //SmallSortStrategy::Branchless,
    //SmallSortStrategy::Branching
};
// Nomina delle strategie per i benchmark
static const char* strat_names[] = {"AlphaDev", "Branchless", "Branching"};

    int main(int argc, char** argv) {
        benchmark::Initialize(&argc, argv);
    
        // 1) Tutti i QuickSort
        for (auto N : sizes) {
            for (auto ms : maxSmalls) {
                for (int s = 0; s < sizeof(strategies) / sizeof(strategies[0]) ; ++s) {
                    auto strat = strategies[s];
                    string name = string("Quick") //+ to_string(N)
                        + "_maxSmall" + to_string(ms)
                        + "_" + strat_names[s];
                    benchmark::RegisterBenchmark(
                        name.c_str(), BM_QuickSort, ms, strat)
                        ->Arg(N)
                        ->Complexity();
                }
            }
        }
    
        // 2) Tutti i MergeSort
        for (auto N : sizes) {
            for (auto ms : maxSmalls) {
                for (int s = 0; s < sizeof(strategies) / sizeof(strategies[0]) ; ++s) {
                    auto strat = strategies[s];
                    string name = string("Merge") //+ to_string(N)
                        + "_maxSmall" + to_string(ms)
                        + "_" + strat_names[s];
                    benchmark::RegisterBenchmark(
                        name.c_str(), BM_MergeSort, ms, strat)
                        ->Arg(N)
                        ->Complexity();
                }
            }
        }
        
    benchmark::RunSpecifiedBenchmarks();
    return 0;
}
