#include <iostream>
using namespace std;

// Includiamo il file che contiene la funzione di sorting "alphadev_sorts_arm64".
// Assumiamo che questo file definisca la funzione seguente:
//   void alphadev_sort(int arr[], int size);
#if defined(__x86_64__) || defined(_M_X64)
#include "../include/alpha_dev_sorts_x86-64.h"
#elif defined(__aarch64__)
#include "../include/alpha_dev_sorts_arm64.h"
#else
#error "Unsupported architecture for AlphaDev sorts"
#endif
#include <ciso646>
#include <random>
#include <algorithm>
#include <numeric>
#include <limits>
#include <vector>
#include <chrono>
#include <unordered_set>




int main() {

    int N = 10; // Dimensione dell'array
    using T = float;

    vector<T> buf(N);
    mt19937_64 rng(std::chrono::steady_clock::now().time_since_epoch().count());


    uniform_real_distribution<T> dist(0, numeric_limits<T>::max());
    uniform_int_distribution<int> sign_dist(0, 1); // 0 or 1
    for (auto& x : buf) {
        int sign = sign_dist(rng) == 0 ? -1 : 1;
        x = dist(rng) * sign;
    }

    cout << "Buffer: ";
    for (const auto& val : buf) {
        cout << val << " ";
    }
    cout << endl;



    return 0;
}
