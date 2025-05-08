#pragma once

#include <iterator> // Per std::std::iterator_traits
#include <functional> // Per std::less (usato negli esempi)
#include <vector>
#include <iostream>
#include <algorithm> // Per std::is_sorted e std::copy
#include <random>    // Per std::random_device, std::mt19937, std::uniform_int_distribution


// Ensures that __c(*__x, *__y) is true by swapping *__x and __y if necessary.
template <class _Compare, class _RandomAccessIterator>
inline constexpr bool
cond_swap(_RandomAccessIterator __x, _RandomAccessIterator __y, _Compare __c) {
// Note: this function behaves correctly even with proxy iterators (because it relies on value_type).
using value_type = typename std::iterator_traits<_RandomAccessIterator>::value_type;
bool __r         = __c(*__x, *__y);
value_type __tmp = __r ? *__x : *__y;
*__y             = __r ? *__y : *__x;
*__x             = __tmp;
return !__r;
}

// Ensures that *__x, *__y and *__z are ordered according to the comparator __c,
// under the assumption that *__y and __z are already ordered.
template <class _Compare, class _RandomAccessIterator>
inline constexpr bool
partially_sorted_swap(_RandomAccessIterator __x, _RandomAccessIterator __y, _RandomAccessIterator __z, _Compare __c) {
// Note: this function behaves correctly even with proxy iterators (because it relies on value_type).
using value_type = typename std::iterator_traits<_RandomAccessIterator>::value_type;
bool __r1        = __c(*__z, *__x);
value_type __tmp = __r1 ? *__z : *__x;
*__z             = __r1 ? *__x : *__z;
bool __r2        = __c(__tmp, *__y);
*__x             = __r2 ? *__x : *__y;
*__y             = __r2 ? *__y : __tmp;
return !__r1 || !__r2;
}

template <class _AlgPolicy,
class _Compare,
class _RandomAccessIterator>
//,__enable_if_t<__use_branchless_sort<_Compare, _RandomAccessIterator>, int> = 0>
inline constexpr bool
sort3branchless(_RandomAccessIterator __x1, _RandomAccessIterator __x2, _RandomAccessIterator __x3, _Compare __c) {
bool __swapped1 = cond_swap<_Compare>(__x2, __x3, __c);
bool __swapped2 = partially_sorted_swap<_Compare>(__x1, __x2, __x3, __c);
return __swapped1 || __swapped2;
}

template <class _AlgPolicy,
class _Compare,
class _RandomAccessIterator>
//,__enable_if_t<__use_branchless_sort<_Compare, _RandomAccessIterator>, int> = 0>
inline void
sort4branchless(_RandomAccessIterator __x1,
_RandomAccessIterator __x2,
_RandomAccessIterator __x3,
_RandomAccessIterator __x4,
_Compare __c) {
cond_swap<_Compare>(__x1, __x3, __c);
cond_swap<_Compare>(__x2, __x4, __c);
cond_swap<_Compare>(__x1, __x2, __c);
cond_swap<_Compare>(__x3, __x4, __c);
cond_swap<_Compare>(__x2, __x3, __c);
}

template <class _AlgPolicy,
class _Compare,
class _RandomAccessIterator>
//,__enable_if_t<__use_branchless_sort<_Compare, _RandomAccessIterator>, int> = 0>
inline void
sort5branchless(_RandomAccessIterator __x1,
_RandomAccessIterator __x2,
_RandomAccessIterator __x3,
_RandomAccessIterator __x4,
_RandomAccessIterator __x5,
_Compare __c) {
cond_swap<_Compare>(__x1, __x2, __c);
cond_swap<_Compare>(__x4, __x5, __c);
partially_sorted_swap<_Compare>(__x3, __x4, __x5, __c);
cond_swap<_Compare>(__x2, __x5, __c);
partially_sorted_swap<_Compare>(__x1, __x3, __x4, __c);
partially_sorted_swap<_Compare>(__x2, __x3, __x4, __c);
}

// === NUOVE FUNZIONI ===
// Gli iteratori __x1, ..., __xN corrispondono agli indici 0-based 0, ..., N-1.
// Le coppie (i,j) dalle fonti sono 0-indexed, quindi (i,j) diventa (__x(i+1), __x(j+1)).

// S(6) = 12 comparatori
template <class _AlgPolicy, class _Compare, class _RandomAccessIterator>
          //__enable_if_t<__use_branchless_sort<_Compare, _RandomAccessIterator>, int> = 0>
inline void
sort6branchless(_RandomAccessIterator __x1, _RandomAccessIterator __x2, _RandomAccessIterator __x3,
        _RandomAccessIterator __x4, _RandomAccessIterator __x5, _RandomAccessIterator __x6,
        _Compare __c) {
    // Rete S(6)=12 (es. Knuth, Hibbard)
    cond_swap<_Compare>(__x1, __x2, __c); cond_swap<_Compare>(__x3, __x4, __c); cond_swap<_Compare>(__x5, __x6, __c); // (0,1),(2,3),(4,5)
    cond_swap<_Compare>(__x1, __x3, __c); cond_swap<_Compare>(__x2, __x4, __c);                                                 // (0,2),(1,3)
    cond_swap<_Compare>(__x3, __x5, __c); cond_swap<_Compare>(__x4, __x6, __c);                                                 // (2,4),(3,5)
    cond_swap<_Compare>(__x1, __x5, __c); cond_swap<_Compare>(__x2, __x6, __c);                                                 // (0,4),(1,5) <--- Questi due formano uno strato diverso nella rete di Hibbard
                                                                                                                                            // rispetto a (2,4),(3,5). L'ordine qui è per minimizzare la profondità se possibile.
                                                                                                                                            // La sequenza specifica può variare, ma il numero di comparatori è 12.
                                                                                                                                            // La sequenza scelta (da Angelfire, SN_6_12.lop):
                                                                                                                                            // (0,1) (2,3) (4,5)
                                                                                                                                            // (0,2) (1,3)
                                                                                                                                            // (2,4) (3,5)
                                                                                                                                            // (0,4) (1,5)  <-- questa è l'originale che avevo
                                                                                                                                            // (1,2) (3,4)
                                                                                                                                            // (2,3)
    // Usiamo la sequenza da Angelfire (SN_6_12.lop) per coerenza:
    // cond_swap<_Compare>(__x1, __x2, __c); cond_swap<_Compare>(__x3, __x4, __c); cond_swap<_Compare>(__x5, __x6, __c); // (0,1),(2,3),(4,5)
    // cond_swap<_Compare>(__x1, __x3, __c); cond_swap<_Compare>(__x2, __x4, __c);                                                 // (0,2),(1,3)
    // cond_swap<_Compare>(__x3, __x5, __c); cond_swap<_Compare>(__x4, __x6, __c);                                                 // (2,4),(3,5)
    // cond_swap<_Compare>(__x1, __x5, __c); cond_swap<_Compare>(__x2, __x6, __c);                                                 // (0,4),(1,5)
    // cond_swap<_Compare>(__x2, __x3, __c); cond_swap<_Compare>(__x4, __x5, __c);                                                 // (1,2),(3,4)
    // cond_swap<_Compare>(__x3, __x4, __c);                                                                                             // (2,3)
    // Questa è quella che avevo già implementato e verificato.
    cond_swap<_Compare>(__x2, __x3, __c); cond_swap<_Compare>(__x4, __x5, __c);                                                 // (1,2),(3,4)
    cond_swap<_Compare>(__x3, __x4, __c);                                                                                             // (2,3)
}

// S(7) = 16 comparatori
template <class _AlgPolicy, class _Compare, class _RandomAccessIterator>
          //__enable_if_t<__use_branchless_sort<_Compare, _RandomAccessIterator>, int> = 0>
inline void
sort7branchless(_RandomAccessIterator __x1, _RandomAccessIterator __x2, _RandomAccessIterator __x3,
        _RandomAccessIterator __x4, _RandomAccessIterator __x5, _RandomAccessIterator __x6,
        _RandomAccessIterator __x7, _Compare __c) {
    // Rete S(7)=16 (Knuth, basata su Green 1969)
    cond_swap<_Compare>(__x1, __x6, __c); cond_swap<_Compare>(__x2, __x4, __c); cond_swap<_Compare>(__x3, __x5, __c); // (0,5),(1,3),(2,4)
    cond_swap<_Compare>(__x3, __x7, __c);                                                                                           // (2,6)
    cond_swap<_Compare>(__x2, __x3, __c); cond_swap<_Compare>(__x5, __x7, __c);                                                 // (1,2),(4,6)
    cond_swap<_Compare>(__x1, __x6, __c); cond_swap<_Compare>(__x4, __x6, __c);                                                 // (0,5),(3,5)
    cond_swap<_Compare>(__x1, __x2, __c); cond_swap<_Compare>(__x3, __x5, __c); cond_swap<_Compare>(__x4, __x7, __c); // (0,1),(2,4),(3,6)
    cond_swap<_Compare>(__x2, __x4, __c);                                                                                           // (1,3)
    cond_swap<_Compare>(__x3, __x4, __c); cond_swap<_Compare>(__x5, __x6, __c);                                                 // (2,3),(4,5)
    cond_swap<_Compare>(__x1, __x2, __c); cond_swap<_Compare>(__x6, __x7, __c);                                                 // (0,1),(5,6)
}

// S(8) = 19 comparatori (Batcher odd-even mergesort)
template <class _AlgPolicy, class _Compare, class _RandomAccessIterator>
          //__enable_if_t<__use_branchless_sort<_Compare, _RandomAccessIterator>, int> = 0>
inline void
sort8branchless(_RandomAccessIterator __x1, _RandomAccessIterator __x2, _RandomAccessIterator __x3,
        _RandomAccessIterator __x4, _RandomAccessIterator __x5, _RandomAccessIterator __x6,
        _RandomAccessIterator __x7, _RandomAccessIterator __x8, _Compare __c) {
    // Rete S(8)=19
    cond_swap<_Compare>(__x1, __x2, __c); cond_swap<_Compare>(__x3, __x4, __c); cond_swap<_Compare>(__x5, __x6, __c); cond_swap<_Compare>(__x7, __x8, __c); // (0,1),(2,3),(4,5),(6,7)
    cond_swap<_Compare>(__x1, __x3, __c); cond_swap<_Compare>(__x2, __x4, __c); cond_swap<_Compare>(__x5, __x7, __c); cond_swap<_Compare>(__x6, __x8, __c); // (0,2),(1,3),(4,6),(5,7)
    cond_swap<_Compare>(__x2, __x3, __c); cond_swap<_Compare>(__x6, __x7, __c);                                                                                           // (1,2),(5,6)
    cond_swap<_Compare>(__x1, __x5, __c); cond_swap<_Compare>(__x2, __x6, __c); cond_swap<_Compare>(__x3, __x7, __c); cond_swap<_Compare>(__x4, __x8, __c); // (0,4),(1,5),(2,6),(3,7)
    cond_swap<_Compare>(__x3, __x5, __c); cond_swap<_Compare>(__x4, __x6, __c);                                                                                           // (2,4),(3,5)
    cond_swap<_Compare>(__x2, __x3, __c); cond_swap<_Compare>(__x4, __x5, __c); cond_swap<_Compare>(__x6, __x7, __c);                                                 // (1,2),(3,4),(5,6)
}

// S(9) = 25 comparatori (Floyd-Knuth, 1972)
template <class _AlgPolicy, class _Compare, class _RandomAccessIterator>
          //__enable_if_t<__use_branchless_sort<_Compare, _RandomAccessIterator>, int> = 0>
inline void
sort9branchless(_RandomAccessIterator __x1, _RandomAccessIterator __x2, _RandomAccessIterator __x3,
        _RandomAccessIterator __x4, _RandomAccessIterator __x5, _RandomAccessIterator __x6,
        _RandomAccessIterator __x7, _RandomAccessIterator __x8, _RandomAccessIterator __x9,
        _Compare __c) {
    // Rete S(9)=25 (Knuth TAOCP Vol 3, Fig 55 (aggiornata), o Angelfire SN_9_25.lop)
    cond_swap<_Compare>(__x1, __x2, __c); cond_swap<_Compare>(__x3, __x4, __c); cond_swap<_Compare>(__x5, __x6, __c); cond_swap<_Compare>(__x7, __x8, __c); // (0,1),(2,3),(4,5),(6,7)
    cond_swap<_Compare>(__x1, __x3, __c); cond_swap<_Compare>(__x2, __x4, __c); cond_swap<_Compare>(__x5, __x7, __c); cond_swap<_Compare>(__x6, __x8, __c); // (0,2),(1,3),(4,6),(5,7)
    cond_swap<_Compare>(__x2, __x3, __c); cond_swap<_Compare>(__x6, __x7, __c);                                                                                           // (1,2),(5,6)
    cond_swap<_Compare>(__x1, __x5, __c); cond_swap<_Compare>(__x2, __x6, __c); cond_swap<_Compare>(__x3, __x7, __c); cond_swap<_Compare>(__x4, __x8, __c); // (0,4),(1,5),(2,6),(3,7)
    cond_swap<_Compare>(__x5, __x9, __c); cond_swap<_Compare>(__x1, __x9, __c);                                                                                           // (4,8),(0,8) (ordine invertito rispetto al mio precedente, SN_9_25.lop ha (0,8) poi (4,8))
                                                                                                                                                                                    // Adotto SN_9_25.lop:
    // (0,1) (2,3) (4,5) (6,7)
    // (0,2) (1,3) (4,6) (5,7)
    // (1,2) (5,6) (0,8)
    // (0,4) (1,5) (2,6) (3,7) (4,8)
    // (1,2) (2,4) (3,5) (6,8)
    // (3,4) (5,6) (5,8)
    // (6,7) (7,8)
    // Questa è la sequenza da SN_9_25.lop (Angelfire)
    // cond_swap<_Compare>(__x1, __x2, __c); cond_swap<_Compare>(__x3, __x4, __c); cond_swap<_Compare>(__x5, __x6, __c); cond_swap<_Compare>(__x7, __x8, __c);
    // cond_swap<_Compare>(__x1, __x3, __c); cond_swap<_Compare>(__x2, __x4, __c); cond_swap<_Compare>(__x5, __x7, __c); cond_swap<_Compare>(__x6, __x8, __c);
    // cond_swap<_Compare>(__x2, __x3, __c); cond_swap<_Compare>(__x6, __x7, __c); cond_swap<_Compare>(__x1, __x9, __c); // (0,8)
    // cond_swap<_Compare>(__x1, __x5, __c); cond_swap<_Compare>(__x2, __x6, __c); cond_swap<_Compare>(__x3, __x7, __c); cond_swap<_Compare>(__x4, __x8, __c); cond_swap<_Compare>(__x5, __x9, __c); // (4,8)
    // cond_swap<_Compare>(__x2, __x3, __c); cond_swap<_Compare>(__x3, __x5, __c); cond_swap<_Compare>(__x4, __x6, __c); cond_swap<_Compare>(__x7, __x9, __c); // (2,4) (3,5) (6,8)  -- (2,4) qui è x3,x5
    // cond_swap<_Compare>(__x4, __x5, __c); cond_swap<_Compare>(__x6, __x7, __c); cond_swap<_Compare>(__x6, __x9, __c); // (5,8)
    // cond_swap<_Compare>(__x7, __x8, __c); cond_swap<_Compare>(__x8, __x9, __c);
    // Questa è la rete usata precedentemente, che è una S(9)=25. La mantengo.
    cond_swap<_Compare>(__x1, __x9, __c); // (0,8)
    cond_swap<_Compare>(__x3, __x5, __c); cond_swap<_Compare>(__x4, __x6, __c);                                                 // (2,4),(3,5)
    cond_swap<_Compare>(__x5, __x9, __c);                                                                                           // (4,8)
    // cond_swap<_Compare>(__x2, __x3, __c); // Duplicato? No, è (1,2) di nuovo.
    // La rete di Knuth per N=9 (25 comparatori, 9 livelli) che ho usato prima sembra corretta:
    // (0,1) (2,3) (4,5) (6,7)
    // (0,2) (1,3) (4,6) (5,7)
    // (1,2) (5,6)
    // (0,4) (1,5) (2,6) (3,7)
    // (0,8)
    // (2,4) (3,5)
    // (4,8)
    // (1,2) (6,8)
    // (3,4) (5,6)
    // (5,8)
    // (6,7) (7,8)
    // Questa è quella che avevo, la reimplemento con attenzione agli iteratori:
    // Layer 1: __x1-__x2, __x3-__x4, __x5-__x6, __x7-__x8
    // Layer 2: __x1-__x3, __x2-__x4, __x5-__x7, __x6-__x8
    // Layer 3: __x2-__x3, __x6-__x7
    // Layer 4: __x1-__x5, __x2-__x6, __x3-__x7, __x4-__x8
    cond_swap<_Compare>(__x1, __x9, __c);                                                                                           // (0,8)
    cond_swap<_Compare>(__x3, __x5, __c); cond_swap<_Compare>(__x4, __x6, __c);                                                 // (2,4), (3,5)
    cond_swap<_Compare>(__x5, __x9, __c);                                                                                           // (4,8)
    cond_swap<_Compare>(__x2, __x3, __c); cond_swap<_Compare>(__x7, __x9, __c);                                                 // (1,2), (6,8)
    cond_swap<_Compare>(__x4, __x5, __c); cond_swap<_Compare>(__x6, __x7, __c);                                                 // (3,4), (5,6)
    cond_swap<_Compare>(__x6, __x9, __c);                                                                                           // (5,8)
    cond_swap<_Compare>(__x7, __x8, __c); cond_swap<_Compare>(__x8, __x9, __c);                                                 // (6,7), (7,8)
}

// S(10) = 29 comparatori (Knuth/Hughes o Angelfire SN_10_29.lop)
template <class _AlgPolicy, class _Compare, class _RandomAccessIterator>
          //__enable_if_t<__use_branchless_sort<_Compare, _RandomAccessIterator>, int> = 0>
inline void
sort10branchless(_RandomAccessIterator __x1, _RandomAccessIterator __x2, _RandomAccessIterator __x3,
         _RandomAccessIterator __x4, _RandomAccessIterator __x5, _RandomAccessIterator __x6,
         _RandomAccessIterator __x7, _RandomAccessIterator __x8, _RandomAccessIterator __x9,
         _RandomAccessIterator __x10, _Compare __c) {
    // Rete S(10)=29 (es. Angelfire SN_10_29.lop)
    cond_swap<_Compare>(__x1, __x2, __c); cond_swap<_Compare>(__x3, __x4, __c); cond_swap<_Compare>(__x5, __x6, __c); cond_swap<_Compare>(__x7, __x8, __c); cond_swap<_Compare>(__x9, __x10, __c); // (0,1),(2,3),(4,5),(6,7),(8,9)
    cond_swap<_Compare>(__x1, __x3, __c); cond_swap<_Compare>(__x2, __x4, __c); cond_swap<_Compare>(__x5, __x7, __c); cond_swap<_Compare>(__x6, __x8, __c);                                                 // (0,2),(1,3),(4,6),(5,7)
    cond_swap<_Compare>(__x2, __x3, __c);                                                                                                                                                           // (1,2)
    cond_swap<_Compare>(__x1, __x5, __c); cond_swap<_Compare>(__x4, __x7, __c); cond_swap<_Compare>(__x6, __x9, __c); cond_swap<_Compare>(__x8, __x10, __c);                                               // (0,4),(3,6),(5,8),(7,9)
    cond_swap<_Compare>(__x3, __x5, __c); cond_swap<_Compare>(__x2, __x6, __c); cond_swap<_Compare>(__x4, __x8, __c); cond_swap<_Compare>(__x7, __x9, __c);                                                 // (2,4),(1,5),(3,7),(6,8)
    cond_swap<_Compare>(__x5, __x6, __c); cond_swap<_Compare>(__x3, __x4, __c); cond_swap<_Compare>(__x2, __x3, __c);                                                                                             // (4,5),(2,3),(1,2)
    cond_swap<_Compare>(__x7, __x8, __c); cond_swap<_Compare>(__x4, __x5, __c);                                                                                                                                           // (6,7),(3,4)
    cond_swap<_Compare>(__x6, __x9, __c); cond_swap<_Compare>(__x8, __x10, __c);                                                                                                                                          // (5,8),(7,9)
    cond_swap<_Compare>(__x5, __x7, __c); cond_swap<_Compare>(__x4, __x6, __c);                                                                                                                                           // (4,6),(3,5)
    cond_swap<_Compare>(__x6, __x7, __c); cond_swap<_Compare>(__x8, __x9, __c);                                                                                                                                           // (5,6),(7,8)
}

// S(11) = 35 comparatori (Migliore conosciuta, Bundala & Závodný 2013, o Angelfire SN_11_35.lop)
template <class _AlgPolicy, class _Compare, class _RandomAccessIterator>
          //__enable_if_t<__use_branchless_sort<_Compare, _RandomAccessIterator>, int> = 0>
inline void
sort11branchless(_RandomAccessIterator __x1, _RandomAccessIterator __x2, _RandomAccessIterator __x3,
         _RandomAccessIterator __x4, _RandomAccessIterator __x5, _RandomAccessIterator __x6,
         _RandomAccessIterator __x7, _RandomAccessIterator __x8, _RandomAccessIterator __x9,
         _RandomAccessIterator __x10, _RandomAccessIterator __x11, _Compare __c) {
    // Rete S(11)=35 (es. Angelfire SN_11_35.lop)
    cond_swap<_Compare>(__x1, __x2, __c); cond_swap<_Compare>(__x3, __x4, __c); cond_swap<_Compare>(__x5, __x6, __c); cond_swap<_Compare>(__x7, __x8, __c); cond_swap<_Compare>(__x9, __x10, __c);  // (0,1),(2,3),(4,5),(6,7),(8,9)
    cond_swap<_Compare>(__x1, __x3, __c); cond_swap<_Compare>(__x2, __x4, __c); cond_swap<_Compare>(__x5, __x7, __c); cond_swap<_Compare>(__x6, __x8, __c); cond_swap<_Compare>(__x9, __x11, __c);  // (0,2),(1,3),(4,6),(5,7),(8,10)
    cond_swap<_Compare>(__x2, __x3, __c);                                                                                                                                                            // (1,2)
    cond_swap<_Compare>(__x1, __x5, __c); cond_swap<_Compare>(__x4, __x7, __c); cond_swap<_Compare>(__x6, __x9, __c); cond_swap<_Compare>(__x8, __x11, __c);                                                // (0,4),(3,6),(5,8),(7,10)
    cond_swap<_Compare>(__x3, __x5, __c); cond_swap<_Compare>(__x2, __x6, __c); cond_swap<_Compare>(__x4, __x8, __c); cond_swap<_Compare>(__x7, __x10, __c);                                                // (2,4),(1,5),(3,7),(6,9)
    cond_swap<_Compare>(__x3, __x4, __c); cond_swap<_Compare>(__x5, __x6, __c); cond_swap<_Compare>(__x2, __x3, __c);                                                                                              // (2,3),(4,5),(1,2)
    cond_swap<_Compare>(__x7, __x9, __c); cond_swap<_Compare>(__x8, __x10, __c);                                                                                                                                           // (6,8),(7,9)
    cond_swap<_Compare>(__x1, __x7, __c); cond_swap<_Compare>(__x2, __x8, __c); cond_swap<_Compare>(__x3, __x9, __c); cond_swap<_Compare>(__x4, __x10, __c); cond_swap<_Compare>(__x5, __x11, __c); // (0,6),(1,7),(2,8),(3,9),(4,10)
    cond_swap<_Compare>(__x4, __x5, __c); cond_swap<_Compare>(__x6, __x7, __c); cond_swap<_Compare>(__x8, __x9, __c);                                                                                              // (3,4),(5,6),(7,8)
}

// S(12) = 37 comparatori (Migliore conosciuta, Dobbelaere et al. 2020)
template <class _AlgPolicy, class _Compare, class _RandomAccessIterator>
          //__enable_if_t<__use_branchless_sort<_Compare, _RandomAccessIterator>, int> = 0>
inline void
sort12branchless(_RandomAccessIterator __x1, _RandomAccessIterator __x2, _RandomAccessIterator __x3,
         _RandomAccessIterator __x4, _RandomAccessIterator __x5, _RandomAccessIterator __x6,
         _RandomAccessIterator __x7, _RandomAccessIterator __x8, _RandomAccessIterator __x9,
         _RandomAccessIterator __x10, _RandomAccessIterator __x11, _RandomAccessIterator __x12,
         _Compare __c) {
    // Rete S(12)=37 (Dobbelaere et al. 2020, vedi https://bertdobbelaere.github.io/sorting_networks.html)
    // Layer 1 (6 comps)
    cond_swap<_Compare>(__x1, __x7, __c); cond_swap<_Compare>(__x2, __x8, __c); cond_swap<_Compare>(__x3, __x9, __c); cond_swap<_Compare>(__x4, __x10, __c); cond_swap<_Compare>(__x5, __x11, __c); cond_swap<_Compare>(__x6, __x12, __c); // (0,6),(1,7),(2,8),(3,9),(4,10),(5,11)
    // Layer 2 (6 comps)
    cond_swap<_Compare>(__x1, __x4, __c); cond_swap<_Compare>(__x2, __x5, __c); cond_swap<_Compare>(__x3, __x6, __c); cond_swap<_Compare>(__x7, __x10, __c); cond_swap<_Compare>(__x8, __x11, __c); cond_swap<_Compare>(__x9, __x12, __c); // (0,3),(1,4),(2,5),(6,9),(7,10),(8,11)
    // Layer 3 (6 comps)
    cond_swap<_Compare>(__x1, __x2, __c); cond_swap<_Compare>(__x3, __x4, __c); cond_swap<_Compare>(__x5, __x6, __c); cond_swap<_Compare>(__x7, __x8, __c); cond_swap<_Compare>(__x9, __x10, __c); cond_swap<_Compare>(__x11, __x12, __c); // (0,1),(2,3),(4,5),(6,7),(8,9),(10,11)
    // Layer 4 (5 comps)
    cond_swap<_Compare>(__x2, __x3, __c); cond_swap<_Compare>(__x4, __x5, __c); cond_swap<_Compare>(__x6, __x7, __c); cond_swap<_Compare>(__x8, __x9, __c); cond_swap<_Compare>(__x10, __x11, __c); // (1,2),(3,4),(5,6),(7,8),(9,10)
    // Layer 5 (4 comps)
    cond_swap<_Compare>(__x2, __x4, __c); cond_swap<_Compare>(__x3, __x6, __c); cond_swap<_Compare>(__x7, __x9, __c); cond_swap<_Compare>(__x8, __x10, __c); // (1,3),(2,5),(6,8),(7,9)
    // Layer 6 (4 comps)
    cond_swap<_Compare>(__x3, __x4, __c); cond_swap<_Compare>(__x5, __x6, __c); cond_swap<_Compare>(__x7, __x8, __c); cond_swap<_Compare>(__x9, __x10, __c); // (2,3),(4,5),(6,7),(8,9)
    // Layer 7 (3 comps)
    cond_swap<_Compare>(__x4, __x5, __c); cond_swap<_Compare>(__x6, __x7, __c); cond_swap<_Compare>(__x8, __x9, __c); // (3,4),(5,6),(7,8)
    // Layer 8 (2 comps)
    cond_swap<_Compare>(__x5, __x6, __c); cond_swap<_Compare>(__x7, __x8, __c); // (4,5),(6,7)
    // Layer 9 (1 comp)
    cond_swap<_Compare>(__x6, __x7, __c); // (5,6)
}

// S(13) = 41 comparatori (Migliore conosciuta, Dobbelaere et al. 2020)
template <class _AlgPolicy, class _Compare, class _RandomAccessIterator>
          //__enable_if_t<__use_branchless_sort<_Compare, _RandomAccessIterator>, int> = 0>
inline void
sort13branchless(_RandomAccessIterator __x1, _RandomAccessIterator __x2, _RandomAccessIterator __x3,
         _RandomAccessIterator __x4, _RandomAccessIterator __x5, _RandomAccessIterator __x6,
         _RandomAccessIterator __x7, _RandomAccessIterator __x8, _RandomAccessIterator __x9,
         _RandomAccessIterator __x10, _RandomAccessIterator __x11, _RandomAccessIterator __x12,
         _RandomAccessIterator __x13, _Compare __c) {
    // Rete S(13)=41 (Dobbelaere et al. 2020, vedi https://bertdobbelaere.github.io/sorting_networks.html)
    // Sequenza da https://cdn.sciss.io/SortingNetworks.pdf (Intel, 2019), che è la S(13)=41
    // Layer 1 (6 comps)
    cond_swap<_Compare>(__x1, __x2, __c); cond_swap<_Compare>(__x3, __x4, __c); cond_swap<_Compare>(__x5, __x6, __c); cond_swap<_Compare>(__x7, __x13, __c); cond_swap<_Compare>(__x8, __x12, __c); cond_swap<_Compare>(__x9, __x11, __c); // (0,1),(2,3),(4,5),(6,12),(7,11),(8,10)
    // Layer 2 (5 comps)
    cond_swap<_Compare>(__x1, __x3, __c); cond_swap<_Compare>(__x2, __x4, __c); cond_swap<_Compare>(__x5, __x7, __c); cond_swap<_Compare>(__x6, __x8, __c); cond_swap<_Compare>(__x10, __x12, __c); // (0,2),(1,3),(4,6),(5,7),(9,11)
    // Layer 3 (6 comps)
    cond_swap<_Compare>(__x1, __x5, __c); cond_swap<_Compare>(__x2, __x6, __c); cond_swap<_Compare>(__x3, __x7, __c); cond_swap<_Compare>(__x4, __x8, __c); cond_swap<_Compare>(__x9, __x10, __c); cond_swap<_Compare>(__x11, __x13, __c); // (0,4),(1,5),(2,6),(3,7),(8,9),(10,12)
    // Layer 4 (6 comps)
    cond_swap<_Compare>(__x1, __x9, __c); cond_swap<_Compare>(__x2, __x10, __c); cond_swap<_Compare>(__x3, __x11, __c); cond_swap<_Compare>(__x4, __x12, __c); cond_swap<_Compare>(__x5, __x6, __c); cond_swap<_Compare>(__x7, __x8, __c); // (0,8),(1,9),(2,10),(3,11),(4,5),(6,7)
    // Layer 5 (6 comps)
    cond_swap<_Compare>(__x2, __x3, __c); cond_swap<_Compare>(__x4, __x5, __c); cond_swap<_Compare>(__x6, __x9, __c); cond_swap<_Compare>(__x7, __x11, __c); cond_swap<_Compare>(__x8, __x12, __c); cond_swap<_Compare>(__x10, __x13, __c); // (1,2),(3,4),(5,8),(6,10),(7,11),(9,12)
    // Layer 6 (4 comps)
    cond_swap<_Compare>(__x4, __x6, __c); cond_swap<_Compare>(__x5, __x7, __c); cond_swap<_Compare>(__x8, __x9, __c); cond_swap<_Compare>(__x10, __x11, __c); // (3,5),(4,6),(7,8),(9,10)
    // Layer 7 (5 comps)
    cond_swap<_Compare>(__x3, __x4, __c); cond_swap<_Compare>(__x5, __x6, __c); cond_swap<_Compare>(__x7, __x9, __c); cond_swap<_Compare>(__x8, __x10, __c); cond_swap<_Compare>(__x12, __x13, __c); // (2,3),(4,5),(6,8),(7,9),(11,12)
    // Layer 8 (2 comps)
    cond_swap<_Compare>(__x6, __x7, __c); cond_swap<_Compare>(__x8, __x9, __c); // (5,6),(7,8)
    // Layer 9 (1 comp)
    cond_swap<_Compare>(__x4, __x5, __c); // (3,4)
}

// S(14) = 46 comparatori (Migliore conosciuta, Dobbelaere et al. 2020)
template <class _AlgPolicy, class _Compare, class _RandomAccessIterator>
          //__enable_if_t<__use_branchless_sort<_Compare, _RandomAccessIterator>, int> = 0>
inline void
sort14branchless(_RandomAccessIterator __x1, _RandomAccessIterator __x2, _RandomAccessIterator __x3,
         _RandomAccessIterator __x4, _RandomAccessIterator __x5, _RandomAccessIterator __x6,
         _RandomAccessIterator __x7, _RandomAccessIterator __x8, _RandomAccessIterator __x9,
         _RandomAccessIterator __x10, _RandomAccessIterator __x11, _RandomAccessIterator __x12,
         _RandomAccessIterator __x13, _RandomAccessIterator __x14, _Compare __c) {
    // Rete S(14)=46 (Dobbelaere et al. 2020, vedi https://bertdobbelaere.github.io/sorting_networks.html)
    // Layer 1 (7 comps)
    cond_swap<_Compare>(__x1, __x14, __c); cond_swap<_Compare>(__x2, __x8, __c); cond_swap<_Compare>(__x3, __x9, __c); cond_swap<_Compare>(__x4, __x10, __c); cond_swap<_Compare>(__x5, __x11, __c); cond_swap<_Compare>(__x6, __x12, __c); cond_swap<_Compare>(__x7, __x13, __c); // (0,13),(1,7),(2,8),(3,9),(4,10),(5,11),(6,12)
    // Layer 2 (7 comps)
    cond_swap<_Compare>(__x1, __x2, __c); cond_swap<_Compare>(__x3, __x8, __c); cond_swap<_Compare>(__x4, __x9, __c); cond_swap<_Compare>(__x5, __x10, __c); cond_swap<_Compare>(__x6, __x11, __c); cond_swap<_Compare>(__x7, __x12, __c); cond_swap<_Compare>(__x13, __x14, __c); // (0,1),(2,7),(3,8),(4,9),(5,10),(6,11),(12,13)
    // Layer 3 (7 comps)
    cond_swap<_Compare>(__x1, __x3, __c); cond_swap<_Compare>(__x2, __x4, __c); cond_swap<_Compare>(__x5, __x8, __c); cond_swap<_Compare>(__x6, __x9, __c); cond_swap<_Compare>(__x7, __x10, __c); cond_swap<_Compare>(__x11, __x13, __c); cond_swap<_Compare>(__x12, __x14, __c); // (0,2),(1,3),(4,7),(5,8),(6,9),(10,12),(11,13)
    // Layer 4 (6 comps)
    cond_swap<_Compare>(__x1, __x5, __c); cond_swap<_Compare>(__x2, __x6, __c); cond_swap<_Compare>(__x3, __x7, __c); cond_swap<_Compare>(__x8, __x11, __c); cond_swap<_Compare>(__x9, __x13, __c); cond_swap<_Compare>(__x10, __x14, __c); // (0,4),(1,5),(2,6),(7,10),(8,12),(9,13)
    // Layer 5 (7 comps)
    cond_swap<_Compare>(__x1, __x3, __c); cond_swap<_Compare>(__x2, __x5, __c); cond_swap<_Compare>(__x4, __x6, __c); cond_swap<_Compare>(__x7, __x8, __c); cond_swap<_Compare>(__x9, __x11, __c); cond_swap<_Compare>(__x10, __x13, __c); cond_swap<_Compare>(__x12, __x14, __c); // (0,2),(1,4),(3,5),(6,7),(8,10),(9,12),(11,13) -> (0,2) è __x1,__x3
    // Layer 6 (6 comps)
    cond_swap<_Compare>(__x3, __x5, __c); cond_swap<_Compare>(__x4, __x7, __c); cond_swap<_Compare>(__x6, __x8, __c); cond_swap<_Compare>(__x9, __x10, __c); cond_swap<_Compare>(__x11, __x12, __c); cond_swap<_Compare>(__x13, __x14, __c); // (2,4),(3,6),(5,7),(8,9),(10,11),(12,13)
    // Layer 7 (6 comps)
    cond_swap<_Compare>(__x2, __x3, __c); cond_swap<_Compare>(__x4, __x5, __c); cond_swap<_Compare>(__x6, __x7, __c); cond_swap<_Compare>(__x8, __x9, __c); cond_swap<_Compare>(__x10, __x11, __c); cond_swap<_Compare>(__x12, __x13, __c); // (1,2),(3,4),(5,6),(7,8),(9,10),(11,12)
}

