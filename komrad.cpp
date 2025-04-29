/*
*
* su x86-64, con compilatore clang nuovo compila codice come dovrebbe essere secondo komrad (cioe che rispecchia sorting networks)
*
* -std=c++14 -O3
* 
*/

#include <cstdint>
#include <immintrin.h>
/*
void sort2(int* __restrict v) {
	const int a = v[0];
	const int b = v[1];
	v[0] = b > a ? a : b;
	v[1] = b > a ? b : a;
}
*/
void sort6(int* __restrict v) {
#define __kmin(a, b) (a < b ? a : b)
#define __kmax(a, b) (a < b ? b : a)
#define __kswap(x,y) { int a = __kmin(v[x], v[y]); int b = __kmax(v[x], v[y]); v[x] = a; v[y] = b; }
	__kswap(1, 2);
	__kswap(0, 2);
	__kswap(0, 1);
	__kswap(4, 5);
	__kswap(3, 5);
	__kswap(3, 4);
	__kswap(0, 3);
	__kswap(1, 4);
	__kswap(2, 5);
	__kswap(2, 4);
	__kswap(1, 3);
	__kswap(2, 3);
#undef __kswap
#undef __kmax
#undef __kmin
}

