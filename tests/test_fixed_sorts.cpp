
#if defined(__x86_64__) || defined(_M_X64)
#include "../include/alpha_dev_sorts_x86-64.h"
#elif defined(__aarch64__)
#include "../include/alpha_dev_sorts_arm64.h"
#else
#error "Unsupported architecture for AlphaDev sorts"
#endif
#include "branchless_fixed_sorts.h"
#include "branching_fixed_sorts.h"

#include <gtest/gtest.h>
#include <algorithm>  // std::is_sorted
#include <vector>


using std::vector;

// Macro per generare test di SortNX
#define TEST_SMALL(N, Func) \
TEST(SmallSortTest, N##_##Func) { \
    int buf[N]; \
    for (int i = 0; i < N; ++i) buf[i] = N - i; \
    Func(buf); \
    EXPECT_TRUE(std::is_sorted(buf, buf + N)); \
}

// AlphaDev
TEST_SMALL(3, Sort3AlphaDev)
TEST_SMALL(4, Sort4AlphaDev)
TEST_SMALL(5, Sort5AlphaDev)
TEST_SMALL(6, Sort6AlphaDev)
TEST_SMALL(7, Sort7AlphaDev)
TEST_SMALL(8, Sort8AlphaDev)

/*
// Branchless
TEST_SMALL(3, sort3branchless)
TEST_SMALL(4, sort4branchless)
TEST_SMALL(5, sort5branchless)
TEST_SMALL(6, sort6branchless)
TEST_SMALL(7, sort7branchless)
TEST_SMALL(8, sort8branchless)

// Branching
TEST_SMALL(3, sort3branching)
TEST_SMALL(4, sort4branching)
TEST_SMALL(5, sort5branching)
TEST_SMALL(6, sort6branching)
TEST_SMALL(7, sort7branching)
TEST_SMALL(8, sort8branching)*/