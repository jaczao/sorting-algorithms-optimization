#include <benchmark/benchmark.h>
#include <random>
#include <vector>
#include <limits>
#include <cstdint>
#include <numeric>
#include "sorts.h"
#include <iostream>

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

// Tipi di ordine per l'array
enum class ArrayOrderType { Random = 0, Ascending = 1, Descending = 2, PipeOrgan = 3, Constant = 4, MostlySorted = 5, NaturalRuns = 6 };
static const char* ArrayOrderNames[] = {"Random", "Ascending", "Descending", "PipeOrgan", "Constant", "MostlySorted", "NaturalRuns"};

// generico fill_random con range selezionabile
template<typename T>
void fill_random(vector<T>& buf, mt19937_64& rng/*e se lo generassi qui senza averlo come param?*/, RandomRangeType rangeType, int N) {
    if constexpr (is_integral_v<T>) { //gestisco caso integral vs not (che non sia float per forza)
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

// Applica l'ordine desiderato all'array
template<typename T>
void apply_order(vector<T>& buf, ArrayOrderType order, mt19937_64& rng) {
    size_t N = buf.size();
    switch (order) {
        case ArrayOrderType::Random:
            // già random, non fare nulla
            break;
        case ArrayOrderType::Ascending:
            std::iota(buf.begin(), buf.end(), T(0));
            break;
        case ArrayOrderType::Descending:
            std::iota(buf.begin(), buf.end(), T(0));
            std::reverse(buf.begin(), buf.end());
            break;
        case ArrayOrderType::PipeOrgan: {
            // valori crescenti fino a metà, poi decrescenti
            vector<T> tmp(N);
            size_t mid = N / 2;
            for (size_t i = 0; i < mid; ++i) tmp[i] = T(i);
            for (size_t i = mid; i < N; ++i) tmp[i] = T(N - 1 - (i - mid));
            buf = std::move(tmp);
            break;
        }
        case ArrayOrderType::Constant: {
            T val = N > 0 ? buf[0] : T(0); //serve?
            std::fill(buf.begin(), buf.end(), val);
            break;
        }
        case ArrayOrderType::MostlySorted: {
            std::iota(buf.begin(), buf.end(), T(0));
            int K = std::max(1, int(std::log2(N)));
            uniform_int_distribution<size_t> dist(0, N-1);
            for (int i = 0; i < K; ++i) {
                size_t a = dist(rng), b = dist(rng); //ok o mi voglio assicurare che sia k coppie diverse?
                std::swap(buf[a], buf[b]);
            }
            break;
        }
        case ArrayOrderType::NaturalRuns: {
            // scegli k casuale tra 2 e N/5
            size_t kmin = 2, kmax = std::max<size_t>(2, N/5);
            uniform_int_distribution<size_t> k_dist(kmin, kmax);
            size_t k = k_dist(rng);
            uniform_int_distribution<size_t> pos_dist(0, N-1);
            uniform_int_distribution<size_t> len_dist(2, std::max<size_t>(2, N/5));
            for (size_t i = 0; i < k; ++i) {
                size_t start = pos_dist(rng);
                size_t maxlen = std::min(len_dist(rng), N - start);
                if (maxlen < 2) continue;
                std::iota(buf.begin() + start, buf.begin() + start + maxlen, T(start)); //attenzione che tstart non sfori maxTYPE
            }
            break;
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
void BM_All(benchmark::State& state, SortType t, SmallSortStrategyEnum ss, SortAlgorithm alg, RandomRangeType rangeType, ArrayOrderType orderType) {
    size_t N = state.range(0);
    vector<T> original(N);
    mt19937_64 rng(5432 + int(t)*100 + int(ss)*10 + int(alg) + int(rangeType)*1000 + int(orderType)*10000);
    fill_random(original, rng, rangeType, int(N));
    apply_order(original, orderType, rng);
    int maxSmall = state.range(1);
    for (auto _ : state) {
        vector<T> buf = original;
        runSort<T>(buf.data(), int(buf.size()), maxSmall, ss, alg);
        benchmark::ClobberMemory();
    }
    state.SetComplexityN(N);
}

int main(int argc, char** argv) {
    
    /*
    if constexpr (__cplusplus == 202002L) {
        std::cout << "✔ C++20 attivo (__cplusplus = " << __cplusplus << ")\n";
    } else {
        std::cout << "✖ C++20 NON ATTIVO (__cplusplus = " << __cplusplus << ")\n";
    }*/


    benchmark::Initialize(&argc, argv);

    SortType types[] = { SortType::Int32 };
    int sizes[]     = {100, 200, 500, 1000, 2000, 5000, 10000, 20000, 50000, 100000, 200000, 500000, 1000000}; // massimo: 8 * 7 = 56 combinazioni
    SmallSortStrategyEnum ssArr[] = { SmallSortStrategyEnum::AlphaDev };//, SmallSortStrategyEnum::Insertion };
    SortAlgorithm algs[] = { SortAlgorithm::StdSort, SortAlgorithm::Intro, SortAlgorithm::TimSort};//, SortAlgorithm::Merge, SortAlgorithm::Quick, SortAlgorithm::Heap  };//, SortAlgorithm::TimSort };
    int maxSmalls[] = {64}; // massimo: 10
    RandomRangeType rangeTypes[] = {RandomRangeType::FullType};//,RandomRangeType::Binary};//, RandomRangeType::Small, RandomRangeType::NRange};
    ArrayOrderType orderTypes[] = {ArrayOrderType::Random};//, ArrayOrderType::Ascending, ArrayOrderType::Descending, ArrayOrderType::PipeOrgan, ArrayOrderType::Constant, ArrayOrderType::MostlySorted, ArrayOrderType::NaturalRuns};

    for (auto t : types) { // massimo: 3 * 12 * 2 * 5 * 8 * 4 * 7 = 80640 combinazioni
        for (auto N : sizes) { // minimo: 3 * 6 * 1 * 5 * 2 * 4 * 7 = 5040 combinazioni
            for (auto ss : ssArr) {
                for (auto alg : algs) {
                    for (auto ms : maxSmalls) {
                        for (auto rangeType : rangeTypes) {
                            for (auto orderType : orderTypes) {
                                // Nome: type_size_ss_alg_maxSmall_range_order
                                string name = string(SortTypeNames[int(t)])
                                    + "_" + SmallSortNames[int(ss)]
                                    + "_" + SortAlgoNames[int(alg)]
                                    + "_maxSmall" + to_string(ms)
                                    + "_rng" + RandomRangeNames[int(rangeType)]
                                    + "_ord" + ArrayOrderNames[int(orderType)];
                                switch (t) {
                                    case SortType::Int32:
                                        benchmark::RegisterBenchmark(name.c_str(), BM_All<int32_t>, t, ss, alg, rangeType, orderType)
                                            ->Args({N, ms})->Complexity();//->Iterations(20);
                                        break;
                                    case SortType::Int64:
                                        benchmark::RegisterBenchmark(name.c_str(), BM_All<int64_t>, t, ss, alg, rangeType, orderType)
                                            ->Args({N, ms})->Complexity();
                                        break;
                                    case SortType::Float:
                                        benchmark::RegisterBenchmark(name.c_str(), BM_All<float>, t, ss, alg, rangeType, orderType)
                                            ->Args({N, ms})->Complexity();
                                        break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    benchmark::SetDefaultTimeUnit(benchmark::kNanosecond);
    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();
    return 0;
}