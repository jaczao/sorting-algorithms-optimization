#include "divide_et_impera_algos.h"
#include <algorithm>  // for std::copy and std::sort

// Helper: manual swap for two elements
inline void sort2(int* a, int* b) {
    if (*a > *b) std::swap(*a, *b);
}

// Dispatch small sorts: size from 2 to maxSmall with chosen strategy
void smallSort(int* buffer, int size, int maxSmall, SmallSortStrategy strat) {
    if (size < 2) return;
    if (size == 2 && maxSmall >= 2) {
        sort2(&buffer[0], &buffer[1]);
        return;
    }
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
    // Fallback to std::sort
    std::sort(buffer, buffer + size);
}

// Merge Sort with configurable small-sort threshold and strategy
void merge(int* arr, int left, int mid, int right, int maxSmall, SmallSortStrategy strat) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    int total = n1 + n2;
    if (total <= maxSmall) {
        smallSort(arr + left, total, maxSmall, strat);
        return;
    }
    int* L = new int[n1];
    int* R = new int[n2];
    std::copy(arr + left, arr + mid + 1, L);
    std::copy(arr + mid + 1, arr + right + 1, R);

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        arr[k++] = (L[i] <= R[j]) ? L[i++] : R[j++];
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];

    delete[] L;
    delete[] R;
}

void mergeSort(int* arr, int left, int right, int maxSmall, SmallSortStrategy strat) {
    int size = right - left + 1;
    if (size <= 1) return;
    if (size <= maxSmall) {
        smallSort(arr + left, size, maxSmall, strat);
        return;
    }
    int mid = left + (right - left) / 2;
    mergeSort(arr, left, mid, maxSmall, strat);
    mergeSort(arr, mid + 1, right, maxSmall, strat);
    merge(arr, left, mid, right, maxSmall, strat);
}

// Quick Sort with pivot from the left, configurable small-sort threshold/strategy
int partition(int* arr, int left, int right) {
    int pivot = arr[left];
    int i = left + 1;
    int j = right;
    while (true) {
        while (i <= j && arr[i] <= pivot) ++i;
        while (i <= j && arr[j] > pivot) --j;
        if (i > j) break;
        std::swap(arr[i], arr[j]);
    }
    std::swap(arr[left], arr[j]);
    return j;
}

void quickSort(int* arr, int left, int right, int maxSmall, SmallSortStrategy strat) {
    int size = right - left + 1;
    if (size <= 1) return;
    if (size <= maxSmall) {
        smallSort(arr + left, size, maxSmall, strat);
        return;
    }
    int pi = partition(arr, left, right);
    quickSort(arr, left, pi - 1, maxSmall, strat);
    quickSort(arr, pi + 1, right, maxSmall, strat);
}

// Overloads with default maxSmall = 8, default strategy = AlphaDev
inline void mergeSort(int* arr, int left, int right) {
    mergeSort(arr, left, right, 8, SmallSortStrategy::AlphaDev);
}
inline void quickSort(int* arr, int left, int right) {
    quickSort(arr, left, right, 8, SmallSortStrategy::AlphaDev);
}
