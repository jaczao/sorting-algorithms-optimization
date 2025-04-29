#include <benchmark/benchmark.h>
#include <algorithm>
#include <random>
#include <vector>
using namespace std;


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


// --- helper per riempire ---
static void fill_random(std::vector<int>& buf, std::mt19937_64& rng) {
    static std::uniform_int_distribution<int> dist(0, 1'000'000);
    for (auto& x : buf) x = dist(rng);
}

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
