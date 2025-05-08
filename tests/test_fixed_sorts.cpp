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
#include <random>
#include <functional>

using std::vector;

// Macro per generare test di SortNX
#define TEST_SMALL(N, Func)                                           \
TEST(SmallSortTest, N##_##Func) {                                     \
    std::mt19937 gen(std::random_device{}());                         \
    std::uniform_int_distribution<int> dist(-1000, 1000);              \
    for (int t = 0; t < (1 << N); ++t) {                                \
        int buf[N];                                                   \
        for (int i = 0; i < N; ++i) {                                   \
            buf[i] = dist(gen);                                         \
        }                                                             \
        Func(buf);                                                    \
        EXPECT_TRUE(std::is_sorted(buf, buf + N));                      \
    }                                                                 \
}

// AlphaDev
TEST_SMALL(3, Sort3AlphaDev)
TEST_SMALL(4, Sort4AlphaDev)
TEST_SMALL(5, Sort5AlphaDev)
TEST_SMALL(6, Sort6AlphaDev)
TEST_SMALL(7, Sort7AlphaDev)
TEST_SMALL(8, Sort8AlphaDev)

/*// Test per tutte le funzioni branchless_fixed_sorts.h (da 3 a 8)
#define TEST_BRANCHLESS(N, Func) \
TEST(BranchlessSortTest, Func) { \
    std::mt19937 gen(std::random_device{}()); \
    std::uniform_int_distribution<int> dist(-1000, 1000); \
    for (int t = 0; t < 10000; ++t) { \
        int buf[N]; \
        for (int i = 0; i < N; ++i) buf[i] = dist(gen); \
        Func(buf); \
        EXPECT_TRUE(std::is_sorted(buf, buf + N)); \
    } \
}

TEST_BRANCHLESS(3, sort3branchless)
TEST_BRANCHLESS(4, sort4branchless)
TEST_BRANCHLESS(5, sort5branchless)
TEST_BRANCHLESS(6, sort6branchless)
TEST_BRANCHLESS(7, sort7branchless)
TEST_BRANCHLESS(8, sort8branchless)*/

// Test per tutte le funzioni branchless_fixed_sorts.h (da 3 a 8, versione template)
#define TEST_BRANCHLESS_TEMPLATE(N, Func) \
TEST(BranchlessSortTest, Func) { \
    std::mt19937 gen(std::random_device{}()); \
    std::uniform_int_distribution<int> dist(-1000, 1000); \
    for (int t = 0; t < 10000; ++t) { \
        int buf[N]; \
        for (int i = 0; i < N; ++i) buf[i] = dist(gen); \
        Func(buf, buf+1, buf+2, std::less<int>()); \
        EXPECT_TRUE(std::is_sorted(buf, buf + N)); \
    } \
}

// Specializzazione per ogni N (3-8)
TEST(BranchlessSortTest, sort3branchless) {
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> dist(-1000, 1000);
    for (int t = 0; t < 10000; ++t) {
        int buf[3];
        for (int i = 0; i < 3; ++i) buf[i] = dist(gen);
        sort3branchless<void>(buf, buf+1, buf+2, std::less<int>());
        EXPECT_TRUE(std::is_sorted(buf, buf + 3));
    }
}
TEST(BranchlessSortTest, sort4branchless) {
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> dist(-1000, 1000);
    for (int t = 0; t < 10000; ++t) {
        int buf[4];
        for (int i = 0; i < 4; ++i) buf[i] = dist(gen);
        sort4branchless<void>(buf, buf+1, buf+2, buf+3, std::less<int>());
        EXPECT_TRUE(std::is_sorted(buf, buf + 4));
    }
}
TEST(BranchlessSortTest, sort5branchless) {
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> dist(-1000, 1000);
    for (int t = 0; t < 10000; ++t) {
        int buf[5];
        for (int i = 0; i < 5; ++i) buf[i] = dist(gen);
        sort5branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, std::less<int>());
        EXPECT_TRUE(std::is_sorted(buf, buf + 5));
    }
}
TEST(BranchlessSortTest, sort6branchless) {
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> dist(-1000, 1000);
    for (int t = 0; t < 10000; ++t) {
        int buf[6];
        for (int i = 0; i < 6; ++i) buf[i] = dist(gen);
        sort6branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, std::less<int>());
        EXPECT_TRUE(std::is_sorted(buf, buf + 6));
    }
}
TEST(BranchlessSortTest, sort7branchless) {
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> dist(-1000, 1000);
    for (int t = 0; t < 10000; ++t) {
        int buf[7];
        for (int i = 0; i < 7; ++i) buf[i] = dist(gen);
        sort7branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, std::less<int>());
        EXPECT_TRUE(std::is_sorted(buf, buf + 7));
    }
}
TEST(BranchlessSortTest, sort8branchless) {
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> dist(-1000, 1000);
    for (int t = 0; t < 10000; ++t) {
        int buf[8];
        for (int i = 0; i < 8; ++i) buf[i] = dist(gen);
        sort8branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, std::less<int>());
        EXPECT_TRUE(std::is_sorted(buf, buf + 8));
    }
}
TEST(BranchlessSortTest, sort9branchless) {
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> dist(-1000, 1000);
    for (int t = 0; t < 10000; ++t) {
        int buf[9];
        for (int i = 0; i < 9; ++i) buf[i] = dist(gen);
        sort9branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, std::less<int>());
        EXPECT_TRUE(std::is_sorted(buf, buf + 9));
    }
}
TEST(BranchlessSortTest, sort10branchless) {
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> dist(-1000, 1000);
    for (int t = 0; t < 10000; ++t) {
        int buf[10];
        for (int i = 0; i < 10; ++i) buf[i] = dist(gen);
        sort10branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, std::less<int>());
        EXPECT_TRUE(std::is_sorted(buf, buf + 10));
    }
}
TEST(BranchlessSortTest, sort11branchless) {
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> dist(-1000, 1000);
    for (int t = 0; t < 10000; ++t) {
        int buf[11];
        for (int i = 0; i < 11; ++i) buf[i] = dist(gen);
        sort11branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, std::less<int>());
        EXPECT_TRUE(std::is_sorted(buf, buf + 11));
    }
}
TEST(BranchlessSortTest, sort12branchless) {
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> dist(-1000, 1000);
    for (int t = 0; t < 10000; ++t) {
        int buf[12];
        for (int i = 0; i < 12; ++i) buf[i] = dist(gen);
        sort12branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, std::less<int>());
        EXPECT_TRUE(std::is_sorted(buf, buf + 12));
    }
}
TEST(BranchlessSortTest, sort13branchless) {
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> dist(-1000, 1000);
    for (int t = 0; t < 10000; ++t) {
        int buf[13];
        for (int i = 0; i < 13; ++i) buf[i] = dist(gen);
        sort13branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, std::less<int>());
        EXPECT_TRUE(std::is_sorted(buf, buf + 13));
    }
}
TEST(BranchlessSortTest, sort14branchless) {
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> dist(-1000, 1000);
    for (int t = 0; t < 10000; ++t) {
        int buf[14];
        for (int i = 0; i < 14; ++i) buf[i] = dist(gen);
        sort14branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, std::less<int>());
        EXPECT_TRUE(std::is_sorted(buf, buf + 14));
    }
}

/*
// Branching
TEST_SMALL(3, sort3branching)
TEST_SMALL(4, sort4branching)
TEST_SMALL(5, sort5branching)
TEST_SMALL(6, sort6branching)
TEST_SMALL(7, sort7branching)
TEST_SMALL(8, sort8branching)*/