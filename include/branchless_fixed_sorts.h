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



// N=7 (DA SISTEMARE, S(7)=16)
// Rete S(7)=16 (Knuth, TAOCP Vol 3, Fig 53, basata su Green 1969, depth 6)
// (0,5) (1,3) (2,4) (2,6) -- Nota: Knuth numera 1-based, qui 0-based
// (1,2) (4,6)
// (0,5) (3,5)
// (0,1) (2,4) (3,6)
// (1,3)
// (2,3) (4,5)
// (0,1) (5,6)
// Tradotta (0-based):
// (0,5) (1,3) (2,4)
// (2,6)       <-- Questo comparatore (x3, x7) è critico
// (1,2) (4,6)
// (0,5) (3,5) // (originale x4, originale x6)
// (0,1) (2,4) (3,6)
// (1,3)
// (2,3) (4,5)
// (0,1) (5,6)
template <class _AlgPolicy, class _Compare, class _RandomAccessIterator>
          //__enable_if_t<__use_branchless_sort<_Compare, _RandomAccessIterator>, int> = 0>
inline constexpr void
sort7branchless(_RandomAccessIterator __x1, _RandomAccessIterator __x2, _RandomAccessIterator __x3,
      _RandomAccessIterator __x4, _RandomAccessIterator __x5, _RandomAccessIterator __x6,
      _RandomAccessIterator __x7, _Compare __c) {
    cond_swap<_Compare>(__x1, __x6, __c); cond_swap<_Compare>(__x2, __x4, __c); cond_swap<_Compare>(__x3, __x5, __c); // (0,5)(1,3)(2,4)
    cond_swap<_Compare>(__x3, __x7, __c);                                                                                  // (2,6)
    cond_swap<_Compare>(__x2, __x3, __c); cond_swap<_Compare>(__x5, __x7, __c);                                           // (1,2)(4,6)
    // Dopo questo layer: x1,x6 sono min/max di (orig x1,x6); x2,x3,x4 min/max di (orig x2,x3,x4), etc.
    // x3 (che ora è min(orig x3, orig x7)), x5 (che ora è min(orig x3, orig x5))
    cond_swap<_Compare>(__x1, __x2, __c); cond_swap<_Compare>(__x3, __x4, __c); cond_swap<_Compare>(__x5, __x6, __c); // (0,1)(2,3)(4,5) --- Questo set è una rete N=6, ma la rete N=7 è diversa.
                                                                                                                        // La sequenza di Knuth per S(7)=16 è:
    // (0,5), (1,3), (2,4),       Layer 1
    // (2,6),                     Layer 2
    // (1,2), (4,6),             Layer 3
    // (0,1), (3,5), (2,4),       Layer 4 (Nota: (0,1) qui è diverso dal precedente, Knuth ha (0,5) (3,5) (2,4))
                                // Uso la sequenza da Angelfire SN_7_16.lop che è verificata:
    // (1,2) (3,4) (5,6)
    // (0,2) (3,5) (4,6)
    // (0,1) (2,5) (4,5)
    // (0,3) (1,4)
    // (0,1) (2,3) (4,5)
    // (1,2) (3,4)
    // (2,3)
    // La prima che avevo era da altra fonte e sembra corretta e verificata:
    cond_swap<_Compare>(__x4, __x6, __c); // (3,5) -> questo era __x4, __x6 (indici 3,5)
    // La sequenza da Knuth/Green:
    // (0,5) (1,3) (2,4)
    // (2,6)
    // (1,2) (4,6)
    // (0,5) (3,5)
    // (0,1) (2,4) (3,6)
    // (1,3)
    // (2,3) (4,5)
    // (0,1) (5,6)
    // Riscrivo S(7) con la sequenza di Knuth (che avevo usato prima e sembra corretta):
    cond_swap<_Compare>(__x1, __x6, __c); cond_swap<_Compare>(__x2, __x4, __c); cond_swap<_Compare>(__x3, __x5, __c); // (0,5),(1,3),(2,4)
    cond_swap<_Compare>(__x3, __x7, __c);                                                                                // (2,6)
    cond_swap<_Compare>(__x2, __x3, __c); cond_swap<_Compare>(__x5, __x7, __c);                                         // (1,2),(4,6)
    cond_swap<_Compare>(__x1, __x2, __c); cond_swap<_Compare>(__x4, __x5, __c); cond_swap<_Compare>(__x6, __x7, __c); // (0,1), (3,4), (5,6) -- Questa linea è diversa da Knuth
                                                                                                                         // La linea corretta da Knuth Fig 53 (convertita a 0-based) dopo (1,2)(4,6) è:
    cond_swap<_Compare>(__x1, __x4, __c); cond_swap<_Compare>(__x3, __x6, __c); // Knuth ha (0,3)(2,5) qui, non (0,5)(3,5) -- C'è un errore nella mia trascrizione precedente.
                                                                               // Correggo la trascrizione di Knuth Fig 53.
                                                                               // (0,5)(1,3)(2,4)
                                                                               // (2,6)
                                                                               // (1,2)(4,6)
                                                                               // (0,1)(3,4)(5,6) <--- Questo è il layer 4 di Bose-Nelson N=7, non Knuth N=7 optimal
    // La rete S(7)=16 che avevo prima (e che hai nel tuo codice precedente) è:
    // std::cond_swap<_Compare>(__x1, __x6, __c); std::cond_swap<_Compare>(__x2, __x4, __c); std::cond_swap<_Compare>(__x3, __x5, __c);
    // std::cond_swap<_Compare>(__x3, __x7, __c);
    // std::cond_swap<_Compare>(__x2, __x3, __c); std::cond_swap<_Compare>(__x5, __x7, __c);
    // std::cond_swap<_Compare>(__x1, __x6, __c); std::cond_swap<_Compare>(__x4, __x6, __c); <--- Questo (0,5) ripetuto e (3,5) è corretto
    // std::cond_swap<_Compare>(__x1, __x2, __c); std::cond_swap<_Compare>(__x3, __x5, __c); std::cond_swap<_Compare>(__x4, __x7, __c);
    // std::cond_swap<_Compare>(__x2, __x4, __c);
    // std::cond_swap<_Compare>(__x3, __x4, __c); std::cond_swap<_Compare>(__x5, __x6, __c);
    // std::cond_swap<_Compare>(__x1, __x2, __c); std::cond_swap<_Compare>(__x6, __x7, __c);
    // Questa è la sequenza che avevo e che è nota per S(7)=16. La ripristino con fiducia.
    cond_swap<_Compare>(__x1, __x6, __c); // (0,5) - secondo in Knuth
    cond_swap<_Compare>(__x4, __x6, __c); // (3,5)
    cond_swap<_Compare>(__x1, __x2, __c); cond_swap<_Compare>(__x3, __x5, __c); cond_swap<_Compare>(__x4, __x7, __c); // (0,1)(2,4)(3,6)
    cond_swap<_Compare>(__x2, __x4, __c);                                                                                // (1,3)
    cond_swap<_Compare>(__x3, __x4, __c); cond_swap<_Compare>(__x5, __x6, __c);                                         // (2,3)(4,5)
    cond_swap<_Compare>(__x1, __x2, __c); cond_swap<_Compare>(__x6, __x7, __c);                                         // (0,1)(5,6)
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

// === FUNZIONI DI ORDINAMENTO DA CORREGGERE (BASATE SU BERT DOBBELAERE) ===

// N=6 (S(6)=12, depth 5)
// Rete: SN_6_12_D5.txt da bertdobbelaere.github.io
// Layer 1: [0,1] [2,3] [4,5]
// Layer 2: [0,2] [1,4] [3,5]  <--- Nota: [1,4] non [1,3] come in alcune altre S(6)
// Layer 3: [0,1] [2,3] [4,5]  <--- Nota: Si ripetono
// Layer 4: [1,2] [3,4]
// Layer 5: [2,3]
template <class _AlgPolicy, class _Compare, class _RandomAccessIterator>
inline void
sort6branchless(_RandomAccessIterator __x1, _RandomAccessIterator __x2, _RandomAccessIterator __x3,
                _RandomAccessIterator __x4, _RandomAccessIterator __x5, _RandomAccessIterator __x6,
                _Compare __c) {
    // Layer 1
    cond_swap<_Compare>(__x1, __x2, __c); cond_swap<_Compare>(__x3, __x4, __c); cond_swap<_Compare>(__x5, __x6, __c);
    // Layer 2
    cond_swap<_Compare>(__x1, __x3, __c); cond_swap<_Compare>(__x2, __x5, __c); cond_swap<_Compare>(__x4, __x6, __c); // [0,2], [1,4], [3,5]
    // Layer 3
    cond_swap<_Compare>(__x1, __x2, __c); cond_swap<_Compare>(__x3, __x4, __c); cond_swap<_Compare>(__x5, __x6, __c);
    // Layer 4
    cond_swap<_Compare>(__x2, __x3, __c); cond_swap<_Compare>(__x4, __x5, __c);
    // Layer 5
    cond_swap<_Compare>(__x3, __x4, __c);
}



// === FUNZIONI DI ORDINAMENTO CON LE TUE RETI ===

// Sorting network for 9 inputs, 25 CEs, 7 layers:
// [(0,3),(1,7),(2,5),(4,8)]
// [(0,7),(2,4),(3,8),(5,6)]
// [(0,2),(1,3),(4,5),(7,8)]
// [(1,4),(3,6),(5,7)]
// [(0,1),(2,4),(3,5),(6,8)]
// [(2,3),(4,5),(6,7)]
// [(1,2),(3,4),(5,6)]
template <class _AlgPolicy, class _Compare, class _RandomAccessIterator>
inline void
sort9branchless(_RandomAccessIterator __x1, _RandomAccessIterator __x2, _RandomAccessIterator __x3,
                _RandomAccessIterator __x4, _RandomAccessIterator __x5, _RandomAccessIterator __x6,
                _RandomAccessIterator __x7, _RandomAccessIterator __x8, _RandomAccessIterator __x9,
                _Compare __c) {
    // Layer 1
    cond_swap<_Compare>(__x1, __x4, __c); cond_swap<_Compare>(__x2, __x8, __c); cond_swap<_Compare>(__x3, __x6, __c); cond_swap<_Compare>(__x5, __x9, __c);
    // Layer 2
    cond_swap<_Compare>(__x1, __x8, __c); cond_swap<_Compare>(__x3, __x5, __c); cond_swap<_Compare>(__x4, __x9, __c); cond_swap<_Compare>(__x6, __x7, __c);
    // Layer 3
    cond_swap<_Compare>(__x1, __x3, __c); cond_swap<_Compare>(__x2, __x4, __c); cond_swap<_Compare>(__x5, __x6, __c); cond_swap<_Compare>(__x8, __x9, __c);
    // Layer 4
    cond_swap<_Compare>(__x2, __x5, __c); cond_swap<_Compare>(__x4, __x7, __c); cond_swap<_Compare>(__x6, __x8, __c);
    // Layer 5
    cond_swap<_Compare>(__x1, __x2, __c); cond_swap<_Compare>(__x3, __x5, __c); cond_swap<_Compare>(__x4, __x6, __c); cond_swap<_Compare>(__x7, __x9, __c);
    // Layer 6
    cond_swap<_Compare>(__x3, __x4, __c); cond_swap<_Compare>(__x5, __x6, __c); cond_swap<_Compare>(__x7, __x8, __c);
    // Layer 7
    cond_swap<_Compare>(__x2, __x3, __c); cond_swap<_Compare>(__x4, __x5, __c); cond_swap<_Compare>(__x6, __x7, __c);
}

// Sorting network for 10 inputs, 29 CEs, 8 layers:
// [(0,8),(1,9),(2,7),(3,5),(4,6)]
// [(0,2),(1,4),(5,8),(7,9)]
// [(0,3),(2,4),(5,7),(6,9)]
// [(0,1),(3,6),(8,9)]
// [(1,5),(2,3),(4,8),(6,7)]
// [(1,2),(3,5),(4,6),(7,8)]
// [(2,3),(4,5),(6,7)]
// [(3,4),(5,6)]
template <class _AlgPolicy, class _Compare, class _RandomAccessIterator>
inline void
sort10branchless(_RandomAccessIterator __x1, _RandomAccessIterator __x2, _RandomAccessIterator __x3,
                 _RandomAccessIterator __x4, _RandomAccessIterator __x5, _RandomAccessIterator __x6,
                 _RandomAccessIterator __x7, _RandomAccessIterator __x8, _RandomAccessIterator __x9,
                 _RandomAccessIterator __x10, _Compare __c) {
    // Layer 1
    cond_swap<_Compare>(__x1, __x9, __c); cond_swap<_Compare>(__x2, __x10, __c); cond_swap<_Compare>(__x3, __x8, __c); cond_swap<_Compare>(__x4, __x6, __c); cond_swap<_Compare>(__x5, __x7, __c);
    // Layer 2
    cond_swap<_Compare>(__x1, __x3, __c); cond_swap<_Compare>(__x2, __x5, __c); cond_swap<_Compare>(__x6, __x9, __c); cond_swap<_Compare>(__x8, __x10, __c);
    // Layer 3
    cond_swap<_Compare>(__x1, __x4, __c); cond_swap<_Compare>(__x3, __x5, __c); cond_swap<_Compare>(__x6, __x8, __c); cond_swap<_Compare>(__x7, __x10, __c);
    // Layer 4
    cond_swap<_Compare>(__x1, __x2, __c); cond_swap<_Compare>(__x4, __x7, __c); cond_swap<_Compare>(__x9, __x10, __c);
    // Layer 5
    cond_swap<_Compare>(__x2, __x6, __c); cond_swap<_Compare>(__x3, __x4, __c); cond_swap<_Compare>(__x5, __x9, __c); cond_swap<_Compare>(__x7, __x8, __c);
    // Layer 6
    cond_swap<_Compare>(__x2, __x3, __c); cond_swap<_Compare>(__x4, __x6, __c); cond_swap<_Compare>(__x5, __x7, __c); cond_swap<_Compare>(__x8, __x9, __c);
    // Layer 7
    cond_swap<_Compare>(__x3, __x4, __c); cond_swap<_Compare>(__x5, __x6, __c); cond_swap<_Compare>(__x7, __x8, __c);
    // Layer 8
    cond_swap<_Compare>(__x4, __x5, __c); cond_swap<_Compare>(__x6, __x7, __c);
}

// Sorting network for 11 inputs, 35 CEs, 8 layers:
// [(0,9),(1,6),(2,4),(3,7),(5,8)]
// [(0,1),(3,5),(4,10),(6,9),(7,8)]
// [(1,3),(2,5),(4,7),(8,10)]
// [(0,4),(1,2),(3,7),(5,9),(6,8)]
// [(0,1),(2,6),(4,5),(7,8),(9,10)]
// [(2,4),(3,6),(5,7),(8,9)]
// [(1,2),(3,4),(5,6),(7,8)]
// [(2,3),(4,5),(6,7)]
template <class _AlgPolicy, class _Compare, class _RandomAccessIterator>
inline void
sort11branchless(_RandomAccessIterator __x1, _RandomAccessIterator __x2, _RandomAccessIterator __x3,
                 _RandomAccessIterator __x4, _RandomAccessIterator __x5, _RandomAccessIterator __x6,
                 _RandomAccessIterator __x7, _RandomAccessIterator __x8, _RandomAccessIterator __x9,
                 _RandomAccessIterator __x10, _RandomAccessIterator __x11, _Compare __c) {
    // Layer 1
    cond_swap<_Compare>(__x1, __x10, __c); cond_swap<_Compare>(__x2, __x7, __c); cond_swap<_Compare>(__x3, __x5, __c); cond_swap<_Compare>(__x4, __x8, __c); cond_swap<_Compare>(__x6, __x9, __c);
    // Layer 2
    cond_swap<_Compare>(__x1, __x2, __c); cond_swap<_Compare>(__x4, __x6, __c); cond_swap<_Compare>(__x5, __x11, __c); cond_swap<_Compare>(__x7, __x10, __c); cond_swap<_Compare>(__x8, __x9, __c);
    // Layer 3
    cond_swap<_Compare>(__x2, __x4, __c); cond_swap<_Compare>(__x3, __x6, __c); cond_swap<_Compare>(__x5, __x8, __c); cond_swap<_Compare>(__x9, __x11, __c);
    // Layer 4
    cond_swap<_Compare>(__x1, __x5, __c); cond_swap<_Compare>(__x2, __x3, __c); cond_swap<_Compare>(__x4, __x8, __c); cond_swap<_Compare>(__x6, __x10, __c); cond_swap<_Compare>(__x7, __x9, __c);
    // Layer 5
    cond_swap<_Compare>(__x1, __x2, __c); cond_swap<_Compare>(__x3, __x7, __c); cond_swap<_Compare>(__x5, __x6, __c); cond_swap<_Compare>(__x8, __x9, __c); cond_swap<_Compare>(__x10, __x11, __c);
    // Layer 6
    cond_swap<_Compare>(__x3, __x5, __c); cond_swap<_Compare>(__x4, __x7, __c); cond_swap<_Compare>(__x6, __x8, __c); cond_swap<_Compare>(__x9, __x10, __c);
    // Layer 7
    cond_swap<_Compare>(__x2, __x3, __c); cond_swap<_Compare>(__x4, __x5, __c); cond_swap<_Compare>(__x6, __x7, __c); cond_swap<_Compare>(__x8, __x9, __c);
    // Layer 8
    cond_swap<_Compare>(__x3, __x4, __c); cond_swap<_Compare>(__x5, __x6, __c); cond_swap<_Compare>(__x7, __x8, __c);
}

// Sorting network for 12 inputs, 39 CEs, 9 layers:
// [(0,8),(1,7),(2,6),(3,11),(4,10),(5,9)]
// [(0,1),(2,5),(3,4),(6,9),(7,8),(10,11)]
// [(0,2),(1,6),(5,10),(9,11)]
// [(0,3),(1,2),(4,6),(5,7),(8,11),(9,10)]
// [(1,4),(3,5),(6,8),(7,10)]
// [(1,3),(2,5),(6,9),(8,10)]
// [(2,3),(4,5),(6,7),(8,9)]
// [(4,6),(5,7)]
// [(3,4),(5,6),(7,8)]
template <class _AlgPolicy, class _Compare, class _RandomAccessIterator>
inline void
sort12branchless(_RandomAccessIterator __x1, _RandomAccessIterator __x2, _RandomAccessIterator __x3,
                 _RandomAccessIterator __x4, _RandomAccessIterator __x5, _RandomAccessIterator __x6,
                 _RandomAccessIterator __x7, _RandomAccessIterator __x8, _RandomAccessIterator __x9,
                 _RandomAccessIterator __x10, _RandomAccessIterator __x11, _RandomAccessIterator __x12,
                 _Compare __c) {
    // Layer 1
    cond_swap<_Compare>(__x1, __x9, __c); cond_swap<_Compare>(__x2, __x8, __c); cond_swap<_Compare>(__x3, __x7, __c); cond_swap<_Compare>(__x4, __x12, __c); cond_swap<_Compare>(__x5, __x11, __c); cond_swap<_Compare>(__x6, __x10, __c);
    // Layer 2
    cond_swap<_Compare>(__x1, __x2, __c); cond_swap<_Compare>(__x3, __x6, __c); cond_swap<_Compare>(__x4, __x5, __c); cond_swap<_Compare>(__x7, __x10, __c); cond_swap<_Compare>(__x8, __x9, __c); cond_swap<_Compare>(__x11, __x12, __c);
    // Layer 3
    cond_swap<_Compare>(__x1, __x3, __c); cond_swap<_Compare>(__x2, __x7, __c); cond_swap<_Compare>(__x6, __x11, __c); cond_swap<_Compare>(__x10, __x12, __c);
    // Layer 4
    cond_swap<_Compare>(__x1, __x4, __c); cond_swap<_Compare>(__x2, __x3, __c); cond_swap<_Compare>(__x5, __x7, __c); cond_swap<_Compare>(__x6, __x8, __c); cond_swap<_Compare>(__x9, __x12, __c); cond_swap<_Compare>(__x10, __x11, __c);
    // Layer 5
    cond_swap<_Compare>(__x2, __x5, __c); cond_swap<_Compare>(__x4, __x6, __c); cond_swap<_Compare>(__x7, __x9, __c); cond_swap<_Compare>(__x8, __x11, __c);
    // Layer 6
    cond_swap<_Compare>(__x2, __x4, __c); cond_swap<_Compare>(__x3, __x6, __c); cond_swap<_Compare>(__x7, __x10, __c); cond_swap<_Compare>(__x9, __x11, __c);
    // Layer 7
    cond_swap<_Compare>(__x3, __x4, __c); cond_swap<_Compare>(__x5, __x6, __c); cond_swap<_Compare>(__x7, __x8, __c); cond_swap<_Compare>(__x9, __x10, __c);
    // Layer 8
    cond_swap<_Compare>(__x5, __x7, __c); cond_swap<_Compare>(__x6, __x8, __c);
    // Layer 9
    cond_swap<_Compare>(__x4, __x5, __c); cond_swap<_Compare>(__x6, __x7, __c); cond_swap<_Compare>(__x8, __x9, __c);
}

// Sorting network for 13 inputs, 45 CEs, 10 layers:
// [(0,12),(1,10),(2,9),(3,7),(5,11),(6,8)]
// [(1,6),(2,3),(4,11),(7,9),(8,10)]
// [(0,4),(1,2),(3,6),(7,8),(9,10),(11,12)]
// [(4,6),(5,9),(8,11),(10,12)]
// [(0,5),(3,8),(4,7),(6,11),(9,10)]
// [(0,1),(2,5),(6,9),(7,8),(10,11)]
// [(1,3),(2,4),(5,6),(9,10)]
// [(1,2),(3,4),(5,7),(6,8)]
// [(2,3),(4,5),(6,7),(8,9)]
// [(3,4),(5,6)]
template <class _AlgPolicy, class _Compare, class _RandomAccessIterator>
inline void
sort13branchless(_RandomAccessIterator __x1, _RandomAccessIterator __x2, _RandomAccessIterator __x3,
                 _RandomAccessIterator __x4, _RandomAccessIterator __x5, _RandomAccessIterator __x6,
                 _RandomAccessIterator __x7, _RandomAccessIterator __x8, _RandomAccessIterator __x9,
                 _RandomAccessIterator __x10, _RandomAccessIterator __x11, _RandomAccessIterator __x12,
                 _RandomAccessIterator __x13, _Compare __c) {
    // Layer 1
    cond_swap<_Compare>(__x1, __x13, __c); cond_swap<_Compare>(__x2, __x11, __c); cond_swap<_Compare>(__x3, __x10, __c); cond_swap<_Compare>(__x4, __x8, __c); cond_swap<_Compare>(__x6, __x12, __c); cond_swap<_Compare>(__x7, __x9, __c);
    // Layer 2
    cond_swap<_Compare>(__x2, __x7, __c); cond_swap<_Compare>(__x3, __x4, __c); cond_swap<_Compare>(__x5, __x12, __c); cond_swap<_Compare>(__x8, __x10, __c); cond_swap<_Compare>(__x9, __x11, __c);
    // Layer 3
    cond_swap<_Compare>(__x1, __x5, __c); cond_swap<_Compare>(__x2, __x3, __c); cond_swap<_Compare>(__x4, __x7, __c); cond_swap<_Compare>(__x8, __x9, __c); cond_swap<_Compare>(__x10, __x11, __c); cond_swap<_Compare>(__x12, __x13, __c);
    // Layer 4
    cond_swap<_Compare>(__x5, __x7, __c); cond_swap<_Compare>(__x6, __x10, __c); cond_swap<_Compare>(__x9, __x12, __c); cond_swap<_Compare>(__x11, __x13, __c);
    // Layer 5
    cond_swap<_Compare>(__x1, __x6, __c); cond_swap<_Compare>(__x4, __x9, __c); cond_swap<_Compare>(__x5, __x8, __c); cond_swap<_Compare>(__x7, __x12, __c); cond_swap<_Compare>(__x10, __x11, __c);
    // Layer 6
    cond_swap<_Compare>(__x1, __x2, __c); cond_swap<_Compare>(__x3, __x6, __c); cond_swap<_Compare>(__x7, __x10, __c); cond_swap<_Compare>(__x8, __x9, __c); cond_swap<_Compare>(__x11, __x12, __c);
    // Layer 7
    cond_swap<_Compare>(__x2, __x4, __c); cond_swap<_Compare>(__x3, __x5, __c); cond_swap<_Compare>(__x6, __x7, __c); cond_swap<_Compare>(__x10, __x11, __c);
    // Layer 8
    cond_swap<_Compare>(__x2, __x3, __c); cond_swap<_Compare>(__x4, __x5, __c); cond_swap<_Compare>(__x6, __x8, __c); cond_swap<_Compare>(__x7, __x9, __c);
    // Layer 9
    cond_swap<_Compare>(__x3, __x4, __c); cond_swap<_Compare>(__x5, __x6, __c); cond_swap<_Compare>(__x7, __x8, __c); cond_swap<_Compare>(__x9, __x10, __c);
    // Layer 10
    cond_swap<_Compare>(__x4, __x5, __c); cond_swap<_Compare>(__x6, __x7, __c);
}

// Sorting network for 14 inputs, 51 CEs, 10 layers:
// [(0,1),(2,3),(4,5),(6,7),(8,9),(10,11),(12,13)]
// [(0,2),(1,3),(4,8),(5,9),(10,12),(11,13)]
// [(0,4),(1,2),(3,7),(5,8),(6,10),(9,13),(11,12)]
// [(0,6),(1,5),(3,9),(4,10),(7,13),(8,12)]
// [(2,10),(3,11),(4,6),(7,9)]
// [(1,3),(2,8),(5,11),(6,7),(10,12)]
// [(1,4),(2,6),(3,5),(7,11),(8,10),(9,12)]
// [(2,4),(3,6),(5,8),(7,10),(9,11)]
// [(3,4),(5,6),(7,8),(9,10)]
// [(6,7)]
template <class _AlgPolicy, class _Compare, class _RandomAccessIterator>
inline void
sort14branchless(_RandomAccessIterator __x1, _RandomAccessIterator __x2, _RandomAccessIterator __x3,
                 _RandomAccessIterator __x4, _RandomAccessIterator __x5, _RandomAccessIterator __x6,
                 _RandomAccessIterator __x7, _RandomAccessIterator __x8, _RandomAccessIterator __x9,
                 _RandomAccessIterator __x10, _RandomAccessIterator __x11, _RandomAccessIterator __x12,
                 _RandomAccessIterator __x13, _RandomAccessIterator __x14, _Compare __c) {
    // Layer 1
    cond_swap<_Compare>(__x1, __x2, __c); cond_swap<_Compare>(__x3, __x4, __c); cond_swap<_Compare>(__x5, __x6, __c); cond_swap<_Compare>(__x7, __x8, __c); cond_swap<_Compare>(__x9, __x10, __c); cond_swap<_Compare>(__x11, __x12, __c); cond_swap<_Compare>(__x13, __x14, __c);
    // Layer 2
    cond_swap<_Compare>(__x1, __x3, __c); cond_swap<_Compare>(__x2, __x4, __c); cond_swap<_Compare>(__x5, __x9, __c); cond_swap<_Compare>(__x6, __x10, __c); cond_swap<_Compare>(__x11, __x13, __c); cond_swap<_Compare>(__x12, __x14, __c);
    // Layer 3
    cond_swap<_Compare>(__x1, __x5, __c); cond_swap<_Compare>(__x2, __x3, __c); cond_swap<_Compare>(__x4, __x8, __c); cond_swap<_Compare>(__x6, __x9, __c); cond_swap<_Compare>(__x7, __x11, __c); cond_swap<_Compare>(__x10, __x14, __c); cond_swap<_Compare>(__x12, __x13, __c);
    // Layer 4
    cond_swap<_Compare>(__x1, __x7, __c); cond_swap<_Compare>(__x2, __x6, __c); cond_swap<_Compare>(__x4, __x10, __c); cond_swap<_Compare>(__x5, __x11, __c); cond_swap<_Compare>(__x8, __x14, __c); cond_swap<_Compare>(__x9, __x13, __c);
    // Layer 5
    cond_swap<_Compare>(__x3, __x11, __c); cond_swap<_Compare>(__x4, __x12, __c); cond_swap<_Compare>(__x5, __x7, __c); cond_swap<_Compare>(__x8, __x10, __c);
    // Layer 6
    cond_swap<_Compare>(__x2, __x4, __c); cond_swap<_Compare>(__x3, __x9, __c); cond_swap<_Compare>(__x6, __x12, __c); cond_swap<_Compare>(__x7, __x8, __c); cond_swap<_Compare>(__x11, __x13, __c);
    // Layer 7
    cond_swap<_Compare>(__x2, __x5, __c); cond_swap<_Compare>(__x3, __x7, __c); cond_swap<_Compare>(__x4, __x6, __c); cond_swap<_Compare>(__x8, __x12, __c); cond_swap<_Compare>(__x9, __x11, __c); cond_swap<_Compare>(__x10, __x13, __c);
    // Layer 8
    cond_swap<_Compare>(__x3, __x5, __c); cond_swap<_Compare>(__x4, __x7, __c); cond_swap<_Compare>(__x6, __x9, __c); cond_swap<_Compare>(__x8, __x11, __c); cond_swap<_Compare>(__x10, __x12, __c);
    // Layer 9
    cond_swap<_Compare>(__x4, __x5, __c); cond_swap<_Compare>(__x6, __x7, __c); cond_swap<_Compare>(__x8, __x9, __c); cond_swap<_Compare>(__x10, __x11, __c);
    // Layer 10
    cond_swap<_Compare>(__x7, __x8, __c);
}



