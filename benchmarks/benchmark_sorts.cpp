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

// Tipi di range per il riempimento random
enum class RandomRangeType { FullType = 0, Binary = 1, Small = 2, NRange = 3 };
static const char* RandomRangeNames[] = {"FullType", "Binary", "Small", "NRange"};

// generico fill_random con range selezionabile
template<typename T>
void fill_random(vector<T>& buf, mt19937_64& rng/*e se lo generassi qui senza averlo come param?*/, RandomRangeType rangeType, int N) {
    if constexpr (is_integral_v<T>) {
        switch (rangeType) {
            case RandomRangeType::FullType: {
                uniform_int_distribution<T> dist(numeric_limits<T>::min(), numeric_limits<T>::max());
                for (auto& x : buf) x = dist(rng);
                break;
            }
            case RandomRangeType::Binary: {
                uniform_int_distribution<int> dist(0, 1);
                for (auto& x : buf) x = static_cast<T>(dist(rng));
                break;
            }
            case RandomRangeType::Small: {
                int maxval = std::max(1, int(std::sqrt(N)));
                uniform_int_distribution<int> dist(0, maxval);
                for (auto& x : buf) x = static_cast<T>(dist(rng));
                break;
            }
            case RandomRangeType::NRange: {
                uniform_int_distribution<int> dist(0, std::max(0, N-1));
                for (auto& x : buf) x = static_cast<T>(dist(rng));
                break;
            }
        }
    } else {
        switch (rangeType) {
            case RandomRangeType::FullType: {
                uniform_real_distribution<T> dist(numeric_limits<T>::lowest(), numeric_limits<T>::max());
                for (auto& x : buf) x = dist(rng);
                break;
            }
            case RandomRangeType::Binary: {
                uniform_int_distribution<int> dist(0, 1);
                for (auto& x : buf) x = static_cast<T>(dist(rng));
                break;
            }
            case RandomRangeType::Small: {
                int maxval = std::max(1, int(std::sqrt(N)));
                uniform_real_distribution<T> dist(0, static_cast<T>(maxval));
                for (auto& x : buf) x = dist(rng);
                break;
            }
            case RandomRangeType::NRange: {
                uniform_real_distribution<T> dist(0, static_cast<T>(std::max(0, N-1)));
                for (auto& x : buf) x = dist(rng);
                break;
            }
        }
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
void BM_All(benchmark::State& state, SortType t/*forse rimuovo*/, SmallSortStrategyEnum ss, SortAlgorithm alg, RandomRangeType rangeType) {
    size_t N = state.range(0);
    vector<T> original(N);
    mt19937_64 rng(5432 + int(t)*100 + int(ss)*10 + int(alg) + int(rangeType)*1000); //addd time se possiblle (e sposto calcolo di la?)
    fill_random(original, rng, rangeType, int(N));
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

    SortType types[] = { SortType::Int32 };
    int sizes[]     = {1000, 1200, 1400, 1600, 1800, 2000};
    SmallSortStrategyEnum ssArr[] = { SmallSortStrategyEnum::AlphaDev };
    SortAlgorithm algs[] = { /*SortAlgorithm::TimSort,*/ SortAlgorithm::Intro };
    int maxSmalls[] = {1};
    RandomRangeType rangeTypes[] = {RandomRangeType::FullType, RandomRangeType::Binary, RandomRangeType::Small, RandomRangeType::NRange};

    for (auto t : types) {
        for (auto N : sizes) {
            for (auto ss : ssArr) {
                for (auto alg : algs) {
                    for (auto ms : maxSmalls) {
                        for (auto rangeType : rangeTypes) {
                            // Nome: type_size_ss_alg_maxSmall_range
                            string name = string(SortTypeNames[int(t)])
                                + "_" + SmallSortNames[int(ss)]
                                + "_" + SortAlgoNames[int(alg)]
                                + "_maxSmall" + to_string(ms)
                                + "_rng" + RandomRangeNames[int(rangeType)];
                            switch (t) {
                                case SortType::Int32:
                                    benchmark::RegisterBenchmark(name.c_str(), BM_All<int32_t>, t, ss, alg, rangeType)
                                        ->Args({N, ms})->Complexity();
                                    break;
                                case SortType::Int64:
                                    benchmark::RegisterBenchmark(name.c_str(), BM_All<int64_t>, t, ss, alg, rangeType)
                                        ->Args({N, ms})->Complexity();
                                    break;
                                case SortType::Float:
                                    benchmark::RegisterBenchmark(name.c_str(), BM_All<float>, t, ss, alg, rangeType)
                                        ->Args({N, ms})->Complexity();
                                    break;
                            }
                        }
                    }
                }
            }
        }
    }

    benchmark::RunSpecifiedBenchmarks();
    return 0;
}