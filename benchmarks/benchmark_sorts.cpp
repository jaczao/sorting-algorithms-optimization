#include <benchmark/benchmark.h>
#include <random>
#include <vector>
#include <limits>
#include <cstdint>
#include "sorts.h"

using namespace std;

// Tipi di dato
enum class SortType { Int32 = 0, Int64 = 1, Float = 2 };
static const char* SortTypeNames[] = {"int32","int64","float"};

// Strategie small sort
enum class SmallSortStrategyEnum { AlphaDev = 0, Branchless = 1, Branching = 2, Selection = 3, Insertion = 4, Bubble = 5 };
static const char* SmallSortNames[] = {"AlphaDev","Branchless","Branching","Selection","Insertion","Bubble"};

// Algoritmi di ordinamento
enum class SortAlgorithm { Quick = 0, Merge = 1, Heap = 2, Merge8 = 3, Selection = 4, Insertion = 5, Bubble = 6, Intro =7, StdSort=8, TimSort=9 };
static const char* SortAlgoNames[] = {"Quick","Merge","Heap","Merge8","Selection","Insertion","Bubble","Intro", "StdSort", "TimSort"};

// generico fill_random
template<typename T>
void fill_random(vector<T>& buf, mt19937_64& rng) {
    if constexpr (is_integral_v<T>) {
        uniform_int_distribution<T> dist(numeric_limits<T>::min(), numeric_limits<T>::max());
        for (auto& x : buf) x = dist(rng);
    } else {
        uniform_real_distribution<T> dist(numeric_limits<T>::lowest(), numeric_limits<T>::max());
        for (auto& x : buf) x = dist(rng);
    }
}

// Funzione di dispatch per ciascun algoritmo
template<typename T>
void runSort(T* data, int size, int maxSmall, SmallSortStrategyEnum ss, SortAlgorithm alg) {
    switch (alg) {
        case SortAlgorithm::Quick:
            quickSort(data, 0, size-1, maxSmall, static_cast<SmallSortStrategy>(ss));
            break;
        case SortAlgorithm::Merge:
            mergeSort(data, 0, size-1, maxSmall, static_cast<SmallSortStrategy>(ss));
            break;
        case SortAlgorithm::Heap:
            heapSort(data, size);
            break;
        case SortAlgorithm::Merge8:
            mergeSort8(data, 0, size-1, maxSmall, static_cast<SmallSortStrategy>(ss));
            break;
        case SortAlgorithm::Selection:
            selectionSort(data, size);
        break;
        case SortAlgorithm::Insertion:
            insertionSort(data, size);
        break;
        case SortAlgorithm::Bubble:
            bubbleSort(data, size);
        break;
        case SortAlgorithm::Intro:
            introSort(data, 0, size-1, maxSmall, static_cast<SmallSortStrategy>(ss));
        break;
        case SortAlgorithm::StdSort:
            std::sort(data, data+size);
        break;
        case SortAlgorithm::TimSort:
            timSort(data, size, maxSmall, static_cast<SmallSortStrategy>(ss));
        break;
    }
}

// Benchmark template
template<typename T>
void BM_All(benchmark::State& state, SortType t, SmallSortStrategyEnum ss, SortAlgorithm alg) {
    size_t N = state.range(0);
    vector<T> original(N);
    mt19937_64 rng(12345 + int(t)*100 + int(ss)*10 + int(alg));
    fill_random(original, rng);
    int maxSmall = state.range(1);
    for (auto _ : state) {
        vector<T> buf = original;
        runSort<T>(buf.data(), int(buf.size()), maxSmall, ss, alg);
        benchmark::ClobberMemory();
    }
    state.SetComplexityN(N);
}

int main(int argc, char** argv) {
    benchmark::Initialize(&argc, argv);

    SortType types[] = { SortType::Int32};//, SortType::Int64, SortType::Float };
    int sizes[]     = {12345678, 123456789};//{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31};//{1};
    SmallSortStrategyEnum ssArr[] = { SmallSortStrategyEnum::AlphaDev, SmallSortStrategyEnum::Insertion//, SmallSortStrategyEnum::Branchless//, SmallSortStrategyEnum::Branchless//SmallSortStrategyEnum::AlphaDev, SmallSortStrategyEnum::Branchless, SmallSortStrategyEnum::Branching,
                                     };//SmallSortStrategyEnum::Selection, SmallSortStrategyEnum::Insertion, SmallSortStrategyEnum::Bubble };
    SortAlgorithm algs[] = { SortAlgorithm::TimSort};//, SortAlgorithm::Heap, SortAlgorithm::StdSort, SortAlgorithm::Intro}; //SortAlgorithm::Quick, SortAlgorithm::Merge, SortAlgorithm::Heap, SortAlgorithm::Intro, SortAlgorithm::StdSort, SortAlgorithm::Selection, SortAlgorithm::Insertion, SortAlgorithm::Bubble, 
    int maxSmalls[] = {8, 32};//{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32};//1,2,3,4,5,6,7,8};

    for (auto t : types) {
        for (auto N : sizes) {
            for (auto ss : ssArr) {
                for (auto alg : algs) {
                    for (auto ms : maxSmalls) {
                        // Nome: type_size_ss_alg_maxSmall
                        string name = string(SortTypeNames[int(t)])
                            + "_" + SmallSortNames[int(ss)]
                            + "_" + SortAlgoNames[int(alg)]
                            + "_maxSmall" + to_string(ms);
                        switch (t) {
                            case SortType::Int32:
                                benchmark::RegisterBenchmark(name.c_str(), BM_All<int32_t>, t, ss, alg)
                                    ->Args({N, ms})->Complexity();
                                break;
                            case SortType::Int64:
                                benchmark::RegisterBenchmark(name.c_str(), BM_All<int64_t>, t, ss, alg)
                                    ->Args({N, ms})->Complexity();
                                break;
                            case SortType::Float:
                                benchmark::RegisterBenchmark(name.c_str(), BM_All<float>, t, ss, alg)
                                    ->Args({N, ms})->Complexity();
                                break;
                        }
                    }
                }
            }
        }
    }

    benchmark::RunSpecifiedBenchmarks();
    return 0;
}