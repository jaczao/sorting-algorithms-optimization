#include <benchmark/benchmark.h>
#include <algorithm>
#include <random>
#include <vector>
#include "divide_et_impera_sorts.h"

using namespace std;
enum class SortType { Int32=0, Int64=1, Float=2 };
static const char* SortTypeNames[] = {"int32","int64","float"};

// fill_random generico
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

// Benchmark QuickSort template
template<typename T>
void BM_Quick(benchmark::State& st, int ms, SmallSortStrategy strat, SortType t) {
    size_t N = st.range(0);
    vector<T> orig(N);
    mt19937_64 rng(1000 + int(t)); fill_random(orig, rng);
    for (auto _ : st) {
        vector<T> buf = orig;
        quickSort(buf.data(), 0, int(buf.size()) - 1, ms, strat);
        benchmark::ClobberMemory();
    }
    st.SetComplexityN(N);
}

// Benchmark MergeSort template
template<typename T>
void BM_Merge(benchmark::State& st, int ms, SmallSortStrategy strat, SortType t) {
    size_t N = st.range(0);
    vector<T> orig(N);
    mt19937_64 rng(2000 + int(t)); fill_random(orig, rng);
    for (auto _ : st) {
        vector<T> buf = orig;
        mergeSort(buf.data(), 0, int(buf.size()) - 1, ms, strat);
        benchmark::ClobberMemory();
    }
    st.SetComplexityN(N);
}

int main(int argc, char** argv) {
    benchmark::Initialize(&argc, argv);
    int sizes[] = {64, 1024, 262144};
    int maxS  [] = {1,2,3,4,5,6,7,8};
    SmallSortStrategy strats[] = {SmallSortStrategy::AlphaDev /*, Branchless, Branching*/};
    SortType types[] = {SortType::Int32, SortType::Int64, SortType::Float};

    for (auto t : types) for (auto N : sizes) for (auto ms : maxS) for (int s = 0; s < 1; ++s) {
        auto strat = strats[s];
        string nameQ = string("QS_") + SortTypeNames[int(t)] + "_N" + to_string(N) + "_ms" + to_string(ms);
        switch (t) {
            case SortType::Int32: benchmark::RegisterBenchmark(nameQ.c_str(), BM_Quick<int32_t>, ms, strat, t)->Arg(N)->Complexity(); break;
            case SortType::Int64: benchmark::RegisterBenchmark(nameQ.c_str(), BM_Quick<int64_t>, ms, strat, t)->Arg(N)->Complexity(); break;
            case SortType::Float: benchmark::RegisterBenchmark(nameQ.c_str(), BM_Quick<float>,    ms, strat, t)->Arg(N)->Complexity(); break;
        }
        string nameM = string("MS_") + SortTypeNames[int(t)] + "_N" + to_string(N) + "_ms" + to_string(ms);
        switch (t) {
            case SortType::Int32: benchmark::RegisterBenchmark(nameM.c_str(), BM_Merge<int32_t>, ms, strat, t)->Arg(N)->Complexity(); break;
            case SortType::Int64: benchmark::RegisterBenchmark(nameM.c_str(), BM_Merge<int64_t>, ms, strat, t)->Arg(N)->Complexity(); break;
            case SortType::Float: benchmark::RegisterBenchmark(nameM.c_str(), BM_Merge<float>,    ms, strat, t)->Arg(N)->Complexity(); break;
        }
    }

    benchmark::RunSpecifiedBenchmarks();
    return 0;
}
