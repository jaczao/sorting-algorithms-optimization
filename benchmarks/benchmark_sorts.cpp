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
enum class SmallSortStrategyEnum { AlphaDev = 0, BranchlessSN = 1, Branching = 2, SelectionSort = 3, InsertionSort = 4, BubbleSort = 5 };
static const char* SmallSortNames[] = {"AlphaDev","BranchlessSN","Branching","SelectionSort","InsertionSort","BubbleSort"};

// Algoritmi di ordinamento
enum class SortAlgorithm { QuickSort = 0, MergeSort = 1, HeapSort = 2, Merge8 = 3, SelectionSort = 4, InsertionSort = 5, BubbleSort = 6, IntroSort =7, StdSort=8, TimSort=9, TimSortOld=10 };
static const char* SortAlgoNames[] = {"QuickSort","MergeSort","HeapSort","Merge8","SelectionSort","InsertionSort","BubbleSort","IntroSort", "StdSort", "TimSort", "TimSortOld"};

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
        case SortAlgorithm::QuickSort:
            quickSort(data, 0, size-1, maxSmall, static_cast<SmallSortStrategy>(ss));
            break;
        case SortAlgorithm::MergeSort:
            mergeSort(data, 0, size-1, maxSmall, static_cast<SmallSortStrategy>(ss));
            break;
        case SortAlgorithm::HeapSort:
            heapSort(data, size);
            break;
        case SortAlgorithm::Merge8:
            mergeSort8(data, 0, size-1, maxSmall, static_cast<SmallSortStrategy>(ss));
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
    vector<SortAlgorithm> algs = { SortAlgorithm::StdSort, SortAlgorithm::IntroSort, SortAlgorithm::TimSort, SortAlgorithm::MergeSort, SortAlgorithm::QuickSort };
    vector<int> maxSmalls = {64};
    vector<RandomRangeType> rangeTypes = { RandomRangeType::FullType };
    vector<ArrayOrderType> orderTypes = { ArrayOrderType::Random };

    // Parse command line arguments (simple CSV for each parameter, order: types sizes ssArr algs maxSmalls rangeTypes orderTypes)
    // Example: ./benchmark_app int32,int64 100,1000 AlphaDev StdSort,Intro 64 FullType Random
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
                for (int i = 0; i < 11; ++i)
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

    for (auto t : types) {
        for (auto N : sizes) {
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
                                            ->Args({N, ms})->Complexity();
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