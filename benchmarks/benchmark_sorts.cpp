#include <benchmark/benchmark.h>
#include <random>
#include <vector>
#include <limits>
#include <cstdint>
#include <numeric>
#include "sorts.h"
#include <iostream>
#include <unordered_set>

using namespace std;

// Tipi di dato
enum class SortType { Int32 = 0, Int64 = 1, Float = 2 };
static const char* SortTypeNames[] = {"int32","int64","float"};

// Strategie small sort
enum class SmallSortStrategyEnum { AlphaDev = 0, BranchlessSN = 1, Branching = 2, SelectionSort = 3, InsertionSort = 4, BubbleSort = 5 };
static const char* SmallSortNames[] = {"AlphaDev","BranchlessSN","Branching","SelectionSort","InsertionSort","BubbleSort"};

// Algoritmi di ordinamento
enum class SortAlgorithm { QuickSort = 0, MergeSort = 1, HeapSort = 2, SelectionSort = 3, InsertionSort = 4, BubbleSort = 5, IntroSort = 6, StdSort = 7, TimSort = 8, TimSortOld = 9, MergeSortOld = 10, QuickSortOld = 11 };
static const char* SortAlgoNames[] = {"QuickSort","MergeSort","HeapSort","SelectionSort","InsertionSort","BubbleSort","IntroSort", "StdSort", "TimSort", "TimSortOld", "MergeSortOld", "QuickSortOld"};

// Tipi di range per il riempimento random
enum class RandomRangeType { Full = 0, Binary = 1, Small = 2, NRange = 3 };
static const char* RandomRangeNames[] = {"Full", "Binary", "Small", "NRange"};

// Tipi di ordine per l'array
enum class ArrayOrderType { Random = 0, Ascending = 1, Descending = 2, PipeOrgan = 3, Constant = 4, MostlySorted = 5, NaturalRuns = 6 };
static const char* ArrayOrderNames[] = {"Random", "Ascending", "Descending", "PipeOrgan", "Constant", "MostlySorted", "NaturalRuns"};

// generico fill_random con range selezionabile
template<typename T>
void fill_random(vector<T>& buf, mt19937_64& rng/*e se lo generassi qui senza averlo come param?*/, RandomRangeType rangeType, int N) {
    if constexpr (is_integral_v<T>) { //gestisco caso integral vs not (che non sia float per forza)
        switch (rangeType) {
            case RandomRangeType::Full: {
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
            case RandomRangeType::Full: {
                uniform_real_distribution<T> dist(0, numeric_limits<T>::max());
                uniform_int_distribution<int> sign_dist(0, 1); // 0 or 1
                for (auto& x : buf) {
                    int sign = sign_dist(rng) == 0 ? -1 : 1;
                    x = dist(rng) * sign;
                }
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
            // Scegli N/10 inizi casuali (oltre a 0), ordina, e ogni segmento è una run
            size_t k = std::max<size_t>(1, std::sqrt(N));
            std::vector<size_t> starts = {0};
            if (k > 1 && N > k) {
                std::unordered_set<size_t> chosen;
                uniform_int_distribution<size_t> dist(1, N-1);
                while (starts.size() < k) {
                    size_t s = dist(rng);
                    if (chosen.insert(s).second) starts.push_back(s);
                }
            }
            std::sort(starts.begin(), starts.end());
            starts.push_back(N);
            int prev_last = 0;
            bool prev_valid = false;
            uniform_int_distribution<int> val_dist(-1000, 1000);
            uniform_int_distribution<int> dir_dist(0, 1); // 0: crescente, 1: decrescente
            for (size_t i = 0; i < k; ++i) {
                size_t seg_start = starts[i];
                size_t seg_end = starts[i+1];
                size_t seg_len = seg_end - seg_start;
                if (seg_len < 2) continue;
                bool crescente = dir_dist(rng) == 0;
                int first_val;
                if (!prev_valid) {
                    first_val = val_dist(rng);
                } else {
                    if (crescente) {
                        uniform_int_distribution<int> dist(prev_last+1, 1000);
                        first_val = dist(rng);
                    } else {
                        uniform_int_distribution<int> dist(-1000, prev_last-1);
                        first_val = dist(rng);
                    }
                }
                if (crescente) {
                    std::iota(buf.begin() + seg_start, buf.begin() + seg_end, T(first_val));
                    prev_last = first_val + int(seg_len) - 1;
                } else {
                    for (size_t j = 0; j < seg_len; ++j) {
                        buf[seg_start + j] = T(first_val - int(j));
                    }
                    prev_last = first_val - int(seg_len) + 1;
                }
                prev_valid = true;
            }
            break;
        }
    }
}

// Funzione di dispatch per ciascun algoritmo
template<typename T>
void runSort(T* data, int size, int maxSmall, SmallSortStrategyEnum ss, SortAlgorithm alg) {
    switch (alg) {
        case SortAlgorithm::QuickSort:
            quickSort(data, 0, size-1, maxSmall, static_cast<SmallSortStrategy>(ss));
            break;
        case SortAlgorithm::MergeSort:
            mergeSort(data, 0, size-1, maxSmall, static_cast<SmallSortStrategy>(ss));
            break;
        case SortAlgorithm::HeapSort:
            heapSort(data, size);
            break;
        case SortAlgorithm::SelectionSort:
            selectionSort(data, size);
        break;
        case SortAlgorithm::InsertionSort:
            insertionSort(data, size);
        break;
        case SortAlgorithm::BubbleSort:
            bubbleSort(data, size);
        break;
        case SortAlgorithm::IntroSort:
            introSort(data, 0, size-1, maxSmall, static_cast<SmallSortStrategy>(ss));
        break;
        case SortAlgorithm::StdSort:
            std::sort(data, data+size);
        break;
        case SortAlgorithm::TimSort:
            timSort(data, size, maxSmall, static_cast<SmallSortStrategy>(ss));
        break;
        case SortAlgorithm::TimSortOld:
            timSortOld(data, size, maxSmall, static_cast<SmallSortStrategy>(ss));
        break;
        case SortAlgorithm::MergeSortOld:
            mergeSortOld(data, 0, size-1, maxSmall, static_cast<SmallSortStrategy>(ss));
        break;
        case SortAlgorithm::QuickSortOld:
            quickSortOld(data, 0, size-1, maxSmall, static_cast<SmallSortStrategy>(ss));
        break;
    }
}

// Benchmark template
template<typename T>
void BM_All(benchmark::State& state, SortType t, SmallSortStrategyEnum ss, SortAlgorithm alg, RandomRangeType rangeType, ArrayOrderType orderType, int maxSmall, int rngSeed) {
    size_t N = state.range(0);
    vector<T> original(N);
    mt19937_64 rng(rngSeed); 
    fill_random(original, rng, rangeType, int(N));
    apply_order(original, orderType, rng);
    for (auto _ : state) {
        vector<T> buf = original;
        runSort<T>(buf.data(), int(buf.size()), maxSmall, ss, alg);
        benchmark::ClobberMemory();
    }
    state.SetComplexityN(N);
}

int main(int argc, char** argv) {
    
    //if constexpr (__cplusplus == 202002L) {
    /*    std::cout << "✔ C++20 attivo (__cplusplus = " << __cplusplus << ")\n";
    } else {
        std::cout << "✖ C++20 NON ATTIVO (__cplusplus = " << __cplusplus << ")\n";
    }*/


    benchmark::Initialize(&argc, argv);

    // Default values
    vector<SortType> types = { SortType::Int32, SortType::Int64, SortType::Float };
    vector<int> sizes = {10, 100, 1000, 10000, 100000};
    vector<SmallSortStrategyEnum> ssArr = { SmallSortStrategyEnum::AlphaDev };
    vector<SortAlgorithm> algs = { SortAlgorithm::StdSort };
    vector<int> maxSmalls = {64};
    vector<RandomRangeType> rangeTypes = { RandomRangeType::Full };
    vector<ArrayOrderType> orderTypes = { ArrayOrderType::Random };

    // Parse command line arguments (simple CSV for each parameter, order: types sizes ssArr algs maxSmalls rangeTypes orderTypes)
    // Example: ./benchmark_app int32,int64 100,1000 AlphaDev StdSort,Intro 64 Full Random
    if (argc > 1) {
        auto parse_csv = [](const char* arg) {
            vector<string> out;
            string s(arg);
            size_t pos = 0;
            while ((pos = s.find(',')) != string::npos) {
                out.push_back(s.substr(0, pos));
                s.erase(0, pos + 1);
            }
            out.push_back(s);
            return out;
        };
        int idx = 1;
        if (argc > idx) { // types
            types.clear();
            for (auto& s : parse_csv(argv[idx++])) {
                for (int i = 0; i < 3; ++i)
                    if (s == SortTypeNames[i]) types.push_back(static_cast<SortType>(i));
            }
        }
        if (argc > idx) { // sizes
            sizes.clear();
            for (auto& s : parse_csv(argv[idx++])) sizes.push_back(stoi(s));
        }
        if (argc > idx) { // ssArr
            ssArr.clear();
            for (auto& s : parse_csv(argv[idx++])) {
                for (int i = 0; i < 6; ++i)
                    if (s == SmallSortNames[i]) ssArr.push_back(static_cast<SmallSortStrategyEnum>(i));
            }
        }
        if (argc > idx) { // algs
            algs.clear();
            for (auto& s : parse_csv(argv[idx++])) {
                for (int i = 0; i < 12; ++i)
                    if (s == SortAlgoNames[i]) algs.push_back(static_cast<SortAlgorithm>(i));
            }
        }
        if (argc > idx) { // maxSmalls
            maxSmalls.clear();
            for (auto& s : parse_csv(argv[idx++])) maxSmalls.push_back(stoi(s));
        }
        if (argc > idx) { // rangeTypes
            rangeTypes.clear();
            for (auto& s : parse_csv(argv[idx++])) {
                for (int i = 0; i < 4; ++i)
                    if (s == RandomRangeNames[i]) rangeTypes.push_back(static_cast<RandomRangeType>(i));
            }
        }
        if (argc > idx) { // orderTypes
            orderTypes.clear();
            for (auto& s : parse_csv(argv[idx++])) {
                for (int i = 0; i < 7; ++i)
                    if (s == ArrayOrderNames[i]) orderTypes.push_back(static_cast<ArrayOrderType>(i));
            }
        }
    }

    int rngSeed = std::chrono::steady_clock::now().time_since_epoch().count();

    for (auto t : types) {
        for (auto N : sizes) {
            for (auto ss : ssArr) {
                for (auto alg : algs) {
                    for (auto ms : maxSmalls) {
                        for (auto rangeType : rangeTypes) {
                            for (auto orderType : orderTypes) {
                                // Nome: type_size_ss_alg_maxSmall_range_order
                                string name = "type:" + string(SortTypeNames[int(t)])
                                    + ",smallSort:" + SmallSortNames[int(ss)]
                                    + ",algo:" + SortAlgoNames[int(alg)]
                                    + ",maxSmall:" + to_string(ms)
                                    + ",range:" + RandomRangeNames[int(rangeType)]
                                    + ",order:" + ArrayOrderNames[int(orderType)];
                                switch (t) {
                                    case SortType::Int32:
                                        benchmark::RegisterBenchmark(name.c_str(), BM_All<int32_t>, t, ss, alg, rangeType, orderType, ms, rngSeed)
                                            ->Args({N})->Complexity();
                                        break;
                                    case SortType::Int64:
                                        benchmark::RegisterBenchmark(name.c_str(), BM_All<int64_t>, t, ss, alg, rangeType, orderType, ms, rngSeed)
                                            ->Args({N})->Complexity();
                                        break;
                                    case SortType::Float:
                                        benchmark::RegisterBenchmark(name.c_str(), BM_All<float>, t, ss, alg, rangeType, orderType, ms, rngSeed)
                                            ->Args({N})->Complexity();
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