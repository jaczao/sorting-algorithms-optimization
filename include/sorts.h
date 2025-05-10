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
#include <cmath>

// for contiguous memory and max 64bit types
template<typename T>
constexpr bool is_max64bit_contiguous = 
    sizeof(T) <= 8 && std::is_trivially_copyable_v<T>;

// --- helper: manual swap ---
template<typename T>
inline void sort2(T* a, T* b) {
    if (*a > *b) std::swap(*a, *b);
}

enum class SmallSortStrategy { AlphaDev, Branchless, Branching, Selection, Insertion, Bubble, Heap };

// Selection Sort
template<typename T>
void selectionSort(T* arr, int size) {
    for (int i = 0; i + 1 < size; ++i) {
        int minIdx = i;
        for (int j = i + 1; j < size; ++j)
            if (arr[j] < arr[minIdx]) minIdx = j;
        std::swap(arr[i], arr[minIdx]);
    }
}

// Insertion Sort
template<typename T>
void insertionSort(T* arr, int size) {
    for (int i = 1; i < size; ++i) {
        T key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) { arr[j + 1] = arr[j]; --j; }
        arr[j + 1] = key;
    }
}

// Bubble Sort
template<typename T>
void bubbleSort(T* arr, int size) {
    for (int i = 0; i < size - 1; ++i)
        for (int j = 0; j < size - 1 - i; ++j)
            if (arr[j] > arr[j + 1]) std::swap(arr[j], arr[j + 1]);
}

// Heap Sort
template<typename T>
void heapSort(T* arr, int size) {
    std::make_heap(arr, arr + size);
    std::sort_heap(arr, arr + size);
}    

// Dispatch small sorts fino a maxSmall; custom only per int32, fallback std::sort
template<typename T>
void smallSort(T* buffer, int size, int maxSmall, SmallSortStrategy strat) {
    if (size < 2) return;
    if (size == 2 && maxSmall >= 2) { sort2(buffer, buffer+1); return; }
    if (size >= 3 && size <= maxSmall) {
        if constexpr (std::is_same_v<T, int32_t>) {
            switch (strat) {
                case SmallSortStrategy::AlphaDev:
                    switch (size) {
                        case 3: Sort3AlphaDev(buffer); return;
                        case 4: Sort4AlphaDev(buffer); return;
                        case 5: Sort5AlphaDev(buffer); return;
                        case 6: Sort6AlphaDev(buffer); return;
                        case 7: Sort7AlphaDev(buffer); return;
                        case 8: Sort8AlphaDev(buffer); return;
                        case 9:  sort9branchless<void>(buffer, buffer+1, buffer+2, buffer+3, buffer+4, buffer+5, buffer+6, buffer+7, buffer+8, std::less<int32_t>{}); return;
                        case 10: sort10branchless<void>(buffer, buffer+1, buffer+2, buffer+3, buffer+4, buffer+5, buffer+6, buffer+7, buffer+8, buffer+9, std::less<int32_t>{}); return;
                        case 11: sort11branchless<void>(buffer, buffer+1, buffer+2, buffer+3, buffer+4, buffer+5, buffer+6, buffer+7, buffer+8, buffer+9, buffer+10, std::less<int32_t>{}); return;
                        case 12: sort12branchless<void>(buffer, buffer+1, buffer+2, buffer+3, buffer+4, buffer+5, buffer+6, buffer+7, buffer+8, buffer+9, buffer+10, buffer+11, std::less<int32_t>{}); return;
                        case 13: sort13branchless<void>(buffer, buffer+1, buffer+2, buffer+3, buffer+4, buffer+5, buffer+6, buffer+7, buffer+8, buffer+9, buffer+10, buffer+11, buffer+12, std::less<int32_t>{}); return;
                        case 14: sort14branchless<void>(buffer, buffer+1, buffer+2, buffer+3, buffer+4, buffer+5, buffer+6, buffer+7, buffer+8, buffer+9, buffer+10, buffer+11, buffer+12, buffer+13, std::less<int32_t>{}); return;
                        case 15: sort15branchless<void>(buffer, buffer+1, buffer+2, buffer+3, buffer+4, buffer+5, buffer+6, buffer+7, buffer+8, buffer+9, buffer+10, buffer+11, buffer+12, buffer+13, buffer+14, std::less<int32_t>{}); return;
                        case 16: sort16branchless<void>(buffer, buffer+1, buffer+2, buffer+3, buffer+4, buffer+5, buffer+6, buffer+7, buffer+8, buffer+9, buffer+10, buffer+11, buffer+12, buffer+13, buffer+14, buffer+15, std::less<int32_t>{}); return;
                        case 17: sort17branchless<void>(buffer, buffer+1, buffer+2, buffer+3, buffer+4, buffer+5, buffer+6, buffer+7, buffer+8, buffer+9, buffer+10, buffer+11, buffer+12, buffer+13, buffer+14, buffer+15, buffer+16, std::less<int32_t>{}); return;
                        case 18: sort18branchless<void>(buffer, buffer+1, buffer+2, buffer+3, buffer+4, buffer+5, buffer+6, buffer+7, buffer+8, buffer+9, buffer+10, buffer+11, buffer+12, buffer+13, buffer+14, buffer+15, buffer+16, buffer+17, std::less<int32_t>{}); return;
                        case 19: sort19branchless<void>(buffer, buffer+1, buffer+2, buffer+3, buffer+4, buffer+5, buffer+6, buffer+7, buffer+8, buffer+9, buffer+10, buffer+11, buffer+12, buffer+13, buffer+14, buffer+15, buffer+16, buffer+17, buffer+18, std::less<int32_t>{}); return;
                        case 20: sort20branchless<void>(buffer, buffer+1, buffer+2, buffer+3, buffer+4, buffer+5, buffer+6, buffer+7, buffer+8, buffer+9, buffer+10, buffer+11, buffer+12, buffer+13, buffer+14, buffer+15, buffer+16, buffer+17, buffer+18, buffer+19, std::less<int32_t>{}); return;
                        case 21: sort21branchless<void>(buffer, buffer+1, buffer+2, buffer+3, buffer+4, buffer+5, buffer+6, buffer+7, buffer+8, buffer+9, buffer+10, buffer+11, buffer+12, buffer+13, buffer+14, buffer+15, buffer+16, buffer+17, buffer+18, buffer+19, buffer+20, std::less<int32_t>{}); return;
                        case 22: sort22branchless<void>(buffer, buffer+1, buffer+2, buffer+3, buffer+4, buffer+5, buffer+6, buffer+7, buffer+8, buffer+9, buffer+10, buffer+11, buffer+12, buffer+13, buffer+14, buffer+15, buffer+16, buffer+17, buffer+18, buffer+19, buffer+20, buffer+21, std::less<int32_t>{}); return;
                        case 23: sort23branchless<void>(buffer, buffer+1, buffer+2, buffer+3, buffer+4, buffer+5, buffer+6, buffer+7, buffer+8, buffer+9, buffer+10, buffer+11, buffer+12, buffer+13, buffer+14, buffer+15, buffer+16, buffer+17, buffer+18, buffer+19, buffer+20, buffer+21, buffer+22, std::less<int32_t>{}); return;
                        case 24: sort24branchless<void>(buffer, buffer+1, buffer+2, buffer+3, buffer+4, buffer+5, buffer+6, buffer+7, buffer+8, buffer+9, buffer+10, buffer+11, buffer+12, buffer+13, buffer+14, buffer+15, buffer+16, buffer+17, buffer+18, buffer+19, buffer+20, buffer+21, buffer+22, buffer+23, std::less<int32_t>{}); return;
                        case 25: sort25branchless<void>(buffer, buffer+1, buffer+2, buffer+3, buffer+4, buffer+5, buffer+6, buffer+7, buffer+8, buffer+9, buffer+10, buffer+11, buffer+12, buffer+13, buffer+14, buffer+15, buffer+16, buffer+17, buffer+18, buffer+19, buffer+20, buffer+21, buffer+22, buffer+23, buffer+24, std::less<int32_t>{}); return;
                        case 26: sort26branchless<void>(buffer, buffer+1, buffer+2, buffer+3, buffer+4, buffer+5, buffer+6, buffer+7, buffer+8, buffer+9, buffer+10, buffer+11, buffer+12, buffer+13, buffer+14, buffer+15, buffer+16, buffer+17, buffer+18, buffer+19, buffer+20, buffer+21, buffer+22, buffer+23, buffer+24, buffer+25, std::less<int32_t>{}); return;
                        case 27: sort27branchless<void>(buffer, buffer+1, buffer+2, buffer+3, buffer+4, buffer+5, buffer+6, buffer+7, buffer+8, buffer+9, buffer+10, buffer+11, buffer+12, buffer+13, buffer+14, buffer+15, buffer+16, buffer+17, buffer+18, buffer+19, buffer+20, buffer+21, buffer+22, buffer+23, buffer+24, buffer+25, buffer+26, std::less<int32_t>{}); return;
                        case 28: sort28branchless<void>(buffer, buffer+1, buffer+2, buffer+3, buffer+4, buffer+5, buffer+6, buffer+7, buffer+8, buffer+9, buffer+10, buffer+11, buffer+12, buffer+13, buffer+14, buffer+15, buffer+16, buffer+17, buffer+18, buffer+19, buffer+20, buffer+21, buffer+22, buffer+23, buffer+24, buffer+25, buffer+26, buffer+27, std::less<int32_t>{}); return;
                        case 29: sort29branchless<void>(buffer, buffer+1, buffer+2, buffer+3, buffer+4, buffer+5, buffer+6, buffer+7, buffer+8, buffer+9, buffer+10, buffer+11, buffer+12, buffer+13, buffer+14, buffer+15, buffer+16, buffer+17, buffer+18, buffer+19, buffer+20, buffer+21, buffer+22, buffer+23, buffer+24, buffer+25, buffer+26, buffer+27, buffer+28, std::less<int32_t>{}); return;
                        case 30: sort30branchless<void>(buffer, buffer+1, buffer+2, buffer+3, buffer+4, buffer+5, buffer+6, buffer+7, buffer+8, buffer+9, buffer+10, buffer+11, buffer+12, buffer+13, buffer+14, buffer+15, buffer+16, buffer+17, buffer+18, buffer+19, buffer+20, buffer+21, buffer+22, buffer+23, buffer+24, buffer+25, buffer+26, buffer+27, buffer+28, buffer+29, std::less<int32_t>{}); return;
                        case 31: sort31branchless<void>(buffer, buffer+1, buffer+2, buffer+3, buffer+4, buffer+5, buffer+6, buffer+7, buffer+8, buffer+9, buffer+10, buffer+11, buffer+12, buffer+13, buffer+14, buffer+15, buffer+16, buffer+17, buffer+18, buffer+19, buffer+20, buffer+21, buffer+22, buffer+23, buffer+24, buffer+25, buffer+26, buffer+27, buffer+28, buffer+29, buffer+30, std::less<int32_t>{}); return;
                        case 32: sort32branchless<void>(buffer, buffer+1, buffer+2, buffer+3, buffer+4, buffer+5, buffer+6, buffer+7, buffer+8, buffer+9, buffer+10, buffer+11, buffer+12, buffer+13, buffer+14, buffer+15, buffer+16, buffer+17, buffer+18, buffer+19, buffer+20, buffer+21, buffer+22, buffer+23, buffer+24, buffer+25, buffer+26, buffer+27, buffer+28, buffer+29, buffer+30, buffer+31, std::less<int32_t>{}); return;
                        case 64: sort64branchless<void>(
                            buffer, buffer+1, buffer+2, buffer+3, buffer+4, buffer+5, buffer+6, buffer+7,
                            buffer+8, buffer+9, buffer+10, buffer+11, buffer+12, buffer+13, buffer+14, buffer+15,
                            buffer+16, buffer+17, buffer+18, buffer+19, buffer+20, buffer+21, buffer+22, buffer+23,
                            buffer+24, buffer+25, buffer+26, buffer+27, buffer+28, buffer+29, buffer+30, buffer+31,
                            buffer+32, buffer+33, buffer+34, buffer+35, buffer+36, buffer+37, buffer+38, buffer+39,
                            buffer+40, buffer+41, buffer+42, buffer+43, buffer+44, buffer+45, buffer+46, buffer+47,
                            buffer+48, buffer+49, buffer+50, buffer+51, buffer+52, buffer+53, buffer+54, buffer+55,
                            buffer+56, buffer+57, buffer+58, buffer+59, buffer+60, buffer+61, buffer+62, buffer+63,
                            std::less<int32_t>{}); return;
                    }
                    break;
                case SmallSortStrategy::Branchless:
                    switch (size) {
                        case 3:  sort3branchless<void>(buffer, buffer+1, buffer+2, std::less<int32_t>{}); return;
                        case 4:  sort4branchless<void>(buffer, buffer+1, buffer+2, buffer+3, std::less<int32_t>{}); return;
                        case 5:  sort5branchless<void>(buffer, buffer+1, buffer+2, buffer+3, buffer+4, std::less<int32_t>{}); return;
                        case 6:  sort6branchless<void>(buffer, buffer+1, buffer+2, buffer+3, buffer+4, buffer+5, std::less<int32_t>{}); return;
                        case 7:  sort7branchless<void>(buffer, buffer+1, buffer+2, buffer+3, buffer+4, buffer+5, buffer+6, std::less<int32_t>{}); return;
                        case 8:  sort8branchless<void>(buffer, buffer+1, buffer+2, buffer+3, buffer+4, buffer+5, buffer+6, buffer+7, std::less<int32_t>{}); return;
                        case 9:  sort9branchless<void>(buffer, buffer+1, buffer+2, buffer+3, buffer+4, buffer+5, buffer+6, buffer+7, buffer+8, std::less<int32_t>{}); return;
                        case 10: sort10branchless<void>(buffer, buffer+1, buffer+2, buffer+3, buffer+4, buffer+5, buffer+6, buffer+7, buffer+8, buffer+9, std::less<int32_t>{}); return;
                        case 11: sort11branchless<void>(buffer, buffer+1, buffer+2, buffer+3, buffer+4, buffer+5, buffer+6, buffer+7, buffer+8, buffer+9, buffer+10, std::less<int32_t>{}); return;
                        case 12: sort12branchless<void>(buffer, buffer+1, buffer+2, buffer+3, buffer+4, buffer+5, buffer+6, buffer+7, buffer+8, buffer+9, buffer+10, buffer+11, std::less<int32_t>{}); return;
                        case 13: sort13branchless<void>(buffer, buffer+1, buffer+2, buffer+3, buffer+4, buffer+5, buffer+6, buffer+7, buffer+8, buffer+9, buffer+10, buffer+11, buffer+12, std::less<int32_t>{}); return;
                        case 14: sort14branchless<void>(buffer, buffer+1, buffer+2, buffer+3, buffer+4, buffer+5, buffer+6, buffer+7, buffer+8, buffer+9, buffer+10, buffer+11, buffer+12, buffer+13, std::less<int32_t>{}); return;
                        case 15: sort15branchless<void>(buffer, buffer+1, buffer+2, buffer+3, buffer+4, buffer+5, buffer+6, buffer+7, buffer+8, buffer+9, buffer+10, buffer+11, buffer+12, buffer+13, buffer+14, std::less<int32_t>{}); return;
                        case 16: sort16branchless<void>(buffer, buffer+1, buffer+2, buffer+3, buffer+4, buffer+5, buffer+6, buffer+7, buffer+8, buffer+9, buffer+10, buffer+11, buffer+12, buffer+13, buffer+14, buffer+15, std::less<int32_t>{}); return;
                        case 17: sort17branchless<void>(buffer, buffer+1, buffer+2, buffer+3, buffer+4, buffer+5, buffer+6, buffer+7, buffer+8, buffer+9, buffer+10, buffer+11, buffer+12, buffer+13, buffer+14, buffer+15, buffer+16, std::less<int32_t>{}); return;
                        case 18: sort18branchless<void>(buffer, buffer+1, buffer+2, buffer+3, buffer+4, buffer+5, buffer+6, buffer+7, buffer+8, buffer+9, buffer+10, buffer+11, buffer+12, buffer+13, buffer+14, buffer+15, buffer+16, buffer+17, std::less<int32_t>{}); return;
                        case 19: sort19branchless<void>(buffer, buffer+1, buffer+2, buffer+3, buffer+4, buffer+5, buffer+6, buffer+7, buffer+8, buffer+9, buffer+10, buffer+11, buffer+12, buffer+13, buffer+14, buffer+15, buffer+16, buffer+17, buffer+18, std::less<int32_t>{}); return;
                        case 20: sort20branchless<void>(buffer, buffer+1, buffer+2, buffer+3, buffer+4, buffer+5, buffer+6, buffer+7, buffer+8, buffer+9, buffer+10, buffer+11, buffer+12, buffer+13, buffer+14, buffer+15, buffer+16, buffer+17, buffer+18, buffer+19, std::less<int32_t>{}); return;
                        case 21: sort21branchless<void>(buffer, buffer+1, buffer+2, buffer+3, buffer+4, buffer+5, buffer+6, buffer+7, buffer+8, buffer+9, buffer+10, buffer+11, buffer+12, buffer+13, buffer+14, buffer+15, buffer+16, buffer+17, buffer+18, buffer+19, buffer+20, std::less<int32_t>{}); return;
                        case 22: sort22branchless<void>(buffer, buffer+1, buffer+2, buffer+3, buffer+4, buffer+5, buffer+6, buffer+7, buffer+8, buffer+9, buffer+10, buffer+11, buffer+12, buffer+13, buffer+14, buffer+15, buffer+16, buffer+17, buffer+18, buffer+19, buffer+20, buffer+21, std::less<int32_t>{}); return;
                        case 23: sort23branchless<void>(buffer, buffer+1, buffer+2, buffer+3, buffer+4, buffer+5, buffer+6, buffer+7, buffer+8, buffer+9, buffer+10, buffer+11, buffer+12, buffer+13, buffer+14, buffer+15, buffer+16, buffer+17, buffer+18, buffer+19, buffer+20, buffer+21, buffer+22, std::less<int32_t>{}); return;
                        case 24: sort24branchless<void>(buffer, buffer+1, buffer+2, buffer+3, buffer+4, buffer+5, buffer+6, buffer+7, buffer+8, buffer+9, buffer+10, buffer+11, buffer+12, buffer+13, buffer+14, buffer+15, buffer+16, buffer+17, buffer+18, buffer+19, buffer+20, buffer+21, buffer+22, buffer+23, std::less<int32_t>{}); return;
                        case 25: sort25branchless<void>(buffer, buffer+1, buffer+2, buffer+3, buffer+4, buffer+5, buffer+6, buffer+7, buffer+8, buffer+9, buffer+10, buffer+11, buffer+12, buffer+13, buffer+14, buffer+15, buffer+16, buffer+17, buffer+18, buffer+19, buffer+20, buffer+21, buffer+22, buffer+23, buffer+24, std::less<int32_t>{}); return;
                        case 26: sort26branchless<void>(buffer, buffer+1, buffer+2, buffer+3, buffer+4, buffer+5, buffer+6, buffer+7, buffer+8, buffer+9, buffer+10, buffer+11, buffer+12, buffer+13, buffer+14, buffer+15, buffer+16, buffer+17, buffer+18, buffer+19, buffer+20, buffer+21, buffer+22, buffer+23, buffer+24, buffer+25, std::less<int32_t>{}); return;
                        case 27: sort27branchless<void>(buffer, buffer+1, buffer+2, buffer+3, buffer+4, buffer+5, buffer+6, buffer+7, buffer+8, buffer+9, buffer+10, buffer+11, buffer+12, buffer+13, buffer+14, buffer+15, buffer+16, buffer+17, buffer+18, buffer+19, buffer+20, buffer+21, buffer+22, buffer+23, buffer+24, buffer+25, buffer+26, std::less<int32_t>{}); return;
                        case 28: sort28branchless<void>(buffer, buffer+1, buffer+2, buffer+3, buffer+4, buffer+5, buffer+6, buffer+7, buffer+8, buffer+9, buffer+10, buffer+11, buffer+12, buffer+13, buffer+14, buffer+15, buffer+16, buffer+17, buffer+18, buffer+19, buffer+20, buffer+21, buffer+22, buffer+23, buffer+24, buffer+25, buffer+26, buffer+27, std::less<int32_t>{}); return;
                        case 29: sort29branchless<void>(buffer, buffer+1, buffer+2, buffer+3, buffer+4, buffer+5, buffer+6, buffer+7, buffer+8, buffer+9, buffer+10, buffer+11, buffer+12, buffer+13, buffer+14, buffer+15, buffer+16, buffer+17, buffer+18, buffer+19, buffer+20, buffer+21, buffer+22, buffer+23, buffer+24, buffer+25, buffer+26, buffer+27, buffer+28, std::less<int32_t>{}); return;
                        case 30: sort30branchless<void>(buffer, buffer+1, buffer+2, buffer+3, buffer+4, buffer+5, buffer+6, buffer+7, buffer+8, buffer+9, buffer+10, buffer+11, buffer+12, buffer+13, buffer+14, buffer+15, buffer+16, buffer+17, buffer+18, buffer+19, buffer+20, buffer+21, buffer+22, buffer+23, buffer+24, buffer+25, buffer+26, buffer+27, buffer+28, buffer+29, std::less<int32_t>{}); return;
                        case 31: sort31branchless<void>(buffer, buffer+1, buffer+2, buffer+3, buffer+4, buffer+5, buffer+6, buffer+7, buffer+8, buffer+9, buffer+10, buffer+11, buffer+12, buffer+13, buffer+14, buffer+15, buffer+16, buffer+17, buffer+18, buffer+19, buffer+20, buffer+21, buffer+22, buffer+23, buffer+24, buffer+25, buffer+26, buffer+27, buffer+28, buffer+29, buffer+30, std::less<int32_t>{}); return;
                        case 32: sort32branchless<void>(buffer, buffer+1, buffer+2, buffer+3, buffer+4, buffer+5, buffer+6, buffer+7, buffer+8, buffer+9, buffer+10, buffer+11, buffer+12, buffer+13, buffer+14, buffer+15, buffer+16, buffer+17, buffer+18, buffer+19, buffer+20, buffer+21, buffer+22, buffer+23, buffer+24, buffer+25, buffer+26, buffer+27, buffer+28, buffer+29, buffer+30, buffer+31, std::less<int32_t>{}); return;
                        case 64: sort64branchless<void>(
                            buffer, buffer+1, buffer+2, buffer+3, buffer+4, buffer+5, buffer+6, buffer+7,
                            buffer+8, buffer+9, buffer+10, buffer+11, buffer+12, buffer+13, buffer+14, buffer+15,
                            buffer+16, buffer+17, buffer+18, buffer+19, buffer+20, buffer+21, buffer+22, buffer+23,
                            buffer+24, buffer+25, buffer+26, buffer+27, buffer+28, buffer+29, buffer+30, buffer+31,
                            buffer+32, buffer+33, buffer+34, buffer+35, buffer+36, buffer+37, buffer+38, buffer+39,
                            buffer+40, buffer+41, buffer+42, buffer+43, buffer+44, buffer+45, buffer+46, buffer+47,
                            buffer+48, buffer+49, buffer+50, buffer+51, buffer+52, buffer+53, buffer+54, buffer+55,
                            buffer+56, buffer+57, buffer+58, buffer+59, buffer+60, buffer+61, buffer+62, buffer+63,
                            std::less<int32_t>{}); return;
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
                case SmallSortStrategy::Selection:
                case SmallSortStrategy::Insertion:
                case SmallSortStrategy::Bubble:
                case SmallSortStrategy::Heap:
                    break;
            }
        }
        switch (strat) {
            case SmallSortStrategy::Selection: insertionSort(buffer, size); return;
            case SmallSortStrategy::Insertion: insertionSort(buffer, size); return;
            case SmallSortStrategy::Bubble:    bubbleSort(buffer, size);    return;
            case SmallSortStrategy::Heap:      heapSort(buffer, size);      return;
            default:                           break;
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
    if (len <= 64) { insertionSort(arr + left, len); return; }
    //if (len <= 0) { heapSort(arr + left, len); return; }
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

// Variante mergeSort8: suddivide in blocchi di dimensione ≤8 in un unico passaggio e poi fonde tutti i blocchi in un unico array ordinato.
// 1) Calcola quanti blocchi (ceil(len/8)).
// 2) Chiama mergeSort8 su ciascun blocco (ricorsivamente), finché la dimensione è ≤8 chiama smallSort.
// 3) Esegue un multi-merge iterativo di tutti i blocchi.

template<typename T>
void mergeSort8(T* arr, int left, int right, int maxSmall, SmallSortStrategy ss) {
    int len = right - left + 1;
    // caso base: piccolo
    if (len <= 8 && len <= maxSmall) {
        smallSort(arr + left, len, maxSmall, ss);
        return;
    }
    // suddivisione in blocchi di max 8
    int numBlocks = (len + 7) / 8;
    // array di indici di inizio e fine
    std::vector<std::pair<int,int>> ranges;
    ranges.reserve(numBlocks);
    for (int b = 0; b < numBlocks; ++b) {
        int start = left + b*8;
        int end   = std::min(start + 8 - 1, right);
        ranges.emplace_back(start, end);
        // ordina ciascun blocco
        mergeSort8(arr, start, end, maxSmall, ss);
    }
    // multi-merge: utilizza un buffer temporaneo per fondere progressivamente i blocchi
    std::vector<T> tmp(len);
    // inizializza cursori
    std::vector<int> idx(numBlocks, 0);
    // merge fino a consumare tutti i blocchi
    for (int k = 0; k < len; ++k) {
        T* bestVal = nullptr;
        int bestBlock = -1;
        for (int b = 0; b < numBlocks; ++b) {
            int start = ranges[b].first;
            int end   = ranges[b].second;
            int off   = idx[b];
            if (off > end - start) continue;
            T& candidate = arr[start + off];
            if (!bestVal || candidate < *bestVal) {
                bestVal = &candidate;
                bestBlock = b;
            }
        }
        tmp[k] = *bestVal;
        // incrementa cursore
        idx[bestBlock]++;
    }
    // copia indietro
    for (int i = 0; i < len; ++i) arr[left + i] = tmp[i];
}

// Introsort helper: funzione ricorsiva
template<typename T>
void introSortUtil(T* arr, int left, int right, int depthLimit, int maxSmall, SmallSortStrategy strat) {
    int len = right - left + 1;
    if (len <= 1)
        return;
    if (len <= maxSmall) {
        smallSort(arr + left, len, maxSmall, strat);
        return;
    }
    if (len <= 24) {
        insertionSort(arr + left, len);
        return;
    }
    if (depthLimit == 0) {
        heapSort(arr + left, len);
        return;
    }
    int pi = partition(arr, left, right);
    introSortUtil(arr, left, pi - 1, depthLimit - 1, maxSmall, strat);
    introSortUtil(arr, pi + 1, right, depthLimit - 1, maxSmall, strat);
}

// Interfaccia pubblica per Introsort
template<typename T>
void introSort(T* arr, int left, int right, int maxSmall, SmallSortStrategy strat) {
    int len = right - left + 1;
    int depthLimit = 2 * static_cast<int>(std::log2(len));
    introSortUtil(arr, left, right, depthLimit, maxSmall, strat);
}

// Aggiunta della funzione timSort
template<typename T>
void timSort(T* arr, int size, int maxSmall, SmallSortStrategy strat) {
    const int RUN = maxSmall;
    // Passo 1: Ordina i piccoli blocchi con insertionSort
    for (int i = 0; i < size; i += RUN) {
        int runSize = std::min(RUN, size - i);
        smallSort(arr + i, runSize, maxSmall, strat);
    }
    // Passo 2: Merge iterativo dei blocchi ordinati
    for (int run = RUN; run < size; run *= 2) {
        for (int left = 0; left < size; left += 2 * run) {
            int mid = left + run - 1;
            int right = std::min(left + 2 * run - 1, size - 1);
            if (mid < right) {
                std::vector<T> tmp(right - left + 1);
                int i = left, j = mid + 1, k = 0;
                while (i <= mid && j <= right) {
                    if (arr[i] <= arr[j])
                        tmp[k++] = arr[i++];
                    else
                        tmp[k++] = arr[j++];
                }
                while (i <= mid) tmp[k++] = arr[i++];
                while (j <= right) tmp[k++] = arr[j++];
                for (int t = 0; t < tmp.size(); ++t)
                    arr[left + t] = tmp[t];
            }
        }
    }
}


// Merge Sort template
template<typename T>
void newSort(T* arr, int left, int right, int maxSmall, int maxAux, SmallSortStrategy strat1, SmallSortStrategy strat2, int depth=0) {
    int len = right - left + 1;
    if (len <= 1) return;
    if (len <= maxSmall) { smallSort(arr + left, len, maxSmall, strat1); return; }
    //if (maxSmall < len && len <= maxAux) { smallSort(arr + left, len, maxAux, strat2); return; }
    if (maxAux < len && len <= 5000 && depth == 0) { quickSort(arr, left, right, maxSmall, strat1); return; }
    int mid = left + (right - left) / 2;
    newSort(arr, left, mid, maxSmall, maxAux, strat1, strat2, depth + 1);
    newSort(arr, mid + 1, right, maxSmall, maxAux, strat1, strat2, depth + 1);
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
