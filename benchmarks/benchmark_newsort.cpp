#include <benchmark/benchmark.h>
#include <random>
#include <vector>
#include <limits>
#include <cstdint>
#include <cmath> // per log2
#include "sorts.h"

using namespace std;

// Utility: genera un vettore casuale di interi.
std::vector<int> generate_random_vector(size_t size) {
    std::vector<int> vec(size);
    std::mt19937 rng(42); // seme fisso per riproducibilità
    std::uniform_int_distribution<int> dist(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
    for (auto &x : vec)
        x = dist(rng);
    return vec;
}

// Nota: la costante 'AlphaDev' è usata come strategia per smallSort.
static const SmallSortStrategy strat = SmallSortStrategy::AlphaDev; // strategia di small sort di Alphadev

// BM_NewSort ora riceve: state, size, maxAus, strat2.
static void BM_NewSort(benchmark::State &state, int size, int maxSmall, int maxAux, SmallSortStrategy strat2) {
    auto original = generate_random_vector(size);
    for (auto _ : state) {
        auto data = original;
        newSort(data.data(), 0, data.size()-1, maxSmall, maxAux, strat, strat2);
        benchmark::ClobberMemory();
    }
}

// Gli altri algoritmi ricevono: state, size.
static void BM_IntroSort(benchmark::State &state, int size) {
    auto original = generate_random_vector(size);
    for (auto _ : state) {
        auto data = original;
        introSort(data.data(), 0, data.size()-1, 32, strat); // 32 posso metterlo parametro
        benchmark::ClobberMemory();
    }
}

static void BM_MergeSort(benchmark::State &state, int size) {
    auto original = generate_random_vector(size);
    for (auto _ : state) {
        auto data = original;
        mergeSort(data.data(), 0, data.size()-1, 32, strat);
        benchmark::ClobberMemory();
    }
}

static void BM_TimSort(benchmark::State &state, int size) {
    auto original = generate_random_vector(size);
    for (auto _ : state) {
        auto data = original;
        timSort(data.data(), data.size());
        benchmark::ClobberMemory();
    }
}

// Aggiungi benchmark per std::sort.
static void BM_StdSort(benchmark::State &state, int size) {
    auto original = generate_random_vector(size);
    for (auto _ : state) {
        auto data = original;
        std::sort(data.begin(), data.end());
        benchmark::ClobberMemory();
    }
}

// Aggiungi benchmark per heapSort.
static void BM_HeapSort(benchmark::State &state, int size) {
    auto original = generate_random_vector(size);
    for (auto _ : state) {
        auto data = original;
        heapSort(data.data(), data.size());
        benchmark::ClobberMemory();
    }
}

// Aggiungi benchmark per quickSort.
static void BM_QuickSort(benchmark::State &state, int size) {
    auto original = generate_random_vector(size);
    for (auto _ : state) {
        auto data = original;
        quickSort(data.data(), 0, data.size()-1, 32, strat);
        benchmark::ClobberMemory();
    }
}

// Registrazione dinamica dei benchmark:
// Per newSort si testano tutte le combinazioni dei parametri su 4 dimensioni.
namespace {
    std::vector<int> sizes   =  { 100000, 200000, 500000, 1000000, 2000000, 5000000};
    std::vector<int> maxSmalls = {32};

    /*struct RegisterNewSort {
        RegisterNewSort() {
            std::vector<int> maxAux_vals = {1024};
            for (auto s : sizes)
                for (auto maxSmall : maxSmalls)
                    for (auto maxAux : maxAux_vals)
                        for (auto strat2 : {SmallSortStrategy::Heap}) { //SmallSortStrategy::Insertion, 
                            std::string strat2_name = (strat2 == SmallSortStrategy::Insertion) ? "Insertion" : "Heap";
                            std::string name = "BM_NewSort/size=" + std::to_string(s) +
                                               ",maxsmall=" + std::to_string(maxSmall) +
                                               ",maxaux=" + std::to_string(maxAux) +
                                               ",strat2=" + strat2_name;
                            benchmark::RegisterBenchmark(name.c_str(), BM_NewSort, s, maxSmall, maxAux, strat2);
                        }
        }
    } registerNewSort;*/
    
    // Per introsort, mergesort, timsort, std::sort e heapSort si testano 4 dimensioni.
    struct RegisterOtherSorts {
        RegisterOtherSorts() {
            for (auto s : sizes) {
                std::string name7 = "BM_NewSort/size=" + std::to_string(s);
                benchmark::RegisterBenchmark(name7.c_str(), BM_NewSort, s, 32, 32, SmallSortStrategy::Insertion);
                /*std::string name2 = "BM_MergeSort/size=" + std::to_string(s);
                benchmark::RegisterBenchmark(name2.c_str(), BM_MergeSort, s);
                std::string name5 = "BM_HeapSort/size=" + std::to_string(s);
                benchmark::RegisterBenchmark(name5.c_str(), BM_HeapSort, s);
                std::string name6 = "BM_QuickSort/size=" + std::to_string(s);
                benchmark::RegisterBenchmark(name6.c_str(), BM_QuickSort, s);
                std::string name1 = "BM_IntroSort/size=" + std::to_string(s);
                benchmark::RegisterBenchmark(name1.c_str(), BM_IntroSort, s);
                std::string name3 = "BM_TimSort/size=" + std::to_string(s);
                benchmark::RegisterBenchmark(name3.c_str(), BM_TimSort, s);*/
                std::string name4 = "BM_StdSort/size=" + std::to_string(s);
                benchmark::RegisterBenchmark(name4.c_str(), BM_StdSort, s);
            }
        }
    } registerOtherSorts;
}

BENCHMARK_MAIN();


