#if defined(__x86_64__) || defined(_M_X64)
#include "../include/alpha_dev_sorts_x86-64.h"
#elif defined(__aarch64__)
#include "../include/alpha_dev_sorts_arm64.h"
#else
#error "Unsupported architecture for AlphaDev sorts"
#endif
#include "branchless_fixed_sorts.h"
#include <gtest/gtest.h>
#include <algorithm>  // std::is_sorted
#include <vector>
#include <random>
#include <functional>
#include <cstdint>
#include <type_traits>

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

/*// Test per tutte le funzioni branchless_fixed_sorts.h (da 3 a 8, versione template)
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
}*/

// Helper per chiamare la funzione template branchless con forwarding degli iteratori
template <typename T, int N>
void call_sort_branchless(T* buf) {
    if constexpr (N == 3)
        sort3branchless<void>(buf, buf+1, buf+2, std::less<T>());
    else if constexpr (N == 4)
        sort4branchless<void>(buf, buf+1, buf+2, buf+3, std::less<T>());
    else if constexpr (N == 5)
        sort5branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, std::less<T>());
    else if constexpr (N == 6)
        sort6branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, std::less<T>());
    else if constexpr (N == 7)
        sort7branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, std::less<T>());
    else if constexpr (N == 8)
        sort8branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, std::less<T>());
    else if constexpr (N == 9)
        sort9branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, std::less<T>());
    else if constexpr (N == 10)
        sort10branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, std::less<T>());
    else if constexpr (N == 11)
        sort11branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, std::less<T>());
    else if constexpr (N == 12)
        sort12branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, std::less<T>());
    else if constexpr (N == 13)
        sort13branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, std::less<T>());
    else if constexpr (N == 14)
        sort14branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, std::less<T>());
    else if constexpr (N == 15)
        sort15branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, std::less<T>());
    else if constexpr (N == 16)
        sort16branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, std::less<T>());
    else if constexpr (N == 17)
        sort17branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, std::less<T>());
    else if constexpr (N == 18)
        sort18branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, std::less<T>());
    else if constexpr (N == 19)
        sort19branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, std::less<T>());
    else if constexpr (N == 20)
        sort20branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, std::less<T>());
    else if constexpr (N == 21)
        sort21branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, std::less<T>());
    else if constexpr (N == 22)
        sort22branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, std::less<T>());
    else if constexpr (N == 23)
        sort23branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, std::less<T>());
    else if constexpr (N == 24)
        sort24branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, std::less<T>());
    else if constexpr (N == 25)
        sort25branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, std::less<T>());
    else if constexpr (N == 26)
        sort26branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, std::less<T>());
    else if constexpr (N == 27)
        sort27branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, std::less<T>());
    else if constexpr (N == 28)
        sort28branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, std::less<T>());
    else if constexpr (N == 29)
        sort29branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, std::less<T>());
    else if constexpr (N == 30)
        sort30branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, std::less<T>());
    else if constexpr (N == 31)
        sort31branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, std::less<T>());
    else if constexpr (N == 32)
        sort32branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, std::less<T>());
    /*else if constexpr (N == 33)
        sort33branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, std::less<T>());
    else if constexpr (N == 34)
        sort34branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, std::less<T>());
    else if constexpr (N == 35)
        sort35branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, std::less<T>());
    else if constexpr (N == 36)
        sort36branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, std::less<T>());
    else if constexpr (N == 37)
        sort37branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, std::less<T>());
    else if constexpr (N == 38)
        sort38branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, buf+37, std::less<T>());
    else if constexpr (N == 39)
        sort39branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, buf+37, buf+38, std::less<T>());
    else if constexpr (N == 40)
        sort40branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, buf+37, buf+38, buf+39, std::less<T>());
    else if constexpr (N == 41)
        sort41branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, buf+37, buf+38, buf+39, buf+40, std::less<T>());
    else if constexpr (N == 42)
        sort42branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, buf+37, buf+38, buf+39, buf+40, buf+41, std::less<T>());
    else if constexpr (N == 43)
        sort43branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, buf+37, buf+38, buf+39, buf+40, buf+41, buf+42, std::less<T>());
    else if constexpr (N == 44)
        sort44branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, buf+37, buf+38, buf+39, buf+40, buf+41, buf+42, buf+43, std::less<T>());
    else if constexpr (N == 45)
        sort45branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, buf+37, buf+38, buf+39, buf+40, buf+41, buf+42, buf+43, buf+44, std::less<T>());
    else if constexpr (N == 46)
        sort46branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, buf+37, buf+38, buf+39, buf+40, buf+41, buf+42, buf+43, buf+44, buf+45, std::less<T>());
    else if constexpr (N == 47)
        sort47branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, buf+37, buf+38, buf+39, buf+40, buf+41, buf+42, buf+43, buf+44, buf+45, buf+46, std::less<T>());
    else if constexpr (N == 48)
        sort48branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, buf+37, buf+38, buf+39, buf+40, buf+41, buf+42, buf+43, buf+44, buf+45, buf+46, buf+47, std::less<T>());
    else if constexpr (N == 49)
        sort49branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, buf+37, buf+38, buf+39, buf+40, buf+41, buf+42, buf+43, buf+44, buf+45, buf+46, buf+47, buf+48, std::less<T>());
    else if constexpr (N == 50)
        sort50branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, buf+37, buf+38, buf+39, buf+40, buf+41, buf+42, buf+43, buf+44, buf+45, buf+46, buf+47, buf+48, buf+49, std::less<T>());
    else if constexpr (N == 51)
        sort51branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, buf+37, buf+38, buf+39, buf+40, buf+41, buf+42, buf+43, buf+44, buf+45, buf+46, buf+47, buf+48, buf+49, buf+50, std::less<T>());
    else if constexpr (N == 52)
        sort52branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, buf+37, buf+38, buf+39, buf+40, buf+41, buf+42, buf+43, buf+44, buf+45, buf+46, buf+47, buf+48, buf+49, buf+50, buf+51, std::less<T>());
    else if constexpr (N == 53)
        sort53branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, buf+37, buf+38, buf+39, buf+40, buf+41, buf+42, buf+43, buf+44, buf+45, buf+46, buf+47, buf+48, buf+49, buf+50, buf+51, buf+52, std::less<T>());
    else if constexpr (N == 54)
        sort54branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, buf+37, buf+38, buf+39, buf+40, buf+41, buf+42, buf+43, buf+44, buf+45, buf+46, buf+47, buf+48, buf+49, buf+50, buf+51, buf+52, buf+53, std::less<T>());
    else if constexpr (N == 55)
        sort55branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, buf+37, buf+38, buf+39, buf+40, buf+41, buf+42, buf+43, buf+44, buf+45, buf+46, buf+47, buf+48, buf+49, buf+50, buf+51, buf+52, buf+53, buf+54, std::less<T>());
    else if constexpr (N == 56)
        sort56branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, buf+37, buf+38, buf+39, buf+40, buf+41, buf+42, buf+43, buf+44, buf+45, buf+46, buf+47, buf+48, buf+49, buf+50, buf+51, buf+52, buf+53, buf+54, buf+55, std::less<T>());
    else if constexpr (N == 57)
        sort57branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, buf+37, buf+38, buf+39, buf+40, buf+41, buf+42, buf+43, buf+44, buf+45, buf+46, buf+47, buf+48, buf+49, buf+50, buf+51, buf+52, buf+53, buf+54, buf+55, buf+56, std::less<T>());
    else if constexpr (N == 58)
        sort58branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, buf+37, buf+38, buf+39, buf+40, buf+41, buf+42, buf+43, buf+44, buf+45, buf+46, buf+47, buf+48, buf+49, buf+50, buf+51, buf+52, buf+53, buf+54, buf+55, buf+56, buf+57, std::less<T>());
    else if constexpr (N == 59)
        sort59branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, buf+37, buf+38, buf+39, buf+40, buf+41, buf+42, buf+43, buf+44, buf+45, buf+46, buf+47, buf+48, buf+49, buf+50, buf+51, buf+52, buf+53, buf+54, buf+55, buf+56, buf+57, buf+58, std::less<T>());
    else if constexpr (N == 60)
        sort60branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, buf+37, buf+38, buf+39, buf+40, buf+41, buf+42, buf+43, buf+44, buf+45, buf+46, buf+47, buf+48, buf+49, buf+50, buf+51, buf+52, buf+53, buf+54, buf+55, buf+56, buf+57, buf+58, buf+59, std::less<T>());
    else if constexpr (N == 61)
        sort61branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, buf+37, buf+38, buf+39, buf+40, buf+41, buf+42, buf+43, buf+44, buf+45, buf+46, buf+47, buf+48, buf+49, buf+50, buf+51, buf+52, buf+53, buf+54, buf+55, buf+56, buf+57, buf+58, buf+59, buf+60, std::less<T>());
    else if constexpr (N == 62)
        sort62branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, buf+37, buf+38, buf+39, buf+40, buf+41, buf+42, buf+43, buf+44, buf+45, buf+46, buf+47, buf+48, buf+49, buf+50, buf+51, buf+52, buf+53, buf+54, buf+55, buf+56, buf+57, buf+58, buf+59, buf+60, buf+61, std::less<T>());
    else if constexpr (N == 63)
        sort63branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, buf+37, buf+38, buf+39, buf+40, buf+41, buf+42, buf+43, buf+44, buf+45, buf+46, buf+47, buf+48, buf+49, buf+50, buf+51, buf+52, buf+53, buf+54, buf+55, buf+56, buf+57, buf+58, buf+59, buf+60, buf+61, buf+62, std::less<T>());*/
    else if constexpr (N == 64)
        sort64branchless<void>(buf, buf+1, buf+2, buf+3, buf+4, buf+5, buf+6, buf+7, buf+8, buf+9, buf+10, buf+11, buf+12, buf+13, buf+14, buf+15, buf+16, buf+17, buf+18, buf+19, buf+20, buf+21, buf+22, buf+23, buf+24, buf+25, buf+26, buf+27, buf+28, buf+29, buf+30, buf+31, buf+32, buf+33, buf+34, buf+35, buf+36, buf+37, buf+38, buf+39, buf+40, buf+41, buf+42, buf+43, buf+44, buf+45, buf+46, buf+47, buf+48, buf+49, buf+50, buf+51, buf+52, buf+53, buf+54, buf+55, buf+56, buf+57, buf+58, buf+59, buf+60, buf+61, buf+62, buf+63, std::less<T>());
}

// Test parametrico branchless per tipo e N
template <typename T, int N>
void test_branchless_sort() {
    std::mt19937 gen(std::random_device{}());
    if constexpr (std::is_same<T, float>::value) {
        std::uniform_real_distribution<float> dist(-1000.0f, 1000.0f);
        for (int t = 0; t < 10000; ++t) {
            float buf[N];
            for (int i = 0; i < N; ++i) buf[i] = dist(gen);
            call_sort_branchless<float, N>(buf);
            EXPECT_TRUE(std::is_sorted(buf, buf + N));
        }
    } else if constexpr (std::is_same<T, int64_t>::value) {
        std::uniform_int_distribution<int64_t> dist(-1000, 1000);
        for (int t = 0; t < 10000; ++t) {
            int64_t buf[N];
            for (int i = 0; i < N; ++i) buf[i] = dist(gen);
            call_sort_branchless<int64_t, N>(buf);
            EXPECT_TRUE(std::is_sorted(buf, buf + N));
        }
    } else {
        std::uniform_int_distribution<int32_t> dist(-1000, 1000);
        for (int t = 0; t < 10000; ++t) {
            int32_t buf[N];
            for (int i = 0; i < N; ++i) buf[i] = dist(gen);
            call_sort_branchless<int32_t, N>(buf);
            EXPECT_TRUE(std::is_sorted(buf, buf + N));
        }
    }
}

// Macro per generare i test branchless per tipo e N
#define TEST_BRANCHLESS_TYPED(N, T, TNAME) \
    TEST(BranchlessSortTest, sort##N##branchless_##TNAME) { \
        test_branchless_sort<T, N>(); \
    }

// Espandi manualmente il ciclo per evitare macro-forbidden constructs
// N.B.: N da 33 a 63 sono esclusi come richiesto
TEST_BRANCHLESS_TYPED(3, int32_t, int32)
TEST_BRANCHLESS_TYPED(3, int64_t, int64)
TEST_BRANCHLESS_TYPED(3, float, float)
TEST_BRANCHLESS_TYPED(4, int32_t, int32)
TEST_BRANCHLESS_TYPED(4, int64_t, int64)
TEST_BRANCHLESS_TYPED(4, float, float)
TEST_BRANCHLESS_TYPED(5, int32_t, int32)
TEST_BRANCHLESS_TYPED(5, int64_t, int64)
TEST_BRANCHLESS_TYPED(5, float, float)
TEST_BRANCHLESS_TYPED(6, int32_t, int32)
TEST_BRANCHLESS_TYPED(6, int64_t, int64)
TEST_BRANCHLESS_TYPED(6, float, float)
TEST_BRANCHLESS_TYPED(7, int32_t, int32)
TEST_BRANCHLESS_TYPED(7, int64_t, int64)
TEST_BRANCHLESS_TYPED(7, float, float)
TEST_BRANCHLESS_TYPED(8, int32_t, int32)
TEST_BRANCHLESS_TYPED(8, int64_t, int64)
TEST_BRANCHLESS_TYPED(8, float, float)
TEST_BRANCHLESS_TYPED(9, int32_t, int32)
TEST_BRANCHLESS_TYPED(9, int64_t, int64)
TEST_BRANCHLESS_TYPED(9, float, float)
TEST_BRANCHLESS_TYPED(10, int32_t, int32)
TEST_BRANCHLESS_TYPED(10, int64_t, int64)
TEST_BRANCHLESS_TYPED(10, float, float)
TEST_BRANCHLESS_TYPED(11, int32_t, int32)
TEST_BRANCHLESS_TYPED(11, int64_t, int64)
TEST_BRANCHLESS_TYPED(11, float, float)
TEST_BRANCHLESS_TYPED(12, int32_t, int32)
TEST_BRANCHLESS_TYPED(12, int64_t, int64)
TEST_BRANCHLESS_TYPED(12, float, float)
TEST_BRANCHLESS_TYPED(13, int32_t, int32)
TEST_BRANCHLESS_TYPED(13, int64_t, int64)
TEST_BRANCHLESS_TYPED(13, float, float)
TEST_BRANCHLESS_TYPED(14, int32_t, int32)
TEST_BRANCHLESS_TYPED(14, int64_t, int64)
TEST_BRANCHLESS_TYPED(14, float, float)
TEST_BRANCHLESS_TYPED(15, int32_t, int32)
TEST_BRANCHLESS_TYPED(15, int64_t, int64)
TEST_BRANCHLESS_TYPED(15, float, float)
TEST_BRANCHLESS_TYPED(16, int32_t, int32)
TEST_BRANCHLESS_TYPED(16, int64_t, int64)
TEST_BRANCHLESS_TYPED(16, float, float)
TEST_BRANCHLESS_TYPED(17, int32_t, int32)
TEST_BRANCHLESS_TYPED(17, int64_t, int64)
TEST_BRANCHLESS_TYPED(17, float, float)
TEST_BRANCHLESS_TYPED(18, int32_t, int32)
TEST_BRANCHLESS_TYPED(18, int64_t, int64)
TEST_BRANCHLESS_TYPED(18, float, float)
TEST_BRANCHLESS_TYPED(19, int32_t, int32)
TEST_BRANCHLESS_TYPED(19, int64_t, int64)
TEST_BRANCHLESS_TYPED(19, float, float)
TEST_BRANCHLESS_TYPED(20, int32_t, int32)
TEST_BRANCHLESS_TYPED(20, int64_t, int64)
TEST_BRANCHLESS_TYPED(20, float, float)
TEST_BRANCHLESS_TYPED(21, int32_t, int32)
TEST_BRANCHLESS_TYPED(21, int64_t, int64)
TEST_BRANCHLESS_TYPED(21, float, float)
TEST_BRANCHLESS_TYPED(22, int32_t, int32)
TEST_BRANCHLESS_TYPED(22, int64_t, int64)
TEST_BRANCHLESS_TYPED(22, float, float)
TEST_BRANCHLESS_TYPED(23, int32_t, int32)
TEST_BRANCHLESS_TYPED(23, int64_t, int64)
TEST_BRANCHLESS_TYPED(23, float, float)
TEST_BRANCHLESS_TYPED(24, int32_t, int32)
TEST_BRANCHLESS_TYPED(24, int64_t, int64)
TEST_BRANCHLESS_TYPED(24, float, float)
TEST_BRANCHLESS_TYPED(25, int32_t, int32)
TEST_BRANCHLESS_TYPED(25, int64_t, int64)
TEST_BRANCHLESS_TYPED(25, float, float)
TEST_BRANCHLESS_TYPED(26, int32_t, int32)
TEST_BRANCHLESS_TYPED(26, int64_t, int64)
TEST_BRANCHLESS_TYPED(26, float, float)
TEST_BRANCHLESS_TYPED(27, int32_t, int32)
TEST_BRANCHLESS_TYPED(27, int64_t, int64)
TEST_BRANCHLESS_TYPED(27, float, float)
TEST_BRANCHLESS_TYPED(28, int32_t, int32)
TEST_BRANCHLESS_TYPED(28, int64_t, int64)
TEST_BRANCHLESS_TYPED(28, float, float)
TEST_BRANCHLESS_TYPED(29, int32_t, int32)
TEST_BRANCHLESS_TYPED(29, int64_t, int64)
TEST_BRANCHLESS_TYPED(29, float, float)
TEST_BRANCHLESS_TYPED(30, int32_t, int32)
TEST_BRANCHLESS_TYPED(30, int64_t, int64)
TEST_BRANCHLESS_TYPED(30, float, float)
TEST_BRANCHLESS_TYPED(31, int32_t, int32)
TEST_BRANCHLESS_TYPED(31, int64_t, int64)
TEST_BRANCHLESS_TYPED(31, float, float)
TEST_BRANCHLESS_TYPED(32, int32_t, int32)
TEST_BRANCHLESS_TYPED(32, int64_t, int64)
TEST_BRANCHLESS_TYPED(32, float, float)
/*TEST_BRANCHLESS_TYPED(33, int32_t, int32)
TEST_BRANCHLESS_TYPED(33, int64_t, int64)
TEST_BRANCHLESS_TYPED(33, float, float)
TEST_BRANCHLESS_TYPED(34, int32_t, int32)
TEST_BRANCHLESS_TYPED(34, int64_t, int64)
TEST_BRANCHLESS_TYPED(34, float, float)
TEST_BRANCHLESS_TYPED(35, int32_t, int32)
TEST_BRANCHLESS_TYPED(35, int64_t, int64)
TEST_BRANCHLESS_TYPED(35, float, float)
TEST_BRANCHLESS_TYPED(36, int32_t, int32)
TEST_BRANCHLESS_TYPED(36, int64_t, int64)
TEST_BRANCHLESS_TYPED(36, float, float)
TEST_BRANCHLESS_TYPED(37, int32_t, int32)
TEST_BRANCHLESS_TYPED(37, int64_t, int64)
TEST_BRANCHLESS_TYPED(37, float, float)
TEST_BRANCHLESS_TYPED(38, int32_t, int32)
TEST_BRANCHLESS_TYPED(38, int64_t, int64)
TEST_BRANCHLESS_TYPED(38, float, float)
TEST_BRANCHLESS_TYPED(39, int32_t, int32)
TEST_BRANCHLESS_TYPED(39, int64_t, int64)
TEST_BRANCHLESS_TYPED(39, float, float)
TEST_BRANCHLESS_TYPED(40, int32_t, int32)
TEST_BRANCHLESS_TYPED(40, int64_t, int64)
TEST_BRANCHLESS_TYPED(40, float, float)
TEST_BRANCHLESS_TYPED(41, int32_t, int32)
TEST_BRANCHLESS_TYPED(41, int64_t, int64)
TEST_BRANCHLESS_TYPED(41, float, float)
TEST_BRANCHLESS_TYPED(42, int32_t, int32)
TEST_BRANCHLESS_TYPED(42, int64_t, int64)
TEST_BRANCHLESS_TYPED(42, float, float)
TEST_BRANCHLESS_TYPED(43, int32_t, int32)
TEST_BRANCHLESS_TYPED(43, int64_t, int64)
TEST_BRANCHLESS_TYPED(43, float, float)
TEST_BRANCHLESS_TYPED(44, int32_t, int32)
TEST_BRANCHLESS_TYPED(44, int64_t, int64)
TEST_BRANCHLESS_TYPED(44, float, float)
TEST_BRANCHLESS_TYPED(45, int32_t, int32)
TEST_BRANCHLESS_TYPED(45, int64_t, int64)
TEST_BRANCHLESS_TYPED(45, float, float)
TEST_BRANCHLESS_TYPED(46, int32_t, int32)
TEST_BRANCHLESS_TYPED(46, int64_t, int64)
TEST_BRANCHLESS_TYPED(46, float, float)
TEST_BRANCHLESS_TYPED(47, int32_t, int32)
TEST_BRANCHLESS_TYPED(47, int64_t, int64)
TEST_BRANCHLESS_TYPED(47, float, float)
TEST_BRANCHLESS_TYPED(48, int32_t, int32)
TEST_BRANCHLESS_TYPED(48, int64_t, int64)
TEST_BRANCHLESS_TYPED(48, float, float)
TEST_BRANCHLESS_TYPED(49, int32_t, int32)
TEST_BRANCHLESS_TYPED(49, int64_t, int64)
TEST_BRANCHLESS_TYPED(49, float, float)
TEST_BRANCHLESS_TYPED(50, int32_t, int32)
TEST_BRANCHLESS_TYPED(50, int64_t, int64)
TEST_BRANCHLESS_TYPED(50, float, float)
TEST_BRANCHLESS_TYPED(51, int32_t, int32)
TEST_BRANCHLESS_TYPED(51, int64_t, int64)
TEST_BRANCHLESS_TYPED(51, float, float)
TEST_BRANCHLESS_TYPED(52, int32_t, int32)
TEST_BRANCHLESS_TYPED(52, int64_t, int64)
TEST_BRANCHLESS_TYPED(52, float, float)
TEST_BRANCHLESS_TYPED(53, int32_t, int32)
TEST_BRANCHLESS_TYPED(53, int64_t, int64)
TEST_BRANCHLESS_TYPED(53, float, float)
TEST_BRANCHLESS_TYPED(54, int32_t, int32)
TEST_BRANCHLESS_TYPED(54, int64_t, int64)
TEST_BRANCHLESS_TYPED(54, float, float)
TEST_BRANCHLESS_TYPED(55, int32_t, int32)
TEST_BRANCHLESS_TYPED(55, int64_t, int64)
TEST_BRANCHLESS_TYPED(55, float, float)
TEST_BRANCHLESS_TYPED(56, int32_t, int32)
TEST_BRANCHLESS_TYPED(56, int64_t, int64)
TEST_BRANCHLESS_TYPED(56, float, float)
TEST_BRANCHLESS_TYPED(57, int32_t, int32)
TEST_BRANCHLESS_TYPED(57, int64_t, int64)
TEST_BRANCHLESS_TYPED(57, float, float)
TEST_BRANCHLESS_TYPED(58, int32_t, int32)
TEST_BRANCHLESS_TYPED(58, int64_t, int64)
TEST_BRANCHLESS_TYPED(58, float, float)
TEST_BRANCHLESS_TYPED(59, int32_t, int32)
TEST_BRANCHLESS_TYPED(59, int64_t, int64)
TEST_BRANCHLESS_TYPED(59, float, float)*/
TEST_BRANCHLESS_TYPED(64, int32_t, int32)
TEST_BRANCHLESS_TYPED(64, int64_t, int64)
TEST_BRANCHLESS_TYPED(64, float, float)

