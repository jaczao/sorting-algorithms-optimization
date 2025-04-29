#pragma once

#include <cstddef>
#include "alpha_dev_sorts.h"
#include "branchless_fixed_sorts.h"
#include "branching_fixed_sorts.h"

enum class SmallSortStrategy { AlphaDev, Branchless, Branching };

void mergeSort(int* arr, int left, int right,
               int maxSmall = 8,
               SmallSortStrategy strat = SmallSortStrategy::AlphaDev);
void quickSort(int* arr, int left, int right,
               int maxSmall = 8,
               SmallSortStrategy strat = SmallSortStrategy::AlphaDev);
