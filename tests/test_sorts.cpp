#include <gtest/gtest.h>
#include <vector>
#include <algorithm>  // std::is_sorted
#include <random>     // std::mt19937_64, std::uniform_int_distribution, std::uniform_real_distribution
#include <cstdint>
#include "sorts.h"

using std::vector;

// Template fixture per array di prova
template <typename T>
class DivideEtImperaTestTyped : public ::testing::Test {
protected:
    void check_sorted(const vector<T>& v, int maxSmall, SmallSortStrategy strat) {
        vector<T> copy = v;
        mergeSort(copy.data(), 0, static_cast<int>(copy.size()) - 1, maxSmall, strat);
        EXPECT_TRUE(std::is_sorted(copy.begin(), copy.end()));
        copy = v;
        quickSort(copy.data(), 0, static_cast<int>(copy.size()) - 1, maxSmall, strat);
        EXPECT_TRUE(std::is_sorted(copy.begin(), copy.end()));
        copy = v;
        introSort(copy.data(), 0, static_cast<int>(copy.size()) - 1, maxSmall, strat);
        EXPECT_TRUE(std::is_sorted(copy.begin(), copy.end()));
        copy = v;
        timSort(copy.data(), static_cast<int>(copy.size()), maxSmall, strat);
        EXPECT_TRUE(std::is_sorted(copy.begin(), copy.end()));
        copy = v;
        newSort(copy.data(), 0, static_cast<int>(copy.size()) - 1, 32, 1, strat, SmallSortStrategy::Heap);
        EXPECT_TRUE(std::is_sorted(copy.begin(), copy.end()));
        copy = v;
        newSort(copy.data(), 0, static_cast<int>(copy.size()) - 1, 32, 1024, strat, SmallSortStrategy::Heap);
        EXPECT_TRUE(std::is_sorted(copy.begin(), copy.end()));
    }
};

using TestTypes = ::testing::Types<int32_t, int64_t, float>;
TYPED_TEST_SUITE(DivideEtImperaTestTyped, TestTypes);

TYPED_TEST(DivideEtImperaTestTyped, SmallArrays) {
    using T = TypeParam;
    vector<vector<T>> cases = {
        {},
        {T(1)},
        {T(2), T(1)},
        {T(3), T(2), T(1)},
        {T(5), T(4), T(3), T(2), T(1)}
    };
    for (const auto& v : cases) {
        this->check_sorted(v, 1, SmallSortStrategy::AlphaDev);
    }
}

TYPED_TEST(DivideEtImperaTestTyped, RandomArrays) {
    using T = TypeParam;
    std::mt19937_64 rng(42);
    for (int n : {10, 100, 1000, 10000}) {
        vector<T> v(n);
        if constexpr (std::is_floating_point<T>::value) {
            std::uniform_real_distribution<T> dist(-1000.0, 1000.0);
            for (auto& x : v) x = dist(rng);
        } else {
            std::uniform_int_distribution<int64_t> dist(-1000, 1000);
            for (auto& x : v) x = static_cast<T>(dist(rng));
        }
        this->check_sorted(v, 1, SmallSortStrategy::AlphaDev);
    }
}

TYPED_TEST(DivideEtImperaTestTyped, InsertionSortFunction) {
    using T = TypeParam;
    vector<vector<T>> cases = {
        {}, {T(1)}, {T(2), T(1)}, {T(3), T(2), T(1)}, {T(5), T(4), T(3), T(2), T(1)}, {T(4), T(2), T(5), T(3), T(1)}
    };
    for (auto v : cases) {
        insertionSort(v.data(), static_cast<int>(v.size()));
        EXPECT_TRUE(std::is_sorted(v.begin(), v.end()));
    }
}

TYPED_TEST(DivideEtImperaTestTyped, BubbleSortFunction) {
    using T = TypeParam;
    vector<vector<T>> cases = {
        {}, {T(1)}, {T(2), T(1)}, {T(3), T(2), T(1)}, {T(5), T(4), T(3), T(2), T(1)}, {T(7), T(3), T(9), T(1), T(4)}
    };
    for (auto v : cases) {
        bubbleSort(v.data(), static_cast<int>(v.size()));
        EXPECT_TRUE(std::is_sorted(v.begin(), v.end()));
    }
}

TYPED_TEST(DivideEtImperaTestTyped, SelectionSortFunction) {
    using T = TypeParam;
    vector<vector<T>> cases = {
        {}, {T(1)}, {T(2), T(1)}, {T(3), T(2), T(1)}, {T(5), T(4), T(3), T(2), T(1)}, {T(8), T(6), T(7), T(5), T(3), T(0), T(9)}
    };
    for (auto v : cases) {
        selectionSort(v.data(), static_cast<int>(v.size()));
        EXPECT_TRUE(std::is_sorted(v.begin(), v.end()));
    }
}

TYPED_TEST(DivideEtImperaTestTyped, HeapSortFunction) {
    using T = TypeParam;
    vector<vector<T>> cases = {
        {}, {T(1)}, {T(2), T(1)}, {T(3), T(2), T(1)}, {T(10), T(7), T(5), T(3), T(2), T(1)}, {T(12), T(4), T(8), T(3), T(9)}
    };
    for (auto v : cases) {
        heapSort(v.data(), static_cast<int>(v.size()));
        EXPECT_TRUE(std::is_sorted(v.begin(), v.end()));
    }
}

TYPED_TEST(DivideEtImperaTestTyped, IntroSortTest) {
    using T = TypeParam;
    vector<vector<T>> cases = {
        {}, {T(1)}, {T(2), T(1)}, {T(3), T(2), T(1)}, {T(5), T(4), T(3), T(2), T(1)}, {T(8), T(3), T(7), T(4), T(6), T(5), T(2), T(1), T(9)}
    };
    for (auto v : cases) {
        if (!v.empty())
            introSort(v.data(), 0, static_cast<int>(v.size()) - 1, 1, SmallSortStrategy::AlphaDev);
        EXPECT_TRUE(std::is_sorted(v.begin(), v.end()));
    }
}

TYPED_TEST(DivideEtImperaTestTyped, TimSortFunction) {
    using T = TypeParam;
    vector<vector<T>> cases = {
        {}, {T(1)}, {T(2), T(1)}, {T(3), T(2), T(1)}, {T(5), T(4), T(3), T(2), T(1)}, {T(7), T(3), T(9), T(1), T(4)}
    };
    for (auto v : cases) {
        timSort(v.data(), static_cast<int>(v.size()), 32, SmallSortStrategy::AlphaDev);
        EXPECT_TRUE(std::is_sorted(v.begin(), v.end()));
    }
}

TYPED_TEST(DivideEtImperaTestTyped, NewSortFunction) {
    using T = TypeParam;
    vector<vector<T>> cases = {
        {}, {T(1)}, {T(2), T(1)}, {T(3), T(2), T(1)}, {T(5), T(4), T(3), T(2), T(1)}, {T(8), T(3), T(7), T(4), T(6), T(5), T(2), T(1), T(9)}
    };
    for (auto v : cases) {
        if (!v.empty())
            newSort(v.data(), 0, static_cast<int>(v.size()) - 1, 4, 4, SmallSortStrategy::AlphaDev, SmallSortStrategy::Insertion);
        EXPECT_TRUE(std::is_sorted(v.begin(), v.end()));
    }
}

