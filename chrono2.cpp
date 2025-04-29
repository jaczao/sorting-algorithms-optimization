#include <benchmark/benchmark.h>
#include <algorithm>
#include <random>
#include <vector>
using namespace std;

void Sort3AlphaDev(int* buffer) { // va ma ordina in senso inverso
    asm volatile(
        /* mov 0x4(%0), %%eax */
        "ldr  w1, [%0, #4]        \n"
        /* mov 0x8(%0), %%ecx */
        "ldr  w2, [%0, #8]        \n"
        /* cmp %%eax, %%ecx */
        "cmp  w1, w2              \n"
        /* mov %%eax, %%edx */
        "mov  w3, w1              \n"
        /* cmovl %%ecx, %%edx  (= if w1 > w2 then w3 = w2) */
        "csel w3, w2, w3, gt      \n"

        /* mov (%0), %%r8d */
        "ldr  w4, [%0]            \n"
        /* cmovg %%ecx, %%eax  (= if w2 > w1 then w1 = w2) */
        "csel w1, w2, w1, gt      \n"

        /* cmp %%r8d, %%eax */
        "cmp  w4, w1              \n"
        /* mov %%r8d, %%ecx */
        "mov  w2, w4              \n"
        /* cmovl %%eax, %%ecx  (= if w1 < w2_old then w2 = w1) */
        "csel w2, w1, w2, lt      \n"
        /* cmovle %%r8d, %%eax (= if w4 <= w1 then w1 = w4) */
        "csel w1, w4, w1, le      \n"

        /* mov %%eax, 0x8(%0) */
        "str  w1, [%0, #8]        \n"

        /* cmp %%ecx, %%edx */
        "cmp  w2, w3              \n"
        /* cmovle %%edx, %%r8d (= if w3 <= w2 then w4 = w3) */
        "csel w4, w3, w4, le      \n"
        /* mov %%r8d, (%0) */
        "str  w4, [%0]            \n"
        /* cmovg %%edx, %%ecx (= if w3 > w2 then w2 = w3) */
        "csel w2, w3, w2, gt      \n"
        /* mov %%ecx, 0x4(%0) */
        "str  w2, [%0, #4]        \n"

        : "+r"(buffer)         // buffer in x0
        :
        : "w1","w2","w3","w4", // clobber temporanei
          "memory"
    );
}

template <typename _Compare, typename _RandomAccessIterator>
inline void cond_swap(_RandomAccessIterator __x, _RandomAccessIterator __y, _Compare __c) {
  using value_type = typename std::iterator_traits<_RandomAccessIterator>::value_type;
  bool __r = __c(*__x, *__y);
  value_type __tmp = __r ? *__x : *__y;
  *__y = __r ? *__y : *__x;
  *__x = __tmp;
}

template <class _Compare, class _RandomAccessIterator>
inline void partially_sorted_swap(_RandomAccessIterator __x, _RandomAccessIterator __y,
                                                           _RandomAccessIterator __z, _Compare __c) {
   using value_type = typename std::iterator_traits<_RandomAccessIterator>::value_type;
   bool __r = __c(*__z, *__x);
   value_type __tmp = __r ? *__z : *__x;
   *__z = __r ? *__x : *__z;
   __r = __c(__tmp, *__y);
   *__x = __r ? *__x : *__y;
   *__y = __r ? *__y : __tmp;
}

template <class _Compare, class _ForwardIterator>
  unsigned sort3(_ForwardIterator __x, _ForwardIterator __y, _ForwardIterator __z, _Compare __c)
 {
     unsigned __r = 0;
     if (!__c(*__y, *__x))          // if x <= y
     {
         if (!__c(*__z, *__y))      // if y <= z
             return __r;            // x <= y && y <= z
                                    // x <= y && y > z
         swap(*__y, *__z);          // x <= z && y < z
         __r = 1;
         if (__c(*__y, *__x))       // if x > y
         {
             swap(*__x, *__y);      // x < y && y <= z
             __r = 2;
         }
         return __r;                // x <= y && y < z
     }
     if (__c(*__z, *__y))           // x > y, if y > z
     {
         swap(*__x, *__z);          // x < y && y < z
         __r = 1;
         return __r;
     }
     swap(*__x, *__y);              // x > y && y <= z
     __r = 1;                       // x < y && x <= z
     if (__c(*__z, *__y))           // if y > z
     {
         swap(*__y, *__z);          // x <= y && y < z
         __r = 2;
     }
     return __r;
 }                                  // x <= y && y <= z

// una semplice insertionSort generica
void insertionSort(int* a, size_t n) {
    for (size_t i = 1; i < n; ++i) {
        int key = a[i];
        size_t j = i;
        while (j > 0 && a[j-1] > key) {
            a[j] = a[j-1];
            --j;
        }
        a[j] = key;
    }
}

// --- helper per riempire ---
static void fill_random(std::vector<int>& buf, std::mt19937_64& rng) {
    static std::uniform_int_distribution<int> dist(0, 1'000'000);
    for (auto& x : buf) x = dist(rng);
}

// --- Benchmark std::sort ---
static void BM_StdSort(benchmark::State& state) {
    size_t N = state.range(0);
    std::vector<int> buf(N);
    std::mt19937_64 rng(123456);

    for (auto _ : state) {
        fill_random(buf, rng);
        std::sort(buf.begin(), buf.end());
    }
    // opzionale: report in nanosecondi per elemento
    state.counters["ns_per_run"] = benchmark::Counter(
        double(state.iterations() ? state.iterations() : 1),
        benchmark::Counter::kIsIterationInvariantRate);
}
BENCHMARK(BM_StdSort)->Args({3})->Args({16})->Args({256})->Args({16384})->Args({262144});

// --- Benchmark insertionSort (solo N<=256) ---
static void BM_InsertionSort(benchmark::State& state) {
    size_t N = state.range(0);
    std::vector<int> buf(N);
    std::mt19937_64 rng(123456);
    for (auto _ : state) {
        fill_random(buf, rng);
        insertionSort(buf.data(), N);
    }
}
BENCHMARK(BM_InsertionSort)->Args({3})
                            ->Args({16})
                            ->Args({256});

static void BM_Sort3InlineAsm(benchmark::State& state) {
    int buf[3];
    std::mt19937_64 rng(123456);
    std::uniform_int_distribution<int> dist(0, 1000000);
    for (auto _ : state) {
        // riempi “buf” direttamente, non reinterpret_cast
        for (int i = 0; i < 3; ++i)
            buf[i] = dist(rng);
        Sort3AlphaDev(buf);
    }
}
BENCHMARK(BM_Sort3InlineAsm)->Arg(3);

static void BM_Sort3PREDeepmind(benchmark::State& state) {
    int buf[3];
    std::mt19937_64 rng(123456);
    std::uniform_int_distribution<int> dist(0, 1000000);
    for (auto _ : state) {
        for (int i = 0; i < 3; ++i)
            buf[i] = dist(rng);
        sort3(buf, buf+1, buf+2, std::less<int>());
    }
}
BENCHMARK(BM_Sort3PREDeepmind)->Arg(3);

static void BM_Sort3POSTDeepmind(benchmark::State& state) {
    int buf[3];
    std::mt19937_64 rng(123456);
    std::uniform_int_distribution<int> dist(0, 1000000);
    for (auto _ : state) {
        for (int i = 0; i < 3; ++i)
            buf[i] = dist(rng);
        cond_swap(buf+1, buf+2, std::less<int>());
        partially_sorted_swap(buf, buf+1, buf+2, std::less<int>());
    }
}
BENCHMARK(BM_Sort3POSTDeepmind)->Arg(3);

// genera il main che esegue tutti i benchmark
BENCHMARK_MAIN();
