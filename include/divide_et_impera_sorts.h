#pragma once

#include <cstddef>
#include "branchless_fixed_sorts.h"
#include "branching_fixed_sorts.h"
// Selezione del header AlphaDev in base all'architettura
#if defined(__x86_64__) || defined(_M_X64)
#include "../include/alpha_dev_sorts_x86-64.h"
#elif defined(__aarch64__)
#include "../include/alpha_dev_sorts_arm64.h"
#else
#error "Unsupported architecture for AlphaDev sorts"
#endif
#include <cstdint>
#include <limits>
#include <type_traits>
#include <algorithm>
#include <vector>


enum class SmallSortStrategy { AlphaDev, Branchless, Branching };

// --- helper: manual swap ---
template<typename T>
inline void sort2(T* a, T* b) {
    if (*a > *b) std::swap(*a, *b);
}

// for contiguous memory and max 64bit types
template<typename T>
constexpr bool is_max64bit_contiguous = 
    sizeof(T) <= 8 && std::is_trivially_copyable_v<T>;

// Dispatch small sorts fino a maxSmall; custom only per int32, fallback std::sort
template<typename T>
void smallSort(T* buffer, int size, int maxSmall, SmallSortStrategy strat) {
    if (size < 2) return;
    if constexpr (std::is_same_v<T, int32_t>) {
        if (size == 2 && maxSmall >= 2) { sort2(buffer, buffer+1); return; }
        if (size >= 3 && size <= maxSmall) {
            switch (strat) {
                case SmallSortStrategy::AlphaDev:
                    switch (size) {
                        case 3: Sort3AlphaDev(buffer); return;
                        case 4: Sort4AlphaDev(buffer); return;
                        case 5: Sort5AlphaDev(buffer); return;
                        case 6: Sort6AlphaDev(buffer); return;
                        case 7: Sort7AlphaDev(buffer); return;
                        case 8: Sort8AlphaDev(buffer); return;
                    }
                    break;
                case SmallSortStrategy::Branchless:
                    switch (size) {
                        case 3: sort3branchless(buffer); return;
                        case 4: sort4branchless(buffer); return;
                        case 5: sort5branchless(buffer); return;
                        case 6: sort6branchless(buffer); return;
                        case 7: sort7branchless(buffer); return;
                        case 8: sort8branchless(buffer); return;
                    }
                    break;
                case SmallSortStrategy::Branching:
                    switch (size) {
                        case 3: sort3branching(buffer); return;
                        case 4: sort4branching(buffer); return;
                        case 5: sort5branching(buffer); return;
                        case 6: sort6branching(buffer); return;
                        case 7: sort7branching(buffer); return;
                        case 8: sort8branching(buffer); return;
                    }
                    break;
            }
        }
    }
    // fallback per tutti i tipi
    std::sort(buffer, buffer + size);
}

// Merge Sort template
template<typename T>
void mergeSort(T* arr, int left, int right, int maxSmall, SmallSortStrategy strat) {
    int len = right - left + 1;
    if (len <= 1) return;
    if (len <= maxSmall) { smallSort(arr + left, len, maxSmall, strat); return; }
    int mid = left + (right - left) / 2;
    mergeSort(arr, left, mid, maxSmall, strat);
    mergeSort(arr, mid + 1, right, maxSmall, strat);
    int n1 = mid - left + 1;
    int n2 = right - mid;
    std::vector<T> L(n1), R(n2);
    std::copy(arr + left,     arr + mid + 1, L.begin());
    std::copy(arr + mid + 1, arr + right + 1, R.begin());
    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) arr[k++] = (L[i] <= R[j]) ? L[i++] : R[j++];
    while (i < n1)            arr[k++] = L[i++];
    while (j < n2)            arr[k++] = R[j++];
}

// Quick Sort template
template<typename T>
int partition(T* arr, int left, int right) {
    T pivot = arr[left];
    int i = left + 1, j = right;
    while (true) {
        while (i <= j && arr[i] <= pivot) ++i;
        while (i <= j && arr[j] >  pivot) --j;
        if (i > j) break;
        std::swap(arr[i], arr[j]);
    }
    std::swap(arr[left], arr[j]);
    return j;
}

template<typename T>
void quickSort(T* arr, int left, int right, int maxSmall, SmallSortStrategy strat) {
    int len = right - left + 1;
    if (len <= 1) return;
    if (len <= maxSmall) { smallSort(arr + left, len, maxSmall, strat); return; }
    int pi = partition(arr, left, right);
    quickSort(arr, left,     pi - 1, maxSmall, strat);
    quickSort(arr, pi + 1, right,     maxSmall, strat);
}