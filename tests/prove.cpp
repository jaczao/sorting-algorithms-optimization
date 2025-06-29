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




int main() {

    int N = 1000; // Dimensione dell'array

    using T = int;

    vector<T> buf(N);
    mt19937_64 rng(54321);

    uniform_int_distribution<T> dist(numeric_limits<T>::min(), numeric_limits<T>::max());
    for (auto& x : buf) x = dist(rng);
                
    std::iota(buf.begin(), buf.end(), T(0));
    int K = std::max(1, int(std::log2(N)));
    uniform_int_distribution<size_t> dist(0, N-1);
    for (int i = 0; i < K; ++i) {
        size_t a = dist(rng), b = dist(rng); //ok o mi voglio assicurare che sia k coppie diverse?
        std::swap(buf[a], buf[b]);
    }

    

    return 0;
}