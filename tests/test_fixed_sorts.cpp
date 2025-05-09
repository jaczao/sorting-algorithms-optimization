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
    for (int t = 0; t < (1 << (N+15)); ++t) {                                \
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

template <int N, typename SortFunc>
void test_branchless_sort(SortFunc&& func) {
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> dist(-1000, 1000);
    for (int t = 0; t < 10000; ++t) {
        int buf[N];
        for (int i = 0; i < N; ++i) buf[i] = dist(gen);
        func(buf);
        EXPECT_TRUE(std::is_sorted(buf, buf + N));
    }
}

// Helper per chiamare la funzione template branchless con forwarding degli iteratori
// (N=3..64)
template <int N>
void call_sort_branchless(int* buf) {
    if constexpr (N == 3)
        sort3branchless<void>(buf, buf+1, buf+2, std::less<int>());
    else if constexpr (N == 4)
        sort4branchless<void>(buf, buf+1, buf+2, buf+3, std::less<int>());
    else if constexpr (N == 5)
        sort5branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, std::less<int>());
    else if constexpr (N == 6)
        sort6branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, std::less<int>());
    else if constexpr (N == 7)
        sort7branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, std::less<int>());
    else if constexpr (N == 8)
        sort8branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, std::less<int>());
    else if constexpr (N == 9)
        sort9branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, std::less<int>());
    else if constexpr (N == 10)
        sort10branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, std::less<int>());
    else if constexpr (N == 11)
        sort11branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, std::less<int>());
    else if constexpr (N == 12)
        sort12branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, std::less<int>());
    else if constexpr (N == 13)
        sort13branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, std::less<int>());
    else if constexpr (N == 14)
        sort14branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, std::less<int>());
    else if constexpr (N == 15)
        sort15branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, std::less<int>());
    else if constexpr (N == 16)
        sort16branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, std::less<int>());
    else if constexpr (N == 17)
        sort17branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, std::less<int>());
    else if constexpr (N == 18)
        sort18branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, std::less<int>());
    else if constexpr (N == 19)
        sort19branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, std::less<int>());
    else if constexpr (N == 20)
        sort20branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, std::less<int>());
    else if constexpr (N == 21)
        sort21branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, std::less<int>());
    else if constexpr (N == 22)
        sort22branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, std::less<int>());
    else if constexpr (N == 23)
        sort23branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, std::less<int>());
    else if constexpr (N == 24)
        sort24branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, std::less<int>());
    else if constexpr (N == 25)
        sort25branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, std::less<int>());
    else if constexpr (N == 26)
        sort26branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, std::less<int>());
    else if constexpr (N == 27)
        sort27branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, std::less<int>());
    else if constexpr (N == 28)
        sort28branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, std::less<int>());
    else if constexpr (N == 29)
        sort29branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, std::less<int>());
    else if constexpr (N == 30)
        sort30branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, std::less<int>());
    else if constexpr (N == 31)
        sort31branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, std::less<int>());
    else if constexpr (N == 32)
        sort32branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, std::less<int>());
    /*else if constexpr (N == 33)
        sort33branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, std::less<int>());
    else if constexpr (N == 34)
        sort34branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, std::less<int>());
    else if constexpr (N == 35)
        sort35branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, std::less<int>());
    else if constexpr (N == 36)
        sort36branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, std::less<int>());
    else if constexpr (N == 37)
        sort37branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, std::less<int>());
    else if constexpr (N == 38)
        sort38branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, buf+37, std::less<int>());
    else if constexpr (N == 39)
        sort39branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, buf+37, buf+38, std::less<int>());
    else if constexpr (N == 40)
        sort40branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, buf+37, buf+38, buf+39, std::less<int>());
    else if constexpr (N == 41)
        sort41branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, buf+37, buf+38, buf+39, buf+40, std::less<int>());
    else if constexpr (N == 42)
        sort42branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, buf+37, buf+38, buf+39, buf+40, buf+41, std::less<int>());
    else if constexpr (N == 43)
        sort43branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, buf+37, buf+38, buf+39, buf+40, buf+41, buf+42, std::less<int>());
    else if constexpr (N == 44)
        sort44branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, buf+37, buf+38, buf+39, buf+40, buf+41, buf+42, buf+43, std::less<int>());
    else if constexpr (N == 45)
        sort45branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, buf+37, buf+38, buf+39, buf+40, buf+41, buf+42, buf+43, buf+44, std::less<int>());
    else if constexpr (N == 46)
        sort46branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, buf+37, buf+38, buf+39, buf+40, buf+41, buf+42, buf+43, buf+44, buf+45, std::less<int>());
    else if constexpr (N == 47)
        sort47branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, buf+37, buf+38, buf+39, buf+40, buf+41, buf+42, buf+43, buf+44, buf+45, buf+46, std::less<int>());
    else if constexpr (N == 48)
        sort48branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, buf+37, buf+38, buf+39, buf+40, buf+41, buf+42, buf+43, buf+44, buf+45, buf+46, buf+47, std::less<int>());
    else if constexpr (N == 49)
        sort49branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, buf+37, buf+38, buf+39, buf+40, buf+41, buf+42, buf+43, buf+44, buf+45, buf+46, buf+47, buf+48, std::less<int>());
    else if constexpr (N == 50)
        sort50branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, buf+37, buf+38, buf+39, buf+40, buf+41, buf+42, buf+43, buf+44, buf+45, buf+46, buf+47, buf+48, buf+49, std::less<int>());
    else if constexpr (N == 51)
        sort51branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, buf+37, buf+38, buf+39, buf+40, buf+41, buf+42, buf+43, buf+44, buf+45, buf+46, buf+47, buf+48, buf+49, buf+50, std::less<int>());
    else if constexpr (N == 52)
        sort52branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, buf+37, buf+38, buf+39, buf+40, buf+41, buf+42, buf+43, buf+44, buf+45, buf+46, buf+47, buf+48, buf+49, buf+50, buf+51, std::less<int>());
    else if constexpr (N == 53)
        sort53branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, buf+37, buf+38, buf+39, buf+40, buf+41, buf+42, buf+43, buf+44, buf+45, buf+46, buf+47, buf+48, buf+49, buf+50, buf+51, buf+52, std::less<int>());
    else if constexpr (N == 54)
        sort54branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, buf+37, buf+38, buf+39, buf+40, buf+41, buf+42, buf+43, buf+44, buf+45, buf+46, buf+47, buf+48, buf+49, buf+50, buf+51, buf+52, buf+53, std::less<int>());
    else if constexpr (N == 55)
        sort55branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, buf+37, buf+38, buf+39, buf+40, buf+41, buf+42, buf+43, buf+44, buf+45, buf+46, buf+47, buf+48, buf+49, buf+50, buf+51, buf+52, buf+53, buf+54, std::less<int>());
    else if constexpr (N == 56)
        sort56branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, buf+37, buf+38, buf+39, buf+40, buf+41, buf+42, buf+43, buf+44, buf+45, buf+46, buf+47, buf+48, buf+49, buf+50, buf+51, buf+52, buf+53, buf+54, buf+55, std::less<int>());
    else if constexpr (N == 57)
        sort57branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, buf+37, buf+38, buf+39, buf+40, buf+41, buf+42, buf+43, buf+44, buf+45, buf+46, buf+47, buf+48, buf+49, buf+50, buf+51, buf+52, buf+53, buf+54, buf+55, buf+56, std::less<int>());
    else if constexpr (N == 58)
        sort58branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, buf+37, buf+38, buf+39, buf+40, buf+41, buf+42, buf+43, buf+44, buf+45, buf+46, buf+47, buf+48, buf+49, buf+50, buf+51, buf+52, buf+53, buf+54, buf+55, buf+56, buf+57, std::less<int>());
    else if constexpr (N == 59)
        sort59branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, buf+37, buf+38, buf+39, buf+40, buf+41, buf+42, buf+43, buf+44, buf+45, buf+46, buf+47, buf+48, buf+49, buf+50, buf+51, buf+52, buf+53, buf+54, buf+55, buf+56, buf+57, buf+58, std::less<int>());
    else if constexpr (N == 60)
        sort60branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, buf+37, buf+38, buf+39, buf+40, buf+41, buf+42, buf+43, buf+44, buf+45, buf+46, buf+47, buf+48, buf+49, buf+50, buf+51, buf+52, buf+53, buf+54, buf+55, buf+56, buf+57, buf+58, buf+59, std::less<int>());
    else if constexpr (N == 61)
        sort61branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, buf+37, buf+38, buf+39, buf+40, buf+41, buf+42, buf+43, buf+44, buf+45, buf+46, buf+47, buf+48, buf+49, buf+50, buf+51, buf+52, buf+53, buf+54, buf+55, buf+56, buf+57, buf+58, buf+59, buf+60, std::less<int>());
    else if constexpr (N == 62)
        sort62branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, buf+37, buf+38, buf+39, buf+40, buf+41, buf+42, buf+43, buf+44, buf+45, buf+46, buf+47, buf+48, buf+49, buf+50, buf+51, buf+52, buf+53, buf+54, buf+55, buf+56, buf+57, buf+58, buf+59, buf+60, buf+61, std::less<int>());
    else if constexpr (N == 63)
        sort63branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, buf+37, buf+38, buf+39, buf+40, buf+41, buf+42, buf+43, buf+44, buf+45, buf+46, buf+47, buf+48, buf+49, buf+50, buf+51, buf+52, buf+53, buf+54, buf+55, buf+56, buf+57, buf+58, buf+59, buf+60, buf+61, buf+62, std::less<int>());
    else if constexpr (N == 64)
        sort64branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, buf+37, buf+38, buf+39, buf+40, buf+41, buf+42, buf+43, buf+44, buf+45, buf+46, buf+47, buf+48, buf+49, buf+50, buf+51, buf+52, buf+53, buf+54, buf+55, buf+56, buf+57, buf+58, buf+59, buf+60, buf+61, buf+62, buf+63, std::less<int>());
        */
}

// Unico test parametrico per tutti i branchless sort
#define TEST_BRANCHLESS(N) \
TEST(BranchlessSortTest, sort##N##branchless) { \
    test_branchless_sort<N>(call_sort_branchless<N>); \
}

TEST_BRANCHLESS(3)
TEST_BRANCHLESS(4)
TEST_BRANCHLESS(5)
TEST_BRANCHLESS(6)
TEST_BRANCHLESS(7)
TEST_BRANCHLESS(8)
TEST_BRANCHLESS(9)
TEST_BRANCHLESS(10)
TEST_BRANCHLESS(11)
TEST_BRANCHLESS(12)
TEST_BRANCHLESS(13)
TEST_BRANCHLESS(14)
TEST_BRANCHLESS(15)
TEST_BRANCHLESS(16)
TEST_BRANCHLESS(17)
TEST_BRANCHLESS(18)
TEST_BRANCHLESS(19)
TEST_BRANCHLESS(20)
TEST_BRANCHLESS(21)
TEST_BRANCHLESS(22)
TEST_BRANCHLESS(23)
TEST_BRANCHLESS(24)
TEST_BRANCHLESS(25)
TEST_BRANCHLESS(26)
TEST_BRANCHLESS(27)
TEST_BRANCHLESS(28)
TEST_BRANCHLESS(29)
TEST_BRANCHLESS(30)
TEST_BRANCHLESS(31)
TEST_BRANCHLESS(32)
/*TEST_BRANCHLESS(33)
TEST_BRANCHLESS(34)
TEST_BRANCHLESS(35)
TEST_BRANCHLESS(36)
TEST_BRANCHLESS(37)
TEST_BRANCHLESS(38)
TEST_BRANCHLESS(39)
TEST_BRANCHLESS(40)
TEST_BRANCHLESS(41)
TEST_BRANCHLESS(42)
TEST_BRANCHLESS(43)
TEST_BRANCHLESS(44)
TEST_BRANCHLESS(45)
TEST_BRANCHLESS(46)
TEST_BRANCHLESS(47)
TEST_BRANCHLESS(48)
TEST_BRANCHLESS(49)
TEST_BRANCHLESS(50)
TEST_BRANCHLESS(51)
TEST_BRANCHLESS(52)
TEST_BRANCHLESS(53)
TEST_BRANCHLESS(54)
TEST_BRANCHLESS(55)
TEST_BRANCHLESS(56)
TEST_BRANCHLESS(57)
TEST_BRANCHLESS(58)
TEST_BRANCHLESS(59)
TEST_BRANCHLESS(60)
TEST_BRANCHLESS(61)
TEST_BRANCHLESS(62)
TEST_BRANCHLESS(63)
TEST_BRANCHLESS(64)*/
