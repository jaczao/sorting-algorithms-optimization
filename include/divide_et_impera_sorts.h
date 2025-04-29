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


enum class SmallSortStrategy { AlphaDev, Branchless, Branching };

void mergeSort(int* arr, int left, int right,
               int maxSmall = 8,
               SmallSortStrategy strat = SmallSortStrategy::AlphaDev);
void quickSort(int* arr, int left, int right,
               int maxSmall = 8,
               SmallSortStrategy strat = SmallSortStrategy::AlphaDev);
