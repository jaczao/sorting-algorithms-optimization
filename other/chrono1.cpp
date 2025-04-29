#include <iostream>
#include <algorithm>
#include <utility>
#include <random>
#include <chrono>
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

int main() {
    const int runs = 1000;
    std::mt19937_64 rng(123456);
    std::uniform_int_distribution<int> dist(0, 1);

    // le dimensioni da testare
    const std::vector<size_t> sizes = { 3, 16, 256 };

    std::cout << "Benchmarking sorting per array di varie dimensioni (" << runs << " run ciascuno)\n\n";

    for (size_t N : sizes) {
        std::vector<int> buffer(N);

        std::cout << "== Dimensione N = " << N << " ==\n";

        // std::sort
        {
            uint64_t sum_ns = 0;
            for (int i = 0; i < runs; ++i) {
                for (size_t j = 0; j < N; ++j)
                    buffer[j] = dist(rng);

                auto t0 = std::chrono::high_resolution_clock::now();
                std::sort(buffer.begin(), buffer.end());
                auto t1 = std::chrono::high_resolution_clock::now();

                sum_ns += std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
            }
            std::cout << "std::sort       average: " << (sum_ns / runs) << " ns\n";
        }

        // Sort3InlineAsm (solo per N==3)
        if (N == 3) {
            uint64_t sum_ns = 0;
            for (int i = 0; i < runs; ++i) {
                for (size_t j = 0; j < 3; ++j)
                    buffer[j] = dist(rng);

                auto t0 = std::chrono::high_resolution_clock::now();
                Sort3AlphaDev(buffer.data());
                auto t1 = std::chrono::high_resolution_clock::now();

                sum_ns += std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
            }
            std::cout << "Sort3InlineAsm   average: " << (sum_ns / runs) << " ns\n";
        }

        // Sort3POSTDeepmind (solo per N==3)
        if (N == 3) {
            uint64_t sum_ns = 0;
            for (int i = 0; i < runs; ++i) {
                for (size_t j = 0; j < 3; ++j)
                    buffer[j] = dist(rng);

                auto t0 = std::chrono::high_resolution_clock::now();
                cond_swap(buffer.data() + 1, buffer.data() + 2, std::less<int>());
                partially_sorted_swap(buffer.data(), buffer.data() + 1, buffer.data() + 2, std::less<int>());
                auto t1 = std::chrono::high_resolution_clock::now();

                sum_ns += std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
            }
            std::cout << "Sort3POSTDeepmind   average: " << (sum_ns / runs) << " ns\n";
        }

        // Sort3PREDeepmind (solo per N==3)
        if (N == 3) {
            uint64_t sum_ns = 0;
            for (int i = 0; i < runs; ++i) {
                for (size_t j = 0; j < 3; ++j)
                    buffer[j] = dist(rng);

                auto t0 = std::chrono::high_resolution_clock::now();
                sort3(buffer.data(), buffer.data() + 1, buffer.data() + 2, std::less<int>());
                auto t1 = std::chrono::high_resolution_clock::now();

                sum_ns += std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
            }
            std::cout << "Sort3PREDeepmind   average: " << (sum_ns / runs) << " ns\n";
        }

        // insertionSort
        if (N <= 256) {
            {
                uint64_t sum_ns = 0;
                for (int i = 0; i < runs; ++i) {
                    for (size_t j = 0; j < N; ++j)
                        buffer[j] = dist(rng);

                    auto t0 = std::chrono::high_resolution_clock::now();
                    insertionSort(buffer.data(), N);
                    auto t1 = std::chrono::high_resolution_clock::now();

                    sum_ns += std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
                }
                std::cout << "insertionSort   average: " << (sum_ns / runs) << " ns\n";
            }
        }

        std::cout << "\n";
    }

    return 0;
}
