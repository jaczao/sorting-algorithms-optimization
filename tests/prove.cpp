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

void printArray(const int arr[], int n) {
    for (int i = 0; i < n; i++) {
        cout << arr[i] << " ";
    }
    cout << endl;
}

int main() {
    // Definiamo alcuni vettori di 4 elementi ciascuno.
    int vector1[8] = {3, 1, 4, 2, 1, 5, 9, 6};
    int vector2[8] = {10, 5, 7, 6, 2, 3, 8, 4};
    int vector3[8] = {8, 2, 9, 1, 6, 4, 7, 3};
    int vector4[8] = {5, 3, 8, 2, 1, 4, 7, 6};

    // Array di puntatori ai vettori e nomi per la stampa.
    int* vectors[] = {vector1, vector2, vector3, vector4};
    const char* names[] = {"vector1", "vector2", "vector3", "vector4"};

    // Per ogni vettore, stampiamo l'array prima e dopo l'ordinamento
    for (int i = 0; i < 4; i++) { // Cambiato da 8 a 4 per evitare accessi fuori limite
        cout << names[i] << " prima del sorting: ";
        printArray(vectors[i], 8);

        // Ordiniamo usando la funzione alphadev_sort definita in "alphadev_sorts_arm64.h"
        Sort8AlphaDev(vectors[i]);

        cout << names[i] << " dopo il sorting: ";
        printArray(vectors[i], 8);
        cout << endl;
    }

    return 0;
}