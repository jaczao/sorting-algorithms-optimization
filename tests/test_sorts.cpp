#include <gtest/gtest.h>
#include <vector>
#include <algorithm>  // std::is_sorted
#include <random>     // std::mt19937_64, std::uniform_int_distribution
#include "sorts.h"

using std::vector;

// Fixture per array di prova
class DivideEtImperaTest : public ::testing::Test {
protected:
    void check_sorted(const vector<int>& v, int maxSmall, SmallSortStrategy strat) {
        vector<int> copy = v;
        mergeSort(copy.data(), 0, static_cast<int>(copy.size()) - 1, maxSmall, strat);
        EXPECT_TRUE(std::is_sorted(copy.begin(), copy.end()));
        copy = v;
        quickSort(copy.data(), 0, static_cast<int>(copy.size()) - 1, maxSmall, strat);
        EXPECT_TRUE(std::is_sorted(copy.begin(), copy.end()));
        copy = v;
        introSort(copy.data(), 0, static_cast<int>(copy.size()) - 1, maxSmall, strat);
        EXPECT_TRUE(std::is_sorted(copy.begin(), copy.end()));
        copy = v;
        timSort(copy.data(), static_cast<int>(copy.size()));
        EXPECT_TRUE(std::is_sorted(copy.begin(), copy.end()));
        copy = v;
        newSort(copy.data(), 0, static_cast<int>(copy.size()) - 1, 32, 1, strat, SmallSortStrategy::Heap);
        EXPECT_TRUE(std::is_sorted(copy.begin(), copy.end()));
        copy = v;
        newSort(copy.data(), 0, static_cast<int>(copy.size()) - 1, 32, 1024, strat, SmallSortStrategy::Heap);
        EXPECT_TRUE(std::is_sorted(copy.begin(), copy.end()));
    }
};

TEST_F(DivideEtImperaTest, SmallArrays) {
    vector<vector<int>> cases = {{}, {1}, {2,1}, {3,2,1}, {5,4,3,2,1}};
    for (const auto& v : cases) {
        check_sorted(v, 1, SmallSortStrategy::AlphaDev);
    }
}

TEST_F(DivideEtImperaTest, RandomArrays) {
    std::mt19937_64 rng(42);
    std::uniform_int_distribution<int> dist(-1000, 1000);
    for (int n : {10, 100, 1000, 10000}) {
        vector<int> v(n);
        for (auto& x : v) x = dist(rng);
        check_sorted(v, 1, SmallSortStrategy::AlphaDev);
    }
}

TEST_F(DivideEtImperaTest, InsertionSortFunction) {
    vector<vector<int>> cases = {
        {}, {1}, {2, 1}, {3, 2, 1}, {5, 4, 3, 2, 1}, {4, 2, 5, 3, 1}
    };
    for (auto v : cases) {
        insertionSort(v.data(), static_cast<int>(v.size()));
        EXPECT_TRUE(std::is_sorted(v.begin(), v.end()));
    }
}

TEST_F(DivideEtImperaTest, BubbleSortFunction) {
    vector<vector<int>> cases = {
        {}, {1}, {2, 1}, {3, 2, 1}, {5, 4, 3, 2, 1}, {7, 3, 9, 1, 4}
    };
    for (auto v : cases) {
        bubbleSort(v.data(), static_cast<int>(v.size()));
        EXPECT_TRUE(std::is_sorted(v.begin(), v.end()));
    }
}

TEST_F(DivideEtImperaTest, SelectionSortFunction) {
    vector<vector<int>> cases = {
        {}, {1}, {2, 1}, {3, 2, 1}, {5, 4, 3, 2, 1}, {8, 6, 7, 5, 3, 0, 9}
    };
    for (auto v : cases) {
        selectionSort(v.data(), static_cast<int>(v.size()));
        EXPECT_TRUE(std::is_sorted(v.begin(), v.end()));
    }
}

TEST_F(DivideEtImperaTest, HeapSortFunction) {
    vector<vector<int>> cases = {
        {}, {1}, {2, 1}, {3, 2, 1}, {10, 7, 5, 3, 2, 1}, {12, 4, 8, 3, 9}
    };
    for (auto v : cases) {
        heapSort(v.data(), static_cast<int>(v.size()));
        EXPECT_TRUE(std::is_sorted(v.begin(), v.end()));
    }
}

TEST_F(DivideEtImperaTest, IntroSortTest) {
    vector<vector<int>> cases = {
        {}, {1}, {2, 1}, {3, 2, 1}, {5, 4, 3, 2, 1}, {8, 3, 7, 4, 6, 5, 2, 1, 9}
    };
    for (auto v : cases) {
        if (!v.empty())
            introSort(v.data(), 0, static_cast<int>(v.size()) - 1, 1, SmallSortStrategy::AlphaDev);
        EXPECT_TRUE(std::is_sorted(v.begin(), v.end()));
    }
}

TEST_F(DivideEtImperaTest, TimSortFunction) {
    vector<vector<int>> cases = {
        {}, {1}, {2, 1}, {3, 2, 1}, {5, 4, 3, 2, 1}, {7, 3, 9, 1, 4}
    };
    for (auto v : cases) {
        timSort(v.data(), static_cast<int>(v.size()));
        EXPECT_TRUE(std::is_sorted(v.begin(), v.end()));
    }
}

TEST_F(DivideEtImperaTest, NewSortFunction) {
    vector<vector<int>> cases = {
        {}, {1}, {2, 1}, {3, 2, 1}, {5, 4, 3, 2, 1}, {8, 3, 7, 4, 6, 5, 2, 1, 9}
    };
    for (auto v : cases) {
        if (!v.empty())
            newSort(v.data(), 0, static_cast<int>(v.size()) - 1, 4, 4, SmallSortStrategy::AlphaDev, SmallSortStrategy::Insertion);
        EXPECT_TRUE(std::is_sorted(v.begin(), v.end()));
    }
}

