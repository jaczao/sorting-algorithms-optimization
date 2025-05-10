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

template <class,
class _Compare,
class _RandomAccessIterator>
//,__enable_if_t<__use_branchless_sort<_Compare, _RandomAccessIterator>, int> = 0>
inline constexpr bool
sort3branchless(_RandomAccessIterator __x1, _RandomAccessIterator __x2, _RandomAccessIterator __x3, _Compare __c) {
bool __swapped1 = cond_swap<_Compare>(__x2, __x3, __c);
bool __swapped2 = partially_sorted_swap<_Compare>(__x1, __x2, __x3, __c);
return __swapped1 || __swapped2;
}

template <class,
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

template <class,
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
template <class, class _Compare, class _RandomAccessIterator>
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
template <class, class _Compare, class _RandomAccessIterator>
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
template <class, class _Compare, class _RandomAccessIterator>
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
template <class, class _Compare, class _RandomAccessIterator>
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
template <class, class _Compare, class _RandomAccessIterator>
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
template <class, class _Compare, class _RandomAccessIterator>
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
template <class, class _Compare, class _RandomAccessIterator>
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
template <class, class _Compare, class _RandomAccessIterator>
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
template <class, class _Compare, class _RandomAccessIterator>
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





// === FUNZIONI DI ORDINAMENTO CON LE RETI DA BERT DOBBELAERE ===

// Sorting network for 15 inputs, 56 CEs, 10 layers:
// [(1,2),(3,10),(4,14),(5,8),(6,13),(7,12),(9,11)]
// [(0,14),(1,5),(2,8),(3,7),(6,9),(10,12),(11,13)]
// [(0,7),(1,6),(2,9),(4,10),(5,11),(8,13),(12,14)]
// [(0,6),(2,4),(3,5),(7,11),(8,10),(9,12),(13,14)]
// [(0,3),(1,2),(4,7),(5,9),(6,8),(10,11),(12,13)]
// [(0,1),(2,3),(4,6),(7,9),(10,12),(11,13)]
// [(1,2),(3,5),(8,10),(11,12)]
// [(3,4),(5,6),(7,8),(9,10)]
// [(2,3),(4,5),(6,7),(8,9),(10,11)]
// [(5,6),(7,8)]
template <class _AlgPolicy, class _Compare, class _RandomAccessIterator> // si puo togliere alg policy (?)
inline void
sort15branchless(_RandomAccessIterator __x1, _RandomAccessIterator __x2, _RandomAccessIterator __x3, _RandomAccessIterator __x4,
                 _RandomAccessIterator __x5, _RandomAccessIterator __x6, _RandomAccessIterator __x7, _RandomAccessIterator __x8,
                 _RandomAccessIterator __x9, _RandomAccessIterator __x10, _RandomAccessIterator __x11, _RandomAccessIterator __x12,
                 _RandomAccessIterator __x13, _RandomAccessIterator __x14, _RandomAccessIterator __x15, _Compare __c) {
    // Layer 1
    cond_swap<_Compare>(__x2, __x3, __c); cond_swap<_Compare>(__x4, __x11, __c); cond_swap<_Compare>(__x5, __x15, __c); cond_swap<_Compare>(__x6, __x9, __c); cond_swap<_Compare>(__x7, __x14, __c); cond_swap<_Compare>(__x8, __x13, __c); cond_swap<_Compare>(__x10, __x12, __c);
    // Layer 2
    cond_swap<_Compare>(__x1, __x15, __c); cond_swap<_Compare>(__x2, __x6, __c); cond_swap<_Compare>(__x3, __x9, __c); cond_swap<_Compare>(__x4, __x8, __c); cond_swap<_Compare>(__x7, __x10, __c); cond_swap<_Compare>(__x11, __x13, __c); cond_swap<_Compare>(__x12, __x14, __c);
    // Layer 3
    cond_swap<_Compare>(__x1, __x8, __c); cond_swap<_Compare>(__x2, __x7, __c); cond_swap<_Compare>(__x3, __x10, __c); cond_swap<_Compare>(__x5, __x11, __c); cond_swap<_Compare>(__x6, __x12, __c); cond_swap<_Compare>(__x9, __x14, __c); cond_swap<_Compare>(__x13, __x15, __c);
    // Layer 4
    cond_swap<_Compare>(__x1, __x7, __c); cond_swap<_Compare>(__x3, __x5, __c); cond_swap<_Compare>(__x4, __x6, __c); cond_swap<_Compare>(__x8, __x12, __c); cond_swap<_Compare>(__x9, __x11, __c); cond_swap<_Compare>(__x10, __x13, __c); cond_swap<_Compare>(__x14, __x15, __c);
    // Layer 5
    cond_swap<_Compare>(__x1, __x4, __c); cond_swap<_Compare>(__x2, __x3, __c); cond_swap<_Compare>(__x5, __x8, __c); cond_swap<_Compare>(__x6, __x10, __c); cond_swap<_Compare>(__x7, __x9, __c); cond_swap<_Compare>(__x11, __x12, __c); cond_swap<_Compare>(__x13, __x14, __c);
    // Layer 6
    cond_swap<_Compare>(__x1, __x2, __c); cond_swap<_Compare>(__x3, __x4, __c); cond_swap<_Compare>(__x5, __x7, __c); cond_swap<_Compare>(__x8, __x10, __c); cond_swap<_Compare>(__x11, __x13, __c); cond_swap<_Compare>(__x12, __x14, __c);
    // Layer 7
    cond_swap<_Compare>(__x2, __x3, __c); cond_swap<_Compare>(__x4, __x6, __c); cond_swap<_Compare>(__x9, __x11, __c); cond_swap<_Compare>(__x12, __x13, __c);
    // Layer 8
    cond_swap<_Compare>(__x4, __x5, __c); cond_swap<_Compare>(__x6, __x7, __c); cond_swap<_Compare>(__x8, __x9, __c); cond_swap<_Compare>(__x10, __x11, __c);
    // Layer 9
    cond_swap<_Compare>(__x3, __x4, __c); cond_swap<_Compare>(__x5, __x6, __c); cond_swap<_Compare>(__x7, __x8, __c); cond_swap<_Compare>(__x9, __x10, __c); cond_swap<_Compare>(__x11, __x12, __c);
    // Layer 10
    cond_swap<_Compare>(__x6, __x7, __c); cond_swap<_Compare>(__x8, __x9, __c);
}

// Sorting network for 16 inputs, 60 CEs, 10 layers:
// [(0,13),(1,12),(2,15),(3,14),(4,8),(5,6),(7,11),(9,10)]
// [(0,5),(1,7),(2,9),(3,4),(6,13),(8,14),(10,15),(11,12)]
// [(0,1),(2,3),(4,5),(6,8),(7,9),(10,11),(12,13),(14,15)]
// [(0,2),(1,3),(4,10),(5,11),(6,7),(8,9),(12,14),(13,15)]
// [(1,2),(3,12),(4,6),(5,7),(8,10),(9,11),(13,14)]
// [(1,4),(2,6),(5,8),(7,10),(9,13),(11,14)]
// [(2,4),(3,6),(9,12),(11,13)]
// [(3,5),(6,8),(7,9),(10,12)]
// [(3,4),(5,6),(7,8),(9,10),(11,12)]
// [(6,7),(8,9)]
template <class _AlgPolicy, class _Compare, class _RandomAccessIterator>
inline void
sort16branchless(_RandomAccessIterator __x1, _RandomAccessIterator __x2, _RandomAccessIterator __x3, _RandomAccessIterator __x4,
                 _RandomAccessIterator __x5, _RandomAccessIterator __x6, _RandomAccessIterator __x7, _RandomAccessIterator __x8,
                 _RandomAccessIterator __x9, _RandomAccessIterator __x10, _RandomAccessIterator __x11, _RandomAccessIterator __x12,
                 _RandomAccessIterator __x13, _RandomAccessIterator __x14, _RandomAccessIterator __x15, _RandomAccessIterator __x16,
                 _Compare __c) {
    // Layer 1
    cond_swap<_Compare>(__x1, __x14, __c); cond_swap<_Compare>(__x2, __x13, __c); cond_swap<_Compare>(__x3, __x16, __c); cond_swap<_Compare>(__x4, __x15, __c); cond_swap<_Compare>(__x5, __x9, __c); cond_swap<_Compare>(__x6, __x7, __c); cond_swap<_Compare>(__x8, __x12, __c); cond_swap<_Compare>(__x10, __x11, __c);
    // Layer 2
    cond_swap<_Compare>(__x1, __x6, __c); cond_swap<_Compare>(__x2, __x8, __c); cond_swap<_Compare>(__x3, __x10, __c); cond_swap<_Compare>(__x4, __x5, __c); cond_swap<_Compare>(__x7, __x14, __c); cond_swap<_Compare>(__x9, __x15, __c); cond_swap<_Compare>(__x11, __x16, __c); cond_swap<_Compare>(__x12, __x13, __c);
    // Layer 3
    cond_swap<_Compare>(__x1, __x2, __c); cond_swap<_Compare>(__x3, __x4, __c); cond_swap<_Compare>(__x5, __x6, __c); cond_swap<_Compare>(__x7, __x9, __c); cond_swap<_Compare>(__x8, __x10, __c); cond_swap<_Compare>(__x11, __x12, __c); cond_swap<_Compare>(__x13, __x14, __c); cond_swap<_Compare>(__x15, __x16, __c);
    // Layer 4
    cond_swap<_Compare>(__x1, __x3, __c); cond_swap<_Compare>(__x2, __x4, __c); cond_swap<_Compare>(__x5, __x11, __c); cond_swap<_Compare>(__x6, __x12, __c); cond_swap<_Compare>(__x7, __x8, __c); cond_swap<_Compare>(__x9, __x10, __c); cond_swap<_Compare>(__x13, __x15, __c); cond_swap<_Compare>(__x14, __x16, __c);
    // Layer 5
    cond_swap<_Compare>(__x2, __x3, __c); cond_swap<_Compare>(__x4, __x13, __c); cond_swap<_Compare>(__x5, __x7, __c); cond_swap<_Compare>(__x6, __x8, __c); cond_swap<_Compare>(__x9, __x11, __c); cond_swap<_Compare>(__x10, __x12, __c); cond_swap<_Compare>(__x14, __x15, __c);
    // Layer 6
    cond_swap<_Compare>(__x2, __x5, __c); cond_swap<_Compare>(__x3, __x7, __c); cond_swap<_Compare>(__x6, __x9, __c); cond_swap<_Compare>(__x8, __x11, __c); cond_swap<_Compare>(__x10, __x14, __c); cond_swap<_Compare>(__x12, __x15, __c);
    // Layer 7
    cond_swap<_Compare>(__x3, __x5, __c); cond_swap<_Compare>(__x4, __x7, __c); cond_swap<_Compare>(__x10, __x13, __c); cond_swap<_Compare>(__x12, __x14, __c);
    // Layer 8
    cond_swap<_Compare>(__x4, __x6, __c); cond_swap<_Compare>(__x7, __x9, __c); cond_swap<_Compare>(__x8, __x10, __c); cond_swap<_Compare>(__x11, __x13, __c);
    // Layer 9
    cond_swap<_Compare>(__x4, __x5, __c); cond_swap<_Compare>(__x6, __x7, __c); cond_swap<_Compare>(__x8, __x9, __c); cond_swap<_Compare>(__x10, __x11, __c); cond_swap<_Compare>(__x12, __x13, __c);
    // Layer 10
    cond_swap<_Compare>(__x7, __x8, __c); cond_swap<_Compare>(__x9, __x10, __c);
}

// Sorting network for 17 inputs, 71 CEs, 12 layers:
// [(0,11),(1,15),(2,10),(3,5),(4,6),(8,12),(9,16),(13,14)]
// [(0,6),(1,13),(2,8),(4,14),(5,15),(7,11)]
// [(0,8),(3,7),(4,9),(6,16),(10,11),(12,14)]
// [(0,2),(1,4),(5,6),(7,13),(8,9),(10,12),(11,14),(15,16)]
// [(0,3),(2,5),(6,11),(7,10),(9,13),(12,15),(14,16)]
// [(0,1),(3,4),(5,10),(6,9),(7,8),(11,15),(13,14)]
// [(1,2),(3,7),(4,8),(6,12),(11,13),(14,15)]
// [(1,3),(2,7),(4,5),(9,11),(10,12),(13,14)]
// [(2,3),(4,6),(5,7),(8,10)]
// [(3,4),(6,8),(7,9),(10,12)]
// [(5,6),(7,8),(9,10),(11,12)]
// [(4,5),(6,7),(8,9),(10,11),(12,13)]
template <class _AlgPolicy, class _Compare, class _RandomAccessIterator>
inline void
sort17branchless(_RandomAccessIterator __x1, _RandomAccessIterator __x2, _RandomAccessIterator __x3, _RandomAccessIterator __x4,
                 _RandomAccessIterator __x5, _RandomAccessIterator __x6, _RandomAccessIterator __x7, _RandomAccessIterator __x8,
                 _RandomAccessIterator __x9, _RandomAccessIterator __x10, _RandomAccessIterator __x11, _RandomAccessIterator __x12,
                 _RandomAccessIterator __x13, _RandomAccessIterator __x14, _RandomAccessIterator __x15, _RandomAccessIterator __x16,
                 _RandomAccessIterator __x17, _Compare __c) {
    // Layer 1
    cond_swap<_Compare>(__x1, __x12, __c); cond_swap<_Compare>(__x2, __x16, __c); cond_swap<_Compare>(__x3, __x11, __c); cond_swap<_Compare>(__x4, __x6, __c); cond_swap<_Compare>(__x5, __x7, __c); cond_swap<_Compare>(__x9, __x13, __c); cond_swap<_Compare>(__x10, __x17, __c); cond_swap<_Compare>(__x14, __x15, __c);
    // Layer 2
    cond_swap<_Compare>(__x1, __x7, __c); cond_swap<_Compare>(__x2, __x14, __c); cond_swap<_Compare>(__x3, __x9, __c); cond_swap<_Compare>(__x5, __x15, __c); cond_swap<_Compare>(__x6, __x16, __c); cond_swap<_Compare>(__x8, __x12, __c);
    // Layer 3
    cond_swap<_Compare>(__x1, __x9, __c); cond_swap<_Compare>(__x4, __x8, __c); cond_swap<_Compare>(__x5, __x10, __c); cond_swap<_Compare>(__x7, __x17, __c); cond_swap<_Compare>(__x11, __x12, __c); cond_swap<_Compare>(__x13, __x15, __c);
    // Layer 4
    cond_swap<_Compare>(__x1, __x3, __c); cond_swap<_Compare>(__x2, __x5, __c); cond_swap<_Compare>(__x6, __x7, __c); cond_swap<_Compare>(__x8, __x14, __c); cond_swap<_Compare>(__x9, __x10, __c); cond_swap<_Compare>(__x11, __x13, __c); cond_swap<_Compare>(__x12, __x15, __c); cond_swap<_Compare>(__x16, __x17, __c);
    // Layer 5
    cond_swap<_Compare>(__x1, __x4, __c); cond_swap<_Compare>(__x3, __x6, __c); cond_swap<_Compare>(__x7, __x12, __c); cond_swap<_Compare>(__x8, __x11, __c); cond_swap<_Compare>(__x10, __x14, __c); cond_swap<_Compare>(__x13, __x16, __c); cond_swap<_Compare>(__x15, __x17, __c);
    // Layer 6
    cond_swap<_Compare>(__x1, __x2, __c); cond_swap<_Compare>(__x4, __x5, __c); cond_swap<_Compare>(__x6, __x11, __c); cond_swap<_Compare>(__x7, __x10, __c); cond_swap<_Compare>(__x8, __x9, __c); cond_swap<_Compare>(__x12, __x16, __c); cond_swap<_Compare>(__x14, __x15, __c);
    // Layer 7
    cond_swap<_Compare>(__x2, __x3, __c); cond_swap<_Compare>(__x4, __x8, __c); cond_swap<_Compare>(__x5, __x9, __c); cond_swap<_Compare>(__x7, __x13, __c); cond_swap<_Compare>(__x12, __x14, __c); cond_swap<_Compare>(__x15, __x16, __c);
    // Layer 8
    cond_swap<_Compare>(__x2, __x4, __c); cond_swap<_Compare>(__x3, __x8, __c); cond_swap<_Compare>(__x5, __x6, __c); cond_swap<_Compare>(__x10, __x12, __c); cond_swap<_Compare>(__x11, __x13, __c); cond_swap<_Compare>(__x14, __x15, __c);
    // Layer 9
    cond_swap<_Compare>(__x3, __x4, __c); cond_swap<_Compare>(__x5, __x7, __c); cond_swap<_Compare>(__x6, __x8, __c); cond_swap<_Compare>(__x9, __x11, __c);
    // Layer 10
    cond_swap<_Compare>(__x4, __x5, __c); cond_swap<_Compare>(__x7, __x9, __c); cond_swap<_Compare>(__x8, __x10, __c); cond_swap<_Compare>(__x11, __x13, __c);
    // Layer 11
    cond_swap<_Compare>(__x6, __x7, __c); cond_swap<_Compare>(__x8, __x9, __c); cond_swap<_Compare>(__x10, __x11, __c); cond_swap<_Compare>(__x12, __x13, __c);
    // Layer 12
    cond_swap<_Compare>(__x5, __x6, __c); cond_swap<_Compare>(__x7, __x8, __c); cond_swap<_Compare>(__x9, __x10, __c); cond_swap<_Compare>(__x11, __x12, __c); cond_swap<_Compare>(__x13, __x14, __c);
}

// Sorting network for 18 inputs, 77 CEs, 12 layers:
// [(0,1),(2,3),(4,5),(6,7),(8,9),(10,11),(12,13),(14,15),(16,17)]
// [(0,2),(1,3),(4,12),(5,13),(6,8),(9,11),(14,16),(15,17)]
// [(0,14),(1,16),(2,15),(3,17)]
// [(0,6),(1,10),(2,9),(7,16),(8,15),(11,17)]
// [(1,4),(3,9),(5,7),(8,14),(10,12),(13,16)]
// [(0,1),(2,5),(3,13),(4,14),(7,9),(8,10),(12,15),(16,17)]
// [(1,2),(3,5),(4,6),(11,13),(12,14),(15,16)]
// [(4,8),(5,12),(6,10),(7,11),(9,13)]
// [(1,4),(2,8),(3,6),(5,7),(9,15),(10,12),(11,14),(13,16)]
// [(2,4),(5,8),(6,10),(7,11),(9,12),(13,15)]
// [(3,5),(6,8),(7,10),(9,11),(12,14)]
// [(3,4),(5,6),(7,8),(9,10),(11,12),(13,14)]
template <class _AlgPolicy, class _Compare, class _RandomAccessIterator>
inline void
sort18branchless(_RandomAccessIterator __x1, _RandomAccessIterator __x2, _RandomAccessIterator __x3, _RandomAccessIterator __x4,
                 _RandomAccessIterator __x5, _RandomAccessIterator __x6, _RandomAccessIterator __x7, _RandomAccessIterator __x8,
                 _RandomAccessIterator __x9, _RandomAccessIterator __x10, _RandomAccessIterator __x11, _RandomAccessIterator __x12,
                 _RandomAccessIterator __x13, _RandomAccessIterator __x14, _RandomAccessIterator __x15, _RandomAccessIterator __x16,
                 _RandomAccessIterator __x17, _RandomAccessIterator __x18, _Compare __c) {
    // Layer 1
    cond_swap<_Compare>(__x1, __x2, __c); cond_swap<_Compare>(__x3, __x4, __c); cond_swap<_Compare>(__x5, __x6, __c); cond_swap<_Compare>(__x7, __x8, __c); cond_swap<_Compare>(__x9, __x10, __c); cond_swap<_Compare>(__x11, __x12, __c); cond_swap<_Compare>(__x13, __x14, __c); cond_swap<_Compare>(__x15, __x16, __c); cond_swap<_Compare>(__x17, __x18, __c);
    // Layer 2
    cond_swap<_Compare>(__x1, __x3, __c); cond_swap<_Compare>(__x2, __x4, __c); cond_swap<_Compare>(__x5, __x13, __c); cond_swap<_Compare>(__x6, __x14, __c); cond_swap<_Compare>(__x7, __x9, __c); cond_swap<_Compare>(__x10, __x12, __c); cond_swap<_Compare>(__x15, __x17, __c); cond_swap<_Compare>(__x16, __x18, __c);
    // Layer 3
    cond_swap<_Compare>(__x1, __x15, __c); cond_swap<_Compare>(__x2, __x17, __c); cond_swap<_Compare>(__x3, __x16, __c); cond_swap<_Compare>(__x4, __x18, __c);
    // Layer 4
    cond_swap<_Compare>(__x1, __x7, __c); cond_swap<_Compare>(__x2, __x11, __c); cond_swap<_Compare>(__x3, __x10, __c); cond_swap<_Compare>(__x8, __x17, __c); cond_swap<_Compare>(__x9, __x16, __c); cond_swap<_Compare>(__x12, __x18, __c);
    // Layer 5
    cond_swap<_Compare>(__x2, __x5, __c); cond_swap<_Compare>(__x4, __x10, __c); cond_swap<_Compare>(__x6, __x8, __c); cond_swap<_Compare>(__x9, __x15, __c); cond_swap<_Compare>(__x11, __x13, __c); cond_swap<_Compare>(__x14, __x17, __c);
    // Layer 6
    cond_swap<_Compare>(__x1, __x2, __c); cond_swap<_Compare>(__x3, __x6, __c); cond_swap<_Compare>(__x4, __x14, __c); cond_swap<_Compare>(__x5, __x15, __c); cond_swap<_Compare>(__x8, __x10, __c); cond_swap<_Compare>(__x9, __x11, __c); cond_swap<_Compare>(__x13, __x16, __c); cond_swap<_Compare>(__x17, __x18, __c);
    // Layer 7
    cond_swap<_Compare>(__x2, __x3, __c); cond_swap<_Compare>(__x4, __x6, __c); cond_swap<_Compare>(__x5, __x7, __c); cond_swap<_Compare>(__x12, __x14, __c); cond_swap<_Compare>(__x13, __x15, __c); cond_swap<_Compare>(__x16, __x17, __c);
    // Layer 8
    cond_swap<_Compare>(__x5, __x9, __c); cond_swap<_Compare>(__x6, __x13, __c); cond_swap<_Compare>(__x7, __x11, __c); cond_swap<_Compare>(__x8, __x12, __c); cond_swap<_Compare>(__x10, __x14, __c);
    // Layer 9
    cond_swap<_Compare>(__x2, __x5, __c); cond_swap<_Compare>(__x3, __x9, __c); cond_swap<_Compare>(__x4, __x7, __c); cond_swap<_Compare>(__x6, __x8, __c); cond_swap<_Compare>(__x10, __x16, __c); cond_swap<_Compare>(__x11, __x13, __c); cond_swap<_Compare>(__x12, __x15, __c); cond_swap<_Compare>(__x14, __x17, __c);
    // Layer 10
    cond_swap<_Compare>(__x3, __x5, __c); cond_swap<_Compare>(__x6, __x9, __c); cond_swap<_Compare>(__x7, __x11, __c); cond_swap<_Compare>(__x8, __x12, __c); cond_swap<_Compare>(__x10, __x13, __c); cond_swap<_Compare>(__x14, __x16, __c);
    // Layer 11
    cond_swap<_Compare>(__x4, __x6, __c); cond_swap<_Compare>(__x7, __x9, __c); cond_swap<_Compare>(__x8, __x11, __c); cond_swap<_Compare>(__x10, __x12, __c); cond_swap<_Compare>(__x13, __x15, __c);
    // Layer 12
    cond_swap<_Compare>(__x4, __x5, __c); cond_swap<_Compare>(__x6, __x7, __c); cond_swap<_Compare>(__x8, __x9, __c); cond_swap<_Compare>(__x10, __x11, __c); cond_swap<_Compare>(__x12, __x13, __c); cond_swap<_Compare>(__x14, __x15, __c);
}

// Sorting network for 19 inputs, 85 CEs, 12 layers:
// [(0,12),(1,4),(2,8),(3,5),(6,17),(7,11),(9,14),(10,13),(15,16)]
// [(0,2),(1,7),(3,6),(4,11),(5,17),(8,12),(10,15),(13,16),(14,18)]
// [(3,10),(4,14),(5,15),(6,13),(7,9),(11,17),(16,18)]
// [(0,7),(1,10),(4,6),(9,15),(11,16),(12,17),(13,14)]
// [(0,3),(2,6),(5,7),(8,11),(12,16)]
// [(1,8),(2,9),(3,4),(6,15),(7,13),(10,11),(12,18)]
// [(1,3),(2,5),(6,9),(7,12),(8,10),(11,14),(17,18)]
// [(0,1),(2,3),(4,8),(6,10),(9,12),(14,15),(16,17)]
// [(1,2),(5,8),(6,7),(9,11),(10,13),(14,16),(15,17)]
// [(3,6),(4,5),(7,9),(8,10),(11,12),(13,14),(15,16)]
// [(3,4),(5,6),(7,8),(9,10),(11,13),(12,14)]
// [(2,3),(4,5),(6,7),(8,9),(10,11),(12,13),(14,15)]
template <class _AlgPolicy, class _Compare, class _RandomAccessIterator>
inline void
sort19branchless(_RandomAccessIterator __x1, _RandomAccessIterator __x2, _RandomAccessIterator __x3, _RandomAccessIterator __x4,
                 _RandomAccessIterator __x5, _RandomAccessIterator __x6, _RandomAccessIterator __x7, _RandomAccessIterator __x8,
                 _RandomAccessIterator __x9, _RandomAccessIterator __x10, _RandomAccessIterator __x11, _RandomAccessIterator __x12,
                 _RandomAccessIterator __x13, _RandomAccessIterator __x14, _RandomAccessIterator __x15, _RandomAccessIterator __x16,
                 _RandomAccessIterator __x17, _RandomAccessIterator __x18, _RandomAccessIterator __x19, _Compare __c) {
    // Layer 1
    cond_swap<_Compare>(__x1, __x13, __c); cond_swap<_Compare>(__x2, __x5, __c); cond_swap<_Compare>(__x3, __x9, __c); cond_swap<_Compare>(__x4, __x6, __c); cond_swap<_Compare>(__x7, __x18, __c); cond_swap<_Compare>(__x8, __x12, __c); cond_swap<_Compare>(__x10, __x15, __c); cond_swap<_Compare>(__x11, __x14, __c); cond_swap<_Compare>(__x16, __x17, __c);
    // Layer 2
    cond_swap<_Compare>(__x1, __x3, __c); cond_swap<_Compare>(__x2, __x8, __c); cond_swap<_Compare>(__x4, __x7, __c); cond_swap<_Compare>(__x5, __x12, __c); cond_swap<_Compare>(__x6, __x18, __c); cond_swap<_Compare>(__x9, __x13, __c); cond_swap<_Compare>(__x11, __x16, __c); cond_swap<_Compare>(__x14, __x17, __c); cond_swap<_Compare>(__x15, __x19, __c);
    // Layer 3
    cond_swap<_Compare>(__x4, __x11, __c); cond_swap<_Compare>(__x5, __x15, __c); cond_swap<_Compare>(__x6, __x16, __c); cond_swap<_Compare>(__x7, __x14, __c); cond_swap<_Compare>(__x8, __x10, __c); cond_swap<_Compare>(__x12, __x18, __c); cond_swap<_Compare>(__x17, __x19, __c);
    // Layer 4
    cond_swap<_Compare>(__x1, __x8, __c); cond_swap<_Compare>(__x2, __x11, __c); cond_swap<_Compare>(__x5, __x7, __c); cond_swap<_Compare>(__x10, __x16, __c); cond_swap<_Compare>(__x12, __x17, __c); cond_swap<_Compare>(__x13, __x18, __c); cond_swap<_Compare>(__x14, __x15, __c);
    // Layer 5
    cond_swap<_Compare>(__x1, __x4, __c); cond_swap<_Compare>(__x3, __x7, __c); cond_swap<_Compare>(__x6, __x8, __c); cond_swap<_Compare>(__x9, __x12, __c); cond_swap<_Compare>(__x13, __x17, __c);
    // Layer 6
    cond_swap<_Compare>(__x2, __x9, __c); cond_swap<_Compare>(__x3, __x10, __c); cond_swap<_Compare>(__x4, __x5, __c); cond_swap<_Compare>(__x7, __x16, __c); cond_swap<_Compare>(__x8, __x14, __c); cond_swap<_Compare>(__x11, __x12, __c); cond_swap<_Compare>(__x13, __x19, __c);
    // Layer 7
    cond_swap<_Compare>(__x2, __x4, __c); cond_swap<_Compare>(__x3, __x6, __c); cond_swap<_Compare>(__x7, __x10, __c); cond_swap<_Compare>(__x8, __x13, __c); cond_swap<_Compare>(__x9, __x11, __c); cond_swap<_Compare>(__x12, __x15, __c); cond_swap<_Compare>(__x18, __x19, __c);
    // Layer 8
    cond_swap<_Compare>(__x1, __x2, __c); cond_swap<_Compare>(__x3, __x4, __c); cond_swap<_Compare>(__x5, __x9, __c); cond_swap<_Compare>(__x7, __x11, __c); cond_swap<_Compare>(__x10, __x13, __c); cond_swap<_Compare>(__x15, __x16, __c); cond_swap<_Compare>(__x17, __x18, __c);
    // Layer 9
    cond_swap<_Compare>(__x2, __x3, __c); cond_swap<_Compare>(__x6, __x9, __c); cond_swap<_Compare>(__x7, __x8, __c); cond_swap<_Compare>(__x10, __x12, __c); cond_swap<_Compare>(__x11, __x14, __c); cond_swap<_Compare>(__x15, __x17, __c); cond_swap<_Compare>(__x16, __x18, __c);
    // Layer 10
    cond_swap<_Compare>(__x4, __x7, __c); cond_swap<_Compare>(__x5, __x6, __c); cond_swap<_Compare>(__x8, __x10, __c); cond_swap<_Compare>(__x9, __x11, __c); cond_swap<_Compare>(__x12, __x13, __c); cond_swap<_Compare>(__x14, __x15, __c); cond_swap<_Compare>(__x16, __x17, __c);
    // Layer 11
    cond_swap<_Compare>(__x4, __x5, __c); cond_swap<_Compare>(__x6, __x7, __c); cond_swap<_Compare>(__x8, __x9, __c); cond_swap<_Compare>(__x10, __x11, __c); cond_swap<_Compare>(__x12, __x14, __c); cond_swap<_Compare>(__x13, __x15, __c);
    // Layer 12
    cond_swap<_Compare>(__x3, __x4, __c); cond_swap<_Compare>(__x5, __x6, __c); cond_swap<_Compare>(__x7, __x8, __c); cond_swap<_Compare>(__x9, __x10, __c); cond_swap<_Compare>(__x11, __x12, __c); cond_swap<_Compare>(__x13, __x14, __c); cond_swap<_Compare>(__x15, __x16, __c);
}

// Sorting network for 20 inputs, 91 CEs, 12 layers:
// [(0,3),(1,7),(2,5),(4,8),(6,9),(10,13),(11,15),(12,18),(14,17),(16,19)]
// [(0,14),(1,11),(2,16),(3,17),(4,12),(5,19),(6,10),(7,15),(8,18),(9,13)]
// [(0,4),(1,2),(3,8),(5,7),(11,16),(12,14),(15,19),(17,18)]
// [(1,6),(2,12),(3,5),(4,11),(7,17),(8,15),(13,18),(14,16)]
// [(0,1),(2,6),(7,10),(9,12),(13,17),(18,19)]
// [(1,6),(5,9),(7,11),(8,12),(10,14),(13,18)]             // Ripetizione (1,6)
// [(3,5),(4,7),(8,10),(9,11),(12,15),(14,16)]
// [(1,3),(2,4),(5,7),(6,10),(9,13),(12,14),(15,17),(16,18)]
// [(1,2),(3,4),(6,7),(8,9),(10,11),(12,13),(15,16),(17,18)]
// [(2,3),(4,6),(5,8),(7,9),(10,12),(11,14),(13,15),(16,17)]
// [(4,5),(6,8),(7,10),(9,12),(11,13),(14,15)]
// [(3,4),(5,6),(7,8),(9,10),(11,12),(13,14),(15,16)]
template <class _AlgPolicy, class _Compare, class _RandomAccessIterator>
inline void
sort20branchless(_RandomAccessIterator __x1, _RandomAccessIterator __x2, _RandomAccessIterator __x3, _RandomAccessIterator __x4,
                 _RandomAccessIterator __x5, _RandomAccessIterator __x6, _RandomAccessIterator __x7, _RandomAccessIterator __x8,
                 _RandomAccessIterator __x9, _RandomAccessIterator __x10, _RandomAccessIterator __x11, _RandomAccessIterator __x12,
                 _RandomAccessIterator __x13, _RandomAccessIterator __x14, _RandomAccessIterator __x15, _RandomAccessIterator __x16,
                 _RandomAccessIterator __x17, _RandomAccessIterator __x18, _RandomAccessIterator __x19, _RandomAccessIterator __x20,
                 _Compare __c) {
    // Layer 1
    cond_swap<_Compare>(__x1, __x4, __c); cond_swap<_Compare>(__x2, __x8, __c); cond_swap<_Compare>(__x3, __x6, __c); cond_swap<_Compare>(__x5, __x9, __c); cond_swap<_Compare>(__x7, __x10, __c); cond_swap<_Compare>(__x11, __x14, __c); cond_swap<_Compare>(__x12, __x16, __c); cond_swap<_Compare>(__x13, __x19, __c); cond_swap<_Compare>(__x15, __x18, __c); cond_swap<_Compare>(__x17, __x20, __c);
    // Layer 2
    cond_swap<_Compare>(__x1, __x15, __c); cond_swap<_Compare>(__x2, __x12, __c); cond_swap<_Compare>(__x3, __x17, __c); cond_swap<_Compare>(__x4, __x18, __c); cond_swap<_Compare>(__x5, __x13, __c); cond_swap<_Compare>(__x6, __x20, __c); cond_swap<_Compare>(__x7, __x11, __c); cond_swap<_Compare>(__x8, __x16, __c); cond_swap<_Compare>(__x9, __x19, __c); cond_swap<_Compare>(__x10, __x14, __c);
    // Layer 3
    cond_swap<_Compare>(__x1, __x5, __c); cond_swap<_Compare>(__x2, __x3, __c); cond_swap<_Compare>(__x4, __x9, __c); cond_swap<_Compare>(__x6, __x8, __c); cond_swap<_Compare>(__x12, __x17, __c); cond_swap<_Compare>(__x13, __x15, __c); cond_swap<_Compare>(__x16, __x20, __c); cond_swap<_Compare>(__x18, __x19, __c);
    // Layer 4
    cond_swap<_Compare>(__x2, __x7, __c); cond_swap<_Compare>(__x3, __x13, __c); cond_swap<_Compare>(__x4, __x6, __c); cond_swap<_Compare>(__x5, __x12, __c); cond_swap<_Compare>(__x8, __x18, __c); cond_swap<_Compare>(__x9, __x16, __c); cond_swap<_Compare>(__x14, __x19, __c); cond_swap<_Compare>(__x15, __x17, __c);
    // Layer 5
    cond_swap<_Compare>(__x1, __x2, __c); cond_swap<_Compare>(__x3, __x7, __c); cond_swap<_Compare>(__x8, __x11, __c); cond_swap<_Compare>(__x10, __x13, __c); cond_swap<_Compare>(__x14, __x18, __c); cond_swap<_Compare>(__x19, __x20, __c);
    // Layer 6
    cond_swap<_Compare>(__x2, __x7, __c); cond_swap<_Compare>(__x6, __x10, __c); cond_swap<_Compare>(__x8, __x12, __c); cond_swap<_Compare>(__x9, __x13, __c); cond_swap<_Compare>(__x11, __x15, __c); cond_swap<_Compare>(__x14, __x19, __c);
    // Layer 7
    cond_swap<_Compare>(__x4, __x6, __c); cond_swap<_Compare>(__x5, __x8, __c); cond_swap<_Compare>(__x9, __x11, __c); cond_swap<_Compare>(__x10, __x12, __c); cond_swap<_Compare>(__x13, __x16, __c); cond_swap<_Compare>(__x15, __x17, __c);
    // Layer 8
    cond_swap<_Compare>(__x2, __x4, __c); cond_swap<_Compare>(__x3, __x5, __c); cond_swap<_Compare>(__x6, __x8, __c); cond_swap<_Compare>(__x7, __x11, __c); cond_swap<_Compare>(__x10, __x14, __c); cond_swap<_Compare>(__x13, __x15, __c); cond_swap<_Compare>(__x16, __x18, __c); cond_swap<_Compare>(__x17, __x19, __c);
    // Layer 9
    cond_swap<_Compare>(__x2, __x3, __c); cond_swap<_Compare>(__x4, __x5, __c); cond_swap<_Compare>(__x7, __x8, __c); cond_swap<_Compare>(__x9, __x10, __c); cond_swap<_Compare>(__x11, __x12, __c); cond_swap<_Compare>(__x13, __x14, __c); cond_swap<_Compare>(__x16, __x17, __c); cond_swap<_Compare>(__x18, __x19, __c);
    // Layer 10
    cond_swap<_Compare>(__x3, __x4, __c); cond_swap<_Compare>(__x5, __x7, __c); cond_swap<_Compare>(__x6, __x9, __c); cond_swap<_Compare>(__x8, __x10, __c); cond_swap<_Compare>(__x11, __x13, __c); cond_swap<_Compare>(__x12, __x15, __c); cond_swap<_Compare>(__x14, __x16, __c); cond_swap<_Compare>(__x17, __x18, __c);
    // Layer 11
    cond_swap<_Compare>(__x5, __x6, __c); cond_swap<_Compare>(__x7, __x9, __c); cond_swap<_Compare>(__x8, __x11, __c); cond_swap<_Compare>(__x10, __x13, __c); cond_swap<_Compare>(__x12, __x14, __c); cond_swap<_Compare>(__x15, __x16, __c);
    // Layer 12
    cond_swap<_Compare>(__x4, __x5, __c); cond_swap<_Compare>(__x6, __x7, __c); cond_swap<_Compare>(__x8, __x9, __c); cond_swap<_Compare>(__x10, __x11, __c); cond_swap<_Compare>(__x12, __x13, __c); cond_swap<_Compare>(__x14, __x15, __c); cond_swap<_Compare>(__x16, __x17, __c);
}

// Sorting network for 21 inputs, 99 CEs, 15 layers:
// [(0,1),(2,3),(4,5),(6,7),(8,9),(10,11),(12,13),(14,15),(16,17),(18,19)]
// [(0,2),(1,3),(4,6),(5,7),(8,10),(9,11),(12,14),(13,15),(16,18),(17,19)]
// [(0,8),(1,9),(2,10),(3,11),(4,12),(5,13),(6,14),(7,15)]
// [(0,4),(1,5),(3,7),(6,20),(8,12),(9,13),(10,14),(15,19)]
// [(2,6),(3,18),(7,20)]
// [(2,16),(3,6),(5,18),(7,17),(11,20)]
// [(0,2),(3,8),(6,12),(7,10),(9,16),(11,15),(13,17),(14,18),(19,20)]
// [(1,7),(2,3),(4,9),(10,11),(13,16),(15,18),(17,19)]
// [(1,4),(5,10),(6,13),(7,8),(11,14),(12,16),(15,17),(18,19)]
// [(1,2),(3,4),(5,6),(10,12),(11,13),(14,16),(17,18)]
// [(2,3),(4,5),(6,9),(10,11),(12,13),(14,15),(16,17)]
// [(6,7),(8,9),(15,16)]
// [(4,6),(7,8),(9,12),(13,15)]
// [(3,4),(5,7),(8,10),(9,11),(12,14)]
// [(5,6),(7,8),(9,10),(11,12),(13,14)]
template <class _AlgPolicy, class _Compare, class _RandomAccessIterator>
inline void
sort21branchless(_RandomAccessIterator __x1, _RandomAccessIterator __x2, _RandomAccessIterator __x3, _RandomAccessIterator __x4,
                 _RandomAccessIterator __x5, _RandomAccessIterator __x6, _RandomAccessIterator __x7, _RandomAccessIterator __x8,
                 _RandomAccessIterator __x9, _RandomAccessIterator __x10, _RandomAccessIterator __x11, _RandomAccessIterator __x12,
                 _RandomAccessIterator __x13, _RandomAccessIterator __x14, _RandomAccessIterator __x15, _RandomAccessIterator __x16,
                 _RandomAccessIterator __x17, _RandomAccessIterator __x18, _RandomAccessIterator __x19, _RandomAccessIterator __x20,
                 _RandomAccessIterator __x21, _Compare __c) {
    // Layer 1
    cond_swap<_Compare>(__x1, __x2, __c); cond_swap<_Compare>(__x3, __x4, __c); cond_swap<_Compare>(__x5, __x6, __c); cond_swap<_Compare>(__x7, __x8, __c); cond_swap<_Compare>(__x9, __x10, __c); cond_swap<_Compare>(__x11, __x12, __c); cond_swap<_Compare>(__x13, __x14, __c); cond_swap<_Compare>(__x15, __x16, __c); cond_swap<_Compare>(__x17, __x18, __c); cond_swap<_Compare>(__x19, __x20, __c);
    // Layer 2
    cond_swap<_Compare>(__x1, __x3, __c); cond_swap<_Compare>(__x2, __x4, __c); cond_swap<_Compare>(__x5, __x7, __c); cond_swap<_Compare>(__x6, __x8, __c); cond_swap<_Compare>(__x9, __x11, __c); cond_swap<_Compare>(__x10, __x12, __c); cond_swap<_Compare>(__x13, __x15, __c); cond_swap<_Compare>(__x14, __x16, __c); cond_swap<_Compare>(__x17, __x19, __c); cond_swap<_Compare>(__x18, __x20, __c);
    // Layer 3
    cond_swap<_Compare>(__x1, __x9, __c); cond_swap<_Compare>(__x2, __x10, __c); cond_swap<_Compare>(__x3, __x11, __c); cond_swap<_Compare>(__x4, __x12, __c); cond_swap<_Compare>(__x5, __x13, __c); cond_swap<_Compare>(__x6, __x14, __c); cond_swap<_Compare>(__x7, __x15, __c); cond_swap<_Compare>(__x8, __x16, __c);
    // Layer 4
    cond_swap<_Compare>(__x1, __x5, __c); cond_swap<_Compare>(__x2, __x6, __c); cond_swap<_Compare>(__x4, __x8, __c); cond_swap<_Compare>(__x7, __x21, __c); cond_swap<_Compare>(__x9, __x13, __c); cond_swap<_Compare>(__x10, __x14, __c); cond_swap<_Compare>(__x11, __x15, __c); cond_swap<_Compare>(__x16, __x20, __c);
    // Layer 5
    cond_swap<_Compare>(__x3, __x7, __c); cond_swap<_Compare>(__x4, __x19, __c); cond_swap<_Compare>(__x8, __x21, __c);
    // Layer 6
    cond_swap<_Compare>(__x3, __x17, __c); cond_swap<_Compare>(__x4, __x7, __c); cond_swap<_Compare>(__x6, __x19, __c); cond_swap<_Compare>(__x8, __x18, __c); cond_swap<_Compare>(__x12, __x21, __c);
    // Layer 7
    cond_swap<_Compare>(__x1, __x3, __c); cond_swap<_Compare>(__x4, __x9, __c); cond_swap<_Compare>(__x7, __x13, __c); cond_swap<_Compare>(__x8, __x11, __c); cond_swap<_Compare>(__x10, __x17, __c); cond_swap<_Compare>(__x12, __x16, __c); cond_swap<_Compare>(__x14, __x18, __c); cond_swap<_Compare>(__x15, __x19, __c); cond_swap<_Compare>(__x20, __x21, __c);
    // Layer 8
    cond_swap<_Compare>(__x2, __x8, __c); cond_swap<_Compare>(__x3, __x4, __c); cond_swap<_Compare>(__x5, __x10, __c); cond_swap<_Compare>(__x11, __x12, __c); cond_swap<_Compare>(__x14, __x17, __c); cond_swap<_Compare>(__x16, __x19, __c); cond_swap<_Compare>(__x18, __x20, __c);
    // Layer 9
    cond_swap<_Compare>(__x2, __x5, __c); cond_swap<_Compare>(__x6, __x11, __c); cond_swap<_Compare>(__x7, __x14, __c); cond_swap<_Compare>(__x8, __x9, __c); cond_swap<_Compare>(__x12, __x15, __c); cond_swap<_Compare>(__x13, __x17, __c); cond_swap<_Compare>(__x16, __x18, __c); cond_swap<_Compare>(__x19, __x20, __c);
    // Layer 10
    cond_swap<_Compare>(__x2, __x3, __c); cond_swap<_Compare>(__x4, __x5, __c); cond_swap<_Compare>(__x6, __x7, __c); cond_swap<_Compare>(__x11, __x13, __c); cond_swap<_Compare>(__x12, __x14, __c); cond_swap<_Compare>(__x15, __x17, __c); cond_swap<_Compare>(__x18, __x19, __c);
    // Layer 11
    cond_swap<_Compare>(__x3, __x4, __c); cond_swap<_Compare>(__x5, __x6, __c); cond_swap<_Compare>(__x7, __x10, __c); cond_swap<_Compare>(__x11, __x12, __c); cond_swap<_Compare>(__x13, __x14, __c); cond_swap<_Compare>(__x15, __x16, __c); cond_swap<_Compare>(__x17, __x18, __c);
    // Layer 12
    cond_swap<_Compare>(__x7, __x8, __c); cond_swap<_Compare>(__x9, __x10, __c); cond_swap<_Compare>(__x16, __x17, __c);
    // Layer 13
    cond_swap<_Compare>(__x5, __x7, __c); cond_swap<_Compare>(__x8, __x9, __c); cond_swap<_Compare>(__x10, __x13, __c); cond_swap<_Compare>(__x14, __x16, __c);
    // Layer 14
    cond_swap<_Compare>(__x4, __x5, __c); cond_swap<_Compare>(__x6, __x8, __c); cond_swap<_Compare>(__x9, __x11, __c); cond_swap<_Compare>(__x10, __x12, __c); cond_swap<_Compare>(__x13, __x15, __c);
    // Layer 15
    cond_swap<_Compare>(__x6, __x7, __c); cond_swap<_Compare>(__x8, __x9, __c); cond_swap<_Compare>(__x10, __x11, __c); cond_swap<_Compare>(__x12, __x13, __c); cond_swap<_Compare>(__x14, __x15, __c);
}

// Sorting network for 22 inputs, 106 CEs, 13 layers (corretta):
template <class _AlgPolicy, class _Compare, class _RandomAccessIterator>
inline void
sort22branchless(_RandomAccessIterator __x1, _RandomAccessIterator __x2, _RandomAccessIterator __x3, _RandomAccessIterator __x4,
                 _RandomAccessIterator __x5, _RandomAccessIterator __x6, _RandomAccessIterator __x7, _RandomAccessIterator __x8,
                 _RandomAccessIterator __x9, _RandomAccessIterator __x10, _RandomAccessIterator __x11, _RandomAccessIterator __x12,
                 _RandomAccessIterator __x13, _RandomAccessIterator __x14, _RandomAccessIterator __x15, _RandomAccessIterator __x16,
                 _RandomAccessIterator __x17, _RandomAccessIterator __x18, _RandomAccessIterator __x19, _RandomAccessIterator __x20,
                 _RandomAccessIterator __x21, _RandomAccessIterator __x22, _Compare __c) {
    // Layer 1
    cond_swap<_Compare>(__x1, __x2, __c); cond_swap<_Compare>(__x3, __x4, __c); cond_swap<_Compare>(__x5, __x6, __c); cond_swap<_Compare>(__x7, __x8, __c); cond_swap<_Compare>(__x9, __x10, __c); cond_swap<_Compare>(__x11, __x12, __c); cond_swap<_Compare>(__x13, __x14, __c); cond_swap<_Compare>(__x15, __x16, __c); cond_swap<_Compare>(__x17, __x18, __c); cond_swap<_Compare>(__x19, __x20, __c); cond_swap<_Compare>(__x21, __x22, __c);
    // Layer 2
    cond_swap<_Compare>(__x1, __x3, __c); cond_swap<_Compare>(__x2, __x4, __c); cond_swap<_Compare>(__x5, __x7, __c); cond_swap<_Compare>(__x6, __x8, __c); cond_swap<_Compare>(__x9, __x13, __c); cond_swap<_Compare>(__x10, __x14, __c); cond_swap<_Compare>(__x15, __x17, __c); cond_swap<_Compare>(__x16, __x18, __c); cond_swap<_Compare>(__x19, __x21, __c); cond_swap<_Compare>(__x20, __x22, __c);
    // Layer 3
    cond_swap<_Compare>(__x1, __x5, __c); cond_swap<_Compare>(__x2, __x6, __c); cond_swap<_Compare>(__x3, __x7, __c); cond_swap<_Compare>(__x4, __x8, __c); cond_swap<_Compare>(__x9, __x11, __c); cond_swap<_Compare>(__x10, __x13, __c); cond_swap<_Compare>(__x12, __x14, __c); cond_swap<_Compare>(__x15, __x19, __c); cond_swap<_Compare>(__x16, __x20, __c); cond_swap<_Compare>(__x17, __x21, __c); cond_swap<_Compare>(__x18, __x22, __c);
    // Layer 4
    cond_swap<_Compare>(__x1, __x15, __c); cond_swap<_Compare>(__x2, __x16, __c); cond_swap<_Compare>(__x3, __x19, __c); cond_swap<_Compare>(__x4, __x20, __c); cond_swap<_Compare>(__x5, __x17, __c); cond_swap<_Compare>(__x6, __x18, __c); cond_swap<_Compare>(__x7, __x21, __c); cond_swap<_Compare>(__x8, __x22, __c); cond_swap<_Compare>(__x10, __x12, __c); cond_swap<_Compare>(__x11, __x13, __c); // Era [9,11], [10,12] - corretto
    // Layer 5
    cond_swap<_Compare>(__x3, __x9, __c); cond_swap<_Compare>(__x4, __x12, __c); cond_swap<_Compare>(__x7, __x10, __c); cond_swap<_Compare>(__x11, __x19, __c); cond_swap<_Compare>(__x13, __x16, __c); cond_swap<_Compare>(__x14, __x20, __c);
    // Layer 6
    cond_swap<_Compare>(__x1, __x3, __c); cond_swap<_Compare>(__x2, __x11, __c); cond_swap<_Compare>(__x4, __x17, __c); cond_swap<_Compare>(__x6, __x19, __c); cond_swap<_Compare>(__x7, __x15, __c); cond_swap<_Compare>(__x8, __x16, __c); cond_swap<_Compare>(__x9, __x13, __c); cond_swap<_Compare>(__x10, __x14, __c); cond_swap<_Compare>(__x12, __x21, __c); cond_swap<_Compare>(__x20, __x22, __c); // Era [19,21] - corretto
    // Layer 7
    cond_swap<_Compare>(__x3, __x7, __c); cond_swap<_Compare>(__x4, __x11, __c); cond_swap<_Compare>(__x5, __x9, __c); cond_swap<_Compare>(__x6, __x13, __c); cond_swap<_Compare>(__x10, __x17, __c); cond_swap<_Compare>(__x12, __x19, __c); cond_swap<_Compare>(__x14, __x18, __c); cond_swap<_Compare>(__x16, __x20, __c);
    // Layer 8
    cond_swap<_Compare>(__x2, __x5, __c); cond_swap<_Compare>(__x8, __x14, __c); cond_swap<_Compare>(__x9, __x15, __c); cond_swap<_Compare>(__x10, __x13, __c); cond_swap<_Compare>(__x18, __x21, __c);
    // Layer 9
    cond_swap<_Compare>(__x2, __x3, __c); cond_swap<_Compare>(__x4, __x9, __c); cond_swap<_Compare>(__x5, __x7, __c); cond_swap<_Compare>(__x8, __x12, __c); cond_swap<_Compare>(__x11, __x15, __c); cond_swap<_Compare>(__x14, __x19, __c); cond_swap<_Compare>(__x16, __x18, __c); cond_swap<_Compare>(__x20, __x21, __c);
    // Layer 10
    cond_swap<_Compare>(__x3, __x5, __c); cond_swap<_Compare>(__x6, __x11, __c); cond_swap<_Compare>(__x8, __x10, __c); cond_swap<_Compare>(__x12, __x17, __c); cond_swap<_Compare>(__x13, __x15, __c); cond_swap<_Compare>(__x18, __x20, __c);
    // Layer 11
    cond_swap<_Compare>(__x6, __x7, __c); cond_swap<_Compare>(__x8, __x9, __c); cond_swap<_Compare>(__x10, __x12, __c); // Era [9,11] - corretto
    cond_swap<_Compare>(__x11, __x13, __c); cond_swap<_Compare>(__x14, __x15, __c); cond_swap<_Compare>(__x16, __x17, __c);
    // Layer 12
    cond_swap<_Compare>(__x4, __x6, __c); cond_swap<_Compare>(__x7, __x8, __c); cond_swap<_Compare>(__x9, __x11, __c); // Era [8,10] - corretto
    cond_swap<_Compare>(__x10, __x13, __c); cond_swap<_Compare>(__x12, __x14, __c); cond_swap<_Compare>(__x15, __x16, __c); cond_swap<_Compare>(__x17, __x19, __c); // Era [16,18] - corretto
    // Layer 13
    cond_swap<_Compare>(__x4, __x5, __c); cond_swap<_Compare>(__x6, __x7, __c); cond_swap<_Compare>(__x8, __x9, __c); cond_swap<_Compare>(__x10, __x11, __c); cond_swap<_Compare>(__x12, __x13, __c); cond_swap<_Compare>(__x14, __x15, __c); cond_swap<_Compare>(__x16, __x17, __c); cond_swap<_Compare>(__x18, __x19, __c);
}

// Sorting network for 23 inputs, 114 CEs, 14 layers:
// [(0,1),(2,3),(4,5),(6,7),(8,9),(10,11),(12,13),(14,15),(16,17),(18,19),(20,21)]
// [(0,2),(1,3),(4,6),(5,7),(8,10),(9,11),(12,14),(13,15),(16,18),(17,19),(21,22)]
// [(0,4),(1,5),(2,6),(3,7),(8,12),(9,13),(10,14),(11,15),(17,21),(18,20),(19,22)]
// [(0,8),(1,9),(2,10),(3,11),(4,12),(5,13),(6,14),(7,15)]
// [(1,2),(5,18),(7,19),(9,16),(10,21),(12,20),(15,22)]
// [(5,9),(6,7),(10,18),(11,21),(12,17),(13,20),(14,15)]
// [(3,17),(6,16),(7,14),(8,12),(15,19),(20,21)]
// [(3,4),(5,8),(6,10),(9,12),(13,16),(14,15),(17,18),(19,21)]
// [(0,5),(1,8),(2,12),(3,9),(4,10),(7,13),(11,17),(14,16),(18,20)]
// [(2,6),(3,5),(4,8),(7,11),(10,12),(13,18),(14,17),(15,20)]
// [(1,3),(2,5),(6,9),(7,10),(11,13),(12,14),(15,18),(16,17),(19,20)]
// [(2,3),(4,6),(8,9),(11,12),(13,14),(15,16),(17,19)]
// [(3,4),(5,6),(7,8),(9,10),(12,13),(14,15),(17,18)]
// [(4,5),(6,7),(8,9),(10,11),(16,17)]
template <class _AlgPolicy, class _Compare, class _RandomAccessIterator>
inline void
sort23branchless(_RandomAccessIterator __x1, _RandomAccessIterator __x2, _RandomAccessIterator __x3, _RandomAccessIterator __x4,
                 _RandomAccessIterator __x5, _RandomAccessIterator __x6, _RandomAccessIterator __x7, _RandomAccessIterator __x8,
                 _RandomAccessIterator __x9, _RandomAccessIterator __x10, _RandomAccessIterator __x11, _RandomAccessIterator __x12,
                 _RandomAccessIterator __x13, _RandomAccessIterator __x14, _RandomAccessIterator __x15, _RandomAccessIterator __x16,
                 _RandomAccessIterator __x17, _RandomAccessIterator __x18, _RandomAccessIterator __x19, _RandomAccessIterator __x20,
                 _RandomAccessIterator __x21, _RandomAccessIterator __x22, _RandomAccessIterator __x23, _Compare __c) {
    // Layer 1
    cond_swap<_Compare>(__x1, __x2, __c); cond_swap<_Compare>(__x3, __x4, __c); cond_swap<_Compare>(__x5, __x6, __c); cond_swap<_Compare>(__x7, __x8, __c); cond_swap<_Compare>(__x9, __x10, __c); cond_swap<_Compare>(__x11, __x12, __c); cond_swap<_Compare>(__x13, __x14, __c); cond_swap<_Compare>(__x15, __x16, __c); cond_swap<_Compare>(__x17, __x18, __c); cond_swap<_Compare>(__x19, __x20, __c); cond_swap<_Compare>(__x21, __x22, __c);
    // Layer 2
    cond_swap<_Compare>(__x1, __x3, __c); cond_swap<_Compare>(__x2, __x4, __c); cond_swap<_Compare>(__x5, __x7, __c); cond_swap<_Compare>(__x6, __x8, __c); cond_swap<_Compare>(__x9, __x11, __c); cond_swap<_Compare>(__x10, __x12, __c); cond_swap<_Compare>(__x13, __x15, __c); cond_swap<_Compare>(__x14, __x16, __c); cond_swap<_Compare>(__x17, __x19, __c); cond_swap<_Compare>(__x18, __x20, __c); cond_swap<_Compare>(__x22, __x23, __c);
    // Layer 3
    cond_swap<_Compare>(__x1, __x5, __c); cond_swap<_Compare>(__x2, __x6, __c); cond_swap<_Compare>(__x3, __x7, __c); cond_swap<_Compare>(__x4, __x8, __c); cond_swap<_Compare>(__x9, __x13, __c); cond_swap<_Compare>(__x10, __x14, __c); cond_swap<_Compare>(__x11, __x15, __c); cond_swap<_Compare>(__x12, __x16, __c); cond_swap<_Compare>(__x18, __x22, __c); cond_swap<_Compare>(__x19, __x21, __c); cond_swap<_Compare>(__x20, __x23, __c);
    // Layer 4
    cond_swap<_Compare>(__x1, __x9, __c); cond_swap<_Compare>(__x2, __x10, __c); cond_swap<_Compare>(__x3, __x11, __c); cond_swap<_Compare>(__x4, __x12, __c); cond_swap<_Compare>(__x5, __x13, __c); cond_swap<_Compare>(__x6, __x14, __c); cond_swap<_Compare>(__x7, __x15, __c); cond_swap<_Compare>(__x8, __x16, __c);
    // Layer 5
    cond_swap<_Compare>(__x2, __x3, __c); cond_swap<_Compare>(__x6, __x19, __c); cond_swap<_Compare>(__x8, __x20, __c); cond_swap<_Compare>(__x10, __x17, __c); cond_swap<_Compare>(__x11, __x22, __c); cond_swap<_Compare>(__x13, __x21, __c); cond_swap<_Compare>(__x16, __x23, __c);
    // Layer 6
    cond_swap<_Compare>(__x6, __x10, __c); cond_swap<_Compare>(__x7, __x8, __c); cond_swap<_Compare>(__x11, __x19, __c); cond_swap<_Compare>(__x12, __x22, __c); cond_swap<_Compare>(__x13, __x18, __c); cond_swap<_Compare>(__x14, __x21, __c); cond_swap<_Compare>(__x15, __x16, __c);
    // Layer 7
    cond_swap<_Compare>(__x4, __x18, __c); cond_swap<_Compare>(__x7, __x17, __c); cond_swap<_Compare>(__x8, __x15, __c); cond_swap<_Compare>(__x9, __x13, __c); cond_swap<_Compare>(__x16, __x20, __c); cond_swap<_Compare>(__x21, __x22, __c);
    // Layer 8
    cond_swap<_Compare>(__x4, __x5, __c); cond_swap<_Compare>(__x6, __x9, __c); cond_swap<_Compare>(__x7, __x11, __c); cond_swap<_Compare>(__x10, __x13, __c); cond_swap<_Compare>(__x14, __x17, __c); cond_swap<_Compare>(__x15, __x16, __c); cond_swap<_Compare>(__x18, __x19, __c); cond_swap<_Compare>(__x20, __x22, __c);
    // Layer 9
    cond_swap<_Compare>(__x1, __x6, __c); cond_swap<_Compare>(__x2, __x9, __c); cond_swap<_Compare>(__x3, __x13, __c); cond_swap<_Compare>(__x4, __x10, __c); cond_swap<_Compare>(__x5, __x11, __c); cond_swap<_Compare>(__x8, __x14, __c); cond_swap<_Compare>(__x12, __x18, __c); cond_swap<_Compare>(__x15, __x17, __c); cond_swap<_Compare>(__x19, __x21, __c);
    // Layer 10
    cond_swap<_Compare>(__x3, __x7, __c); cond_swap<_Compare>(__x4, __x6, __c); cond_swap<_Compare>(__x5, __x9, __c); cond_swap<_Compare>(__x8, __x12, __c); cond_swap<_Compare>(__x11, __x13, __c); cond_swap<_Compare>(__x14, __x19, __c); cond_swap<_Compare>(__x15, __x18, __c); cond_swap<_Compare>(__x16, __x21, __c);
    // Layer 11
    cond_swap<_Compare>(__x2, __x4, __c); cond_swap<_Compare>(__x3, __x6, __c); cond_swap<_Compare>(__x7, __x10, __c); cond_swap<_Compare>(__x8, __x11, __c); cond_swap<_Compare>(__x12, __x14, __c); cond_swap<_Compare>(__x13, __x15, __c); cond_swap<_Compare>(__x16, __x19, __c); cond_swap<_Compare>(__x17, __x18, __c); cond_swap<_Compare>(__x20, __x21, __c);
    // Layer 12
    cond_swap<_Compare>(__x3, __x4, __c); cond_swap<_Compare>(__x5, __x7, __c); cond_swap<_Compare>(__x9, __x10, __c); cond_swap<_Compare>(__x12, __x13, __c); cond_swap<_Compare>(__x14, __x15, __c); cond_swap<_Compare>(__x16, __x17, __c); cond_swap<_Compare>(__x18, __x20, __c);
    // Layer 13
    cond_swap<_Compare>(__x4, __x5, __c); cond_swap<_Compare>(__x6, __x7, __c); cond_swap<_Compare>(__x8, __x9, __c); cond_swap<_Compare>(__x10, __x11, __c); cond_swap<_Compare>(__x13, __x14, __c); cond_swap<_Compare>(__x15, __x16, __c); cond_swap<_Compare>(__x18, __x19, __c);
    // Layer 14
    cond_swap<_Compare>(__x5, __x6, __c); cond_swap<_Compare>(__x7, __x8, __c); cond_swap<_Compare>(__x9, __x10, __c); cond_swap<_Compare>(__x11, __x12, __c); cond_swap<_Compare>(__x17, __x18, __c);
}

// Sorting network for 24 inputs, 120 CEs, 13 layers:
// [(0,20),(1,12),(2,16),(3,23),(4,6),(5,10),(7,21),(8,14),(9,15),(11,22),(13,18),(17,19)]
// [(0,3),(1,11),(2,7),(4,17),(5,13),(6,19),(8,9),(10,18),(12,22),(14,15),(16,21),(20,23)]
// [(0,1),(2,4),(3,12),(5,8),(6,9),(7,10),(11,20),(13,16),(14,17),(15,18),(19,21),(22,23)]
// [(2,5),(4,8),(6,11),(7,14),(9,16),(12,17),(15,19),(18,21)]
// [(1,8),(3,14),(4,7),(9,20),(10,12),(11,13),(15,22),(16,19)]
// [(0,7),(1,5),(3,4),(6,11),(8,15),(9,14),(10,13),(12,17),(16,23),(18,22),(19,20)]
// [(0,2),(1,6),(4,7),(5,9),(8,10),(13,15),(14,18),(16,19),(17,22),(21,23)]
// [(2,3),(4,5),(6,8),(7,9),(10,11),(12,13),(14,16),(15,17),(18,19),(20,21)]
// [(1,2),(3,6),(4,10),(7,8),(9,11),(12,14),(13,19),(15,16),(17,20),(21,22)]
// [(2,3),(5,10),(6,7),(8,9),(13,18),(14,15),(16,17),(20,21)]
// [(3,4),(5,7),(10,12),(11,13),(16,18),(19,20)]
// [(4,6),(8,10),(9,12),(11,14),(13,15),(17,19)]
// [(5,6),(7,8),(9,10),(11,12),(13,14),(15,16),(17,18)]
template <class _AlgPolicy, class _Compare, class _RandomAccessIterator>
inline void
sort24branchless(_RandomAccessIterator __x1, _RandomAccessIterator __x2, _RandomAccessIterator __x3, _RandomAccessIterator __x4,
                 _RandomAccessIterator __x5, _RandomAccessIterator __x6, _RandomAccessIterator __x7, _RandomAccessIterator __x8,
                 _RandomAccessIterator __x9, _RandomAccessIterator __x10, _RandomAccessIterator __x11, _RandomAccessIterator __x12,
                 _RandomAccessIterator __x13, _RandomAccessIterator __x14, _RandomAccessIterator __x15, _RandomAccessIterator __x16,
                 _RandomAccessIterator __x17, _RandomAccessIterator __x18, _RandomAccessIterator __x19, _RandomAccessIterator __x20,
                 _RandomAccessIterator __x21, _RandomAccessIterator __x22, _RandomAccessIterator __x23, _RandomAccessIterator __x24,
                 _Compare __c) {
    // Layer 1
    cond_swap<_Compare>(__x1, __x21, __c); cond_swap<_Compare>(__x2, __x13, __c); cond_swap<_Compare>(__x3, __x17, __c); cond_swap<_Compare>(__x4, __x24, __c); cond_swap<_Compare>(__x5, __x7, __c); cond_swap<_Compare>(__x6, __x11, __c); cond_swap<_Compare>(__x8, __x22, __c); cond_swap<_Compare>(__x9, __x15, __c); cond_swap<_Compare>(__x10, __x16, __c); cond_swap<_Compare>(__x12, __x23, __c); cond_swap<_Compare>(__x14, __x19, __c); cond_swap<_Compare>(__x18, __x20, __c);
    // Layer 2
    cond_swap<_Compare>(__x1, __x4, __c); cond_swap<_Compare>(__x2, __x12, __c); cond_swap<_Compare>(__x3, __x8, __c); cond_swap<_Compare>(__x5, __x18, __c); cond_swap<_Compare>(__x6, __x14, __c); cond_swap<_Compare>(__x7, __x20, __c); cond_swap<_Compare>(__x9, __x10, __c); cond_swap<_Compare>(__x11, __x19, __c); cond_swap<_Compare>(__x13, __x23, __c); cond_swap<_Compare>(__x15, __x16, __c); cond_swap<_Compare>(__x17, __x22, __c); cond_swap<_Compare>(__x21, __x24, __c);
    // Layer 3
    cond_swap<_Compare>(__x1, __x2, __c); cond_swap<_Compare>(__x3, __x5, __c); cond_swap<_Compare>(__x4, __x13, __c); cond_swap<_Compare>(__x6, __x9, __c); cond_swap<_Compare>(__x7, __x10, __c); cond_swap<_Compare>(__x8, __x11, __c); cond_swap<_Compare>(__x12, __x21, __c); cond_swap<_Compare>(__x14, __x17, __c); cond_swap<_Compare>(__x15, __x18, __c); cond_swap<_Compare>(__x16, __x19, __c); cond_swap<_Compare>(__x20, __x22, __c); cond_swap<_Compare>(__x23, __x24, __c);
    // Layer 4
    cond_swap<_Compare>(__x3, __x6, __c); cond_swap<_Compare>(__x5, __x9, __c); cond_swap<_Compare>(__x7, __x12, __c); cond_swap<_Compare>(__x8, __x15, __c); cond_swap<_Compare>(__x10, __x17, __c); cond_swap<_Compare>(__x13, __x18, __c); cond_swap<_Compare>(__x16, __x20, __c); cond_swap<_Compare>(__x19, __x22, __c);
    // Layer 5
    cond_swap<_Compare>(__x2, __x9, __c); cond_swap<_Compare>(__x4, __x15, __c); cond_swap<_Compare>(__x5, __x8, __c); cond_swap<_Compare>(__x10, __x21, __c); cond_swap<_Compare>(__x11, __x13, __c); cond_swap<_Compare>(__x12, __x14, __c); cond_swap<_Compare>(__x16, __x23, __c); cond_swap<_Compare>(__x17, __x20, __c);
    // Layer 6
    cond_swap<_Compare>(__x1, __x8, __c); cond_swap<_Compare>(__x2, __x6, __c); cond_swap<_Compare>(__x4, __x5, __c); cond_swap<_Compare>(__x7, __x12, __c); cond_swap<_Compare>(__x9, __x16, __c); cond_swap<_Compare>(__x10, __x15, __c); cond_swap<_Compare>(__x11, __x14, __c); cond_swap<_Compare>(__x13, __x18, __c); cond_swap<_Compare>(__x17, __x24, __c); cond_swap<_Compare>(__x19, __x23, __c); cond_swap<_Compare>(__x20, __x21, __c);
    // Layer 7
    cond_swap<_Compare>(__x1, __x3, __c); cond_swap<_Compare>(__x2, __x7, __c); cond_swap<_Compare>(__x5, __x8, __c); cond_swap<_Compare>(__x6, __x10, __c); cond_swap<_Compare>(__x9, __x11, __c); cond_swap<_Compare>(__x14, __x16, __c); cond_swap<_Compare>(__x15, __x19, __c); cond_swap<_Compare>(__x17, __x20, __c); cond_swap<_Compare>(__x18, __x23, __c); cond_swap<_Compare>(__x22, __x24, __c);
    // Layer 8
    cond_swap<_Compare>(__x3, __x4, __c); cond_swap<_Compare>(__x5, __x6, __c); cond_swap<_Compare>(__x7, __x9, __c); cond_swap<_Compare>(__x8, __x10, __c); cond_swap<_Compare>(__x11, __x12, __c); cond_swap<_Compare>(__x13, __x14, __c); cond_swap<_Compare>(__x15, __x17, __c); cond_swap<_Compare>(__x16, __x18, __c); cond_swap<_Compare>(__x19, __x20, __c); cond_swap<_Compare>(__x21, __x22, __c);
    // Layer 9
    cond_swap<_Compare>(__x2, __x3, __c); cond_swap<_Compare>(__x4, __x7, __c); cond_swap<_Compare>(__x5, __x11, __c); cond_swap<_Compare>(__x8, __x9, __c); cond_swap<_Compare>(__x10, __x12, __c); cond_swap<_Compare>(__x13, __x15, __c); cond_swap<_Compare>(__x14, __x20, __c); cond_swap<_Compare>(__x16, __x17, __c); cond_swap<_Compare>(__x18, __x21, __c); cond_swap<_Compare>(__x22, __x23, __c);
    // Layer 10
    cond_swap<_Compare>(__x3, __x4, __c); cond_swap<_Compare>(__x6, __x11, __c); cond_swap<_Compare>(__x7, __x8, __c); cond_swap<_Compare>(__x9, __x10, __c); cond_swap<_Compare>(__x14, __x19, __c); cond_swap<_Compare>(__x15, __x16, __c); cond_swap<_Compare>(__x17, __x18, __c); cond_swap<_Compare>(__x21, __x22, __c);
    // Layer 11
    cond_swap<_Compare>(__x4, __x5, __c); cond_swap<_Compare>(__x6, __x8, __c); cond_swap<_Compare>(__x11, __x13, __c); cond_swap<_Compare>(__x12, __x14, __c); cond_swap<_Compare>(__x17, __x19, __c); cond_swap<_Compare>(__x20, __x21, __c);
    // Layer 12
    cond_swap<_Compare>(__x5, __x7, __c); cond_swap<_Compare>(__x9, __x11, __c); cond_swap<_Compare>(__x10, __x13, __c); cond_swap<_Compare>(__x12, __x15, __c); cond_swap<_Compare>(__x14, __x16, __c); cond_swap<_Compare>(__x18, __x20, __c);
    // Layer 13
    cond_swap<_Compare>(__x6, __x7, __c); cond_swap<_Compare>(__x8, __x9, __c); cond_swap<_Compare>(__x10, __x11, __c); cond_swap<_Compare>(__x12, __x13, __c); cond_swap<_Compare>(__x14, __x15, __c); cond_swap<_Compare>(__x16, __x17, __c); cond_swap<_Compare>(__x18, __x19, __c);
}


// N=25 (S(25)=130, depth 15)
// Rete: Dal testo fornito
template <class, class _Compare, class _RandomAccessIterator>
inline void
sort25branchless(_RandomAccessIterator __x1, _RandomAccessIterator __x2, _RandomAccessIterator __x3,
                 _RandomAccessIterator __x4, _RandomAccessIterator __x5, _RandomAccessIterator __x6,
                 _RandomAccessIterator __x7, _RandomAccessIterator __x8, _RandomAccessIterator __x9,
                 _RandomAccessIterator __x10, _RandomAccessIterator __x11, _RandomAccessIterator __x12,
                 _RandomAccessIterator __x13, _RandomAccessIterator __x14, _RandomAccessIterator __x15,
                 _RandomAccessIterator __x16, _RandomAccessIterator __x17, _RandomAccessIterator __x18,
                 _RandomAccessIterator __x19, _RandomAccessIterator __x20, _RandomAccessIterator __x21,
                 _RandomAccessIterator __x22, _RandomAccessIterator __x23, _RandomAccessIterator __x24,
                 _RandomAccessIterator __x25,
                 _Compare __c) {
    // Layer 1: [(0,1),(2,3),(4,5),(6,7),(8,9),(10,11),(12,13),(14,15),(16,17),(18,19),(20,21),(22,23)]
    cond_swap<_Compare>(__x1, __x2, __c); cond_swap<_Compare>(__x3, __x4, __c); cond_swap<_Compare>(__x5, __x6, __c);
    cond_swap<_Compare>(__x7, __x8, __c); cond_swap<_Compare>(__x9, __x10, __c); cond_swap<_Compare>(__x11, __x12, __c);
    cond_swap<_Compare>(__x13, __x14, __c); cond_swap<_Compare>(__x15, __x16, __c); cond_swap<_Compare>(__x17, __x18, __c);
    cond_swap<_Compare>(__x19, __x20, __c); cond_swap<_Compare>(__x21, __x22, __c); cond_swap<_Compare>(__x23, __x24, __c);

    // Layer 2: [(0,2),(1,3),(4,6),(5,7),(8,10),(9,11),(12,14),(13,15),(16,18),(17,19),(21,22),(23,24)]
    cond_swap<_Compare>(__x1, __x3, __c); cond_swap<_Compare>(__x2, __x4, __c); cond_swap<_Compare>(__x5, __x7, __c);
    cond_swap<_Compare>(__x6, __x8, __c); cond_swap<_Compare>(__x9, __x11, __c); cond_swap<_Compare>(__x10, __x12, __c);
    cond_swap<_Compare>(__x13, __x15, __c); cond_swap<_Compare>(__x14, __x16, __c); cond_swap<_Compare>(__x17, __x19, __c);
    cond_swap<_Compare>(__x18, __x20, __c); cond_swap<_Compare>(__x22, __x23, __c); cond_swap<_Compare>(__x24, __x25, __c);

    // Layer 3: [(0,4),(1,5),(2,6),(3,7),(8,12),(9,13),(10,14),(11,15),(18,21),(20,23),(22,24)]
    cond_swap<_Compare>(__x1, __x5, __c); cond_swap<_Compare>(__x2, __x6, __c); cond_swap<_Compare>(__x3, __x7, __c);
    cond_swap<_Compare>(__x4, __x8, __c); cond_swap<_Compare>(__x9, __x13, __c); cond_swap<_Compare>(__x10, __x14, __c);
    cond_swap<_Compare>(__x11, __x15, __c); cond_swap<_Compare>(__x12, __x16, __c); cond_swap<_Compare>(__x19, __x22, __c);
    cond_swap<_Compare>(__x21, __x24, __c); cond_swap<_Compare>(__x23, __x25, __c);

    // Layer 4: [(0,8),(1,9),(2,10),(3,11),(4,12),(5,13),(6,14),(7,15),(16,20),(17,22),(19,24),(21,23)]
    cond_swap<_Compare>(__x1, __x9, __c); cond_swap<_Compare>(__x2, __x10, __c); cond_swap<_Compare>(__x3, __x11, __c);
    cond_swap<_Compare>(__x4, __x12, __c); cond_swap<_Compare>(__x5, __x13, __c); cond_swap<_Compare>(__x6, __x14, __c);
    cond_swap<_Compare>(__x7, __x15, __c); cond_swap<_Compare>(__x8, __x16, __c); cond_swap<_Compare>(__x17, __x21, __c);
    cond_swap<_Compare>(__x18, __x23, __c); cond_swap<_Compare>(__x20, __x25, __c); cond_swap<_Compare>(__x22, __x24, __c);

    // Layer 5: [(1,18),(3,21),(5,23),(6,19),(11,14),(15,24)]
    cond_swap<_Compare>(__x2, __x19, __c); cond_swap<_Compare>(__x4, __x22, __c); cond_swap<_Compare>(__x6, __x24, __c);
    cond_swap<_Compare>(__x7, __x20, __c); cond_swap<_Compare>(__x12, __x15, __c); cond_swap<_Compare>(__x16, __x25, __c);

    // Layer 6: [(1,16),(3,17),(6,9),(7,11),(13,19),(14,23)]
    cond_swap<_Compare>(__x2, __x17, __c); cond_swap<_Compare>(__x4, __x18, __c); cond_swap<_Compare>(__x7, __x10, __c);
    cond_swap<_Compare>(__x8, __x12, __c); cond_swap<_Compare>(__x14, __x20, __c); cond_swap<_Compare>(__x15, __x24, __c);

    // Layer 7: [(0,1),(2,16),(3,8),(7,20),(10,13),(11,22),(15,23)]
    cond_swap<_Compare>(__x1, __x2, __c); cond_swap<_Compare>(__x3, __x17, __c); cond_swap<_Compare>(__x4, __x9, __c);
    cond_swap<_Compare>(__x8, __x21, __c); cond_swap<_Compare>(__x11, __x14, __c); cond_swap<_Compare>(__x12, __x23, __c);
    cond_swap<_Compare>(__x16, __x24, __c);

    // Layer 8: [(1,2),(5,10),(7,18),(11,21),(15,20),(19,22)]
    cond_swap<_Compare>(__x2, __x3, __c); cond_swap<_Compare>(__x6, __x11, __c); cond_swap<_Compare>(__x8, __x19, __c);
    cond_swap<_Compare>(__x12, __x22, __c); cond_swap<_Compare>(__x16, __x21, __c); cond_swap<_Compare>(__x20, __x23, __c);

    // Layer 9: [(4,7),(5,6),(9,18),(10,17),(11,12),(13,21),(14,15),(19,20),(22,23)]
    cond_swap<_Compare>(__x5, __x8, __c); cond_swap<_Compare>(__x6, __x7, __c); cond_swap<_Compare>(__x10, __x19, __c);
    cond_swap<_Compare>(__x11, __x18, __c); cond_swap<_Compare>(__x12, __x13, __c); cond_swap<_Compare>(__x14, __x22, __c);
    cond_swap<_Compare>(__x15, __x16, __c); cond_swap<_Compare>(__x20, __x21, __c); cond_swap<_Compare>(__x23, __x24, __c);

    // Layer 10: [(3,4),(7,8),(9,10),(11,16),(12,17),(13,18),(19,21),(20,22)]
    cond_swap<_Compare>(__x4, __x5, __c); cond_swap<_Compare>(__x8, __x9, __c); cond_swap<_Compare>(__x10, __x11, __c);
    cond_swap<_Compare>(__x12, __x17, __c); cond_swap<_Compare>(__x13, __x18, __c); cond_swap<_Compare>(__x14, __x19, __c);
    cond_swap<_Compare>(__x20, __x22, __c); cond_swap<_Compare>(__x21, __x23, __c);

    // Layer 11: [(1,3),(2,4),(5,11),(6,16),(7,9),(8,10),(12,13),(14,19),(15,18)]
    cond_swap<_Compare>(__x2, __x4, __c); cond_swap<_Compare>(__x3, __x5, __c); cond_swap<_Compare>(__x6, __x12, __c);
    cond_swap<_Compare>(__x7, __x17, __c); cond_swap<_Compare>(__x8, __x10, __c); cond_swap<_Compare>(__x9, __x11, __c);
    cond_swap<_Compare>(__x13, __x14, __c); cond_swap<_Compare>(__x15, __x20, __c); cond_swap<_Compare>(__x16, __x19, __c);

    // Layer 12: [(2,3),(5,7),(6,9),(8,11),(10,16),(12,14),(15,17)]
    cond_swap<_Compare>(__x3, __x4, __c); cond_swap<_Compare>(__x6, __x8, __c); cond_swap<_Compare>(__x7, __x10, __c);
    cond_swap<_Compare>(__x9, __x12, __c); cond_swap<_Compare>(__x11, __x17, __c); cond_swap<_Compare>(__x13, __x15, __c);
    cond_swap<_Compare>(__x16, __x18, __c);

    // Layer 13: [(3,5),(4,6),(7,8),(9,11),(10,12),(13,14),(15,16),(17,18)]
    cond_swap<_Compare>(__x4, __x6, __c); cond_swap<_Compare>(__x5, __x7, __c); cond_swap<_Compare>(__x8, __x9, __c);
    cond_swap<_Compare>(__x10, __x12, __c); cond_swap<_Compare>(__x11, __x13, __c); cond_swap<_Compare>(__x14, __x15, __c);
    cond_swap<_Compare>(__x16, __x17, __c); cond_swap<_Compare>(__x18, __x19, __c);

    // Layer 14: [(4,7),(6,8),(9,10),(11,12),(13,15),(14,16),(17,19),(18,21)]
    cond_swap<_Compare>(__x5, __x8, __c); cond_swap<_Compare>(__x7, __x9, __c); cond_swap<_Compare>(__x10, __x11, __c);
    cond_swap<_Compare>(__x12, __x13, __c); cond_swap<_Compare>(__x14, __x16, __c); cond_swap<_Compare>(__x15, __x17, __c);
    cond_swap<_Compare>(__x18, __x20, __c); cond_swap<_Compare>(__x19, __x22, __c);

    // Layer 15: [(4,5),(6,7),(8,9),(10,11),(12,13),(14,15),(16,17),(18,19),(20,21)]
    cond_swap<_Compare>(__x5, __x6, __c); cond_swap<_Compare>(__x7, __x8, __c); cond_swap<_Compare>(__x9, __x10, __c);
    cond_swap<_Compare>(__x11, __x12, __c); cond_swap<_Compare>(__x13, __x14, __c); cond_swap<_Compare>(__x15, __x16, __c);
    cond_swap<_Compare>(__x17, __x18, __c); cond_swap<_Compare>(__x19, __x20, __c); cond_swap<_Compare>(__x21, __x22, __c);
}


// N=26 (S(26)=138, depth 15)
// Rete: Dal testo fornito
template <class, class _Compare, class _RandomAccessIterator>
inline void
sort26branchless(_RandomAccessIterator __x1, _RandomAccessIterator __x2, _RandomAccessIterator __x3,
                 _RandomAccessIterator __x4, _RandomAccessIterator __x5, _RandomAccessIterator __x6,
                 _RandomAccessIterator __x7, _RandomAccessIterator __x8, _RandomAccessIterator __x9,
                 _RandomAccessIterator __x10, _RandomAccessIterator __x11, _RandomAccessIterator __x12,
                 _RandomAccessIterator __x13, _RandomAccessIterator __x14, _RandomAccessIterator __x15,
                 _RandomAccessIterator __x16, _RandomAccessIterator __x17, _RandomAccessIterator __x18,
                 _RandomAccessIterator __x19, _RandomAccessIterator __x20, _RandomAccessIterator __x21,
                 _RandomAccessIterator __x22, _RandomAccessIterator __x23, _RandomAccessIterator __x24,
                 _RandomAccessIterator __x25, _RandomAccessIterator __x26,
                 _Compare __c) {
    // Layer 1: [(0,1),(2,3),(4,5),(6,7),(8,9),(10,11),(12,13),(14,15),(16,17),(18,19),(20,21),(22,23),(24,25)]
    cond_swap<_Compare>(__x1, __x2, __c); cond_swap<_Compare>(__x3, __x4, __c); cond_swap<_Compare>(__x5, __x6, __c); cond_swap<_Compare>(__x7, __x8, __c); cond_swap<_Compare>(__x9, __x10, __c); cond_swap<_Compare>(__x11, __x12, __c); cond_swap<_Compare>(__x13, __x14, __c); cond_swap<_Compare>(__x15, __x16, __c); cond_swap<_Compare>(__x17, __x18, __c); cond_swap<_Compare>(__x19, __x20, __c); cond_swap<_Compare>(__x21, __x22, __c); cond_swap<_Compare>(__x23, __x24, __c); cond_swap<_Compare>(__x25, __x26, __c);
    // Layer 2: [(0,2),(1,3),(4,6),(5,7),(8,10),(9,11),(14,16),(15,17),(18,20),(19,21),(22,24),(23,25)]
    cond_swap<_Compare>(__x1, __x3, __c); cond_swap<_Compare>(__x2, __x4, __c); cond_swap<_Compare>(__x5, __x7, __c); cond_swap<_Compare>(__x6, __x8, __c); cond_swap<_Compare>(__x9, __x11, __c); cond_swap<_Compare>(__x10, __x12, __c); cond_swap<_Compare>(__x15, __x17, __c); cond_swap<_Compare>(__x16, __x18, __c); cond_swap<_Compare>(__x19, __x21, __c); cond_swap<_Compare>(__x20, __x22, __c); cond_swap<_Compare>(__x23, __x25, __c); cond_swap<_Compare>(__x24, __x26, __c);
    // Layer 3: [(0,4),(1,6),(2,5),(3,7),(8,14),(9,16),(10,15),(11,17),(18,22),(19,24),(20,23),(21,25)]
    cond_swap<_Compare>(__x1, __x5, __c); cond_swap<_Compare>(__x2, __x7, __c); cond_swap<_Compare>(__x3, __x6, __c); cond_swap<_Compare>(__x4, __x8, __c); cond_swap<_Compare>(__x9, __x15, __c); cond_swap<_Compare>(__x10, __x17, __c); cond_swap<_Compare>(__x11, __x16, __c); cond_swap<_Compare>(__x12, __x18, __c); cond_swap<_Compare>(__x19, __x23, __c); cond_swap<_Compare>(__x20, __x25, __c); cond_swap<_Compare>(__x21, __x24, __c); cond_swap<_Compare>(__x22, __x26, __c);
    // Layer 4: [(0,18),(1,19),(2,20),(3,21),(4,22),(5,23),(6,24),(7,25),(9,12),(13,16)]
    cond_swap<_Compare>(__x1, __x19, __c); cond_swap<_Compare>(__x2, __x20, __c); cond_swap<_Compare>(__x3, __x21, __c); cond_swap<_Compare>(__x4, __x22, __c); cond_swap<_Compare>(__x5, __x23, __c); cond_swap<_Compare>(__x6, __x24, __c); cond_swap<_Compare>(__x7, __x25, __c); cond_swap<_Compare>(__x8, __x26, __c); cond_swap<_Compare>(__x10, __x13, __c); cond_swap<_Compare>(__x14, __x17, __c);
    // Layer 5: [(3,11),(8,9),(10,13),(12,15),(14,22),(16,17)]
    cond_swap<_Compare>(__x4, __x12, __c); cond_swap<_Compare>(__x9, __x10, __c); cond_swap<_Compare>(__x11, __x14, __c); cond_swap<_Compare>(__x13, __x16, __c); cond_swap<_Compare>(__x15, __x23, __c); cond_swap<_Compare>(__x17, __x18, __c);
    // Layer 6: [(0,8),(1,9),(2,14),(6,12),(7,15),(10,18),(11,23),(13,19),(16,24),(17,25)]
    cond_swap<_Compare>(__x1, __x9, __c); cond_swap<_Compare>(__x2, __x10, __c); cond_swap<_Compare>(__x3, __x15, __c); cond_swap<_Compare>(__x7, __x13, __c); cond_swap<_Compare>(__x8, __x16, __c); cond_swap<_Compare>(__x11, __x19, __c); cond_swap<_Compare>(__x12, __x24, __c); cond_swap<_Compare>(__x14, __x20, __c); cond_swap<_Compare>(__x17, __x25, __c); cond_swap<_Compare>(__x18, __x26, __c);
    // Layer 7: [(1,2),(3,18),(4,8),(7,22),(17,21),(23,24)]
    cond_swap<_Compare>(__x2, __x3, __c); cond_swap<_Compare>(__x4, __x19, __c); cond_swap<_Compare>(__x5, __x9, __c); cond_swap<_Compare>(__x8, __x23, __c); cond_swap<_Compare>(__x18, __x22, __c); cond_swap<_Compare>(__x24, __x25, __c);
    // Layer 8: [(3,14),(4,10),(5,18),(7,20),(8,13),(11,22),(12,17),(15,21)]
    cond_swap<_Compare>(__x4, __x15, __c); cond_swap<_Compare>(__x5, __x11, __c); cond_swap<_Compare>(__x6, __x19, __c); cond_swap<_Compare>(__x8, __x21, __c); cond_swap<_Compare>(__x9, __x14, __c); cond_swap<_Compare>(__x12, __x23, __c); cond_swap<_Compare>(__x13, __x18, __c); cond_swap<_Compare>(__x16, __x22, __c);
    // Layer 9: [(1,4),(5,6),(7,9),(8,10),(15,17),(16,18),(19,20),(21,24)]
    cond_swap<_Compare>(__x2, __x5, __c); cond_swap<_Compare>(__x6, __x7, __c); cond_swap<_Compare>(__x8, __x10, __c); cond_swap<_Compare>(__x9, __x11, __c); cond_swap<_Compare>(__x16, __x18, __c); cond_swap<_Compare>(__x17, __x19, __c); cond_swap<_Compare>(__x20, __x21, __c); cond_swap<_Compare>(__x22, __x25, __c);
    // Layer 10: [(2,5),(3,10),(6,14),(9,13),(11,19),(12,16),(15,22),(20,23)]
    cond_swap<_Compare>(__x3, __x6, __c); cond_swap<_Compare>(__x4, __x11, __c); cond_swap<_Compare>(__x7, __x15, __c); cond_swap<_Compare>(__x10, __x14, __c); cond_swap<_Compare>(__x12, __x20, __c); cond_swap<_Compare>(__x13, __x17, __c); cond_swap<_Compare>(__x16, __x23, __c); cond_swap<_Compare>(__x21, __x24, __c);
    // Layer 11: [(2,8),(5,7),(6,9),(11,12),(13,14),(16,19),(17,23),(18,20)]
    cond_swap<_Compare>(__x3, __x9, __c); cond_swap<_Compare>(__x6, __x8, __c); cond_swap<_Compare>(__x7, __x10, __c); cond_swap<_Compare>(__x12, __x13, __c); cond_swap<_Compare>(__x14, __x15, __c); cond_swap<_Compare>(__x17, __x20, __c); cond_swap<_Compare>(__x18, __x24, __c); cond_swap<_Compare>(__x19, __x21, __c);
    // Layer 12: [(2,4),(3,5),(6,11),(7,10),(9,16),(12,13),(14,19),(15,18),(20,22),(21,23)]
    cond_swap<_Compare>(__x3, __x5, __c); cond_swap<_Compare>(__x4, __x6, __c); cond_swap<_Compare>(__x7, __x12, __c); cond_swap<_Compare>(__x8, __x11, __c); cond_swap<_Compare>(__x10, __x17, __c); cond_swap<_Compare>(__x13, __x14, __c); cond_swap<_Compare>(__x15, __x20, __c); cond_swap<_Compare>(__x16, __x19, __c); cond_swap<_Compare>(__x21, __x23, __c); cond_swap<_Compare>(__x22, __x24, __c);
    // Layer 13: [(3,4),(5,8),(6,7),(9,11),(10,12),(13,15),(14,16),(17,20),(18,19),(21,22)]
    cond_swap<_Compare>(__x4, __x5, __c); cond_swap<_Compare>(__x6, __x9, __c); cond_swap<_Compare>(__x7, __x8, __c); cond_swap<_Compare>(__x10, __x12, __c); cond_swap<_Compare>(__x11, __x13, __c); cond_swap<_Compare>(__x14, __x16, __c); cond_swap<_Compare>(__x15, __x17, __c); cond_swap<_Compare>(__x18, __x21, __c); cond_swap<_Compare>(__x19, __x20, __c); cond_swap<_Compare>(__x22, __x23, __c);
    // Layer 14: [(5,6),(7,8),(9,10),(11,12),(13,14),(15,16),(17,18),(19,20)]
    cond_swap<_Compare>(__x6, __x7, __c); cond_swap<_Compare>(__x8, __x9, __c); cond_swap<_Compare>(__x10, __x11, __c); cond_swap<_Compare>(__x12, __x13, __c); cond_swap<_Compare>(__x14, __x15, __c); cond_swap<_Compare>(__x16, __x17, __c); cond_swap<_Compare>(__x18, __x19, __c); cond_swap<_Compare>(__x20, __x21, __c);
    // Layer 15: [(4,5),(6,7),(8,9),(10,11),(12,13),(14,15),(16,17),(18,19),(20,21)]
    cond_swap<_Compare>(__x5, __x6, __c); cond_swap<_Compare>(__x7, __x8, __c); cond_swap<_Compare>(__x9, __x10, __c); cond_swap<_Compare>(__x11, __x12, __c); cond_swap<_Compare>(__x13, __x14, __c); cond_swap<_Compare>(__x15, __x16, __c); cond_swap<_Compare>(__x17, __x18, __c); cond_swap<_Compare>(__x19, __x20, __c); cond_swap<_Compare>(__x21, __x22, __c);
}

// N=27 (S(27)=147, depth 16)
// Rete: Dal testo fornito
template <class, class _Compare, class _RandomAccessIterator>
inline void
sort27branchless(_RandomAccessIterator __x1, _RandomAccessIterator __x2, _RandomAccessIterator __x3,
                 _RandomAccessIterator __x4, _RandomAccessIterator __x5, _RandomAccessIterator __x6,
                 _RandomAccessIterator __x7, _RandomAccessIterator __x8, _RandomAccessIterator __x9,
                 _RandomAccessIterator __x10, _RandomAccessIterator __x11, _RandomAccessIterator __x12,
                 _RandomAccessIterator __x13, _RandomAccessIterator __x14, _RandomAccessIterator __x15,
                 _RandomAccessIterator __x16, _RandomAccessIterator __x17, _RandomAccessIterator __x18,
                 _RandomAccessIterator __x19, _RandomAccessIterator __x20, _RandomAccessIterator __x21,
                 _RandomAccessIterator __x22, _RandomAccessIterator __x23, _RandomAccessIterator __x24,
                 _RandomAccessIterator __x25, _RandomAccessIterator __x26, _RandomAccessIterator __x27,
                 _Compare __c) {
    // Layer 1: [(0,1),(2,3),(4,5),(6,7),(8,9),(10,11),(12,14),(15,16),(17,18),(19,20),(21,22),(23,24),(25,26)]
    cond_swap<_Compare>(__x1, __x2, __c); cond_swap<_Compare>(__x3, __x4, __c); cond_swap<_Compare>(__x5, __x6, __c); cond_swap<_Compare>(__x7, __x8, __c); cond_swap<_Compare>(__x9, __x10, __c); cond_swap<_Compare>(__x11, __x12, __c); cond_swap<_Compare>(__x13, __x15, __c); cond_swap<_Compare>(__x16, __x17, __c); cond_swap<_Compare>(__x18, __x19, __c); cond_swap<_Compare>(__x20, __x21, __c); cond_swap<_Compare>(__x22, __x23, __c); cond_swap<_Compare>(__x24, __x25, __c); cond_swap<_Compare>(__x26, __x27, __c);
    // Layer 2: [(0,2),(1,3),(4,6),(5,7),(8,10),(9,11),(12,13),(15,17),(16,18),(19,21),(20,22),(23,25),(24,26)]
    cond_swap<_Compare>(__x1, __x3, __c); cond_swap<_Compare>(__x2, __x4, __c); cond_swap<_Compare>(__x5, __x7, __c); cond_swap<_Compare>(__x6, __x8, __c); cond_swap<_Compare>(__x9, __x11, __c); cond_swap<_Compare>(__x10, __x12, __c); cond_swap<_Compare>(__x13, __x14, __c); cond_swap<_Compare>(__x16, __x18, __c); cond_swap<_Compare>(__x17, __x19, __c); cond_swap<_Compare>(__x20, __x22, __c); cond_swap<_Compare>(__x21, __x23, __c); cond_swap<_Compare>(__x24, __x26, __c); cond_swap<_Compare>(__x25, __x27, __c);
    // Layer 3: [(0,23),(1,24),(2,25),(3,26),(4,8),(5,9),(6,10),(7,11),(13,14),(15,19),(16,20),(17,21),(18,22)]
    cond_swap<_Compare>(__x1, __x24, __c); cond_swap<_Compare>(__x2, __x25, __c); cond_swap<_Compare>(__x3, __x26, __c); cond_swap<_Compare>(__x4, __x27, __c); cond_swap<_Compare>(__x5, __x9, __c); cond_swap<_Compare>(__x6, __x10, __c); cond_swap<_Compare>(__x7, __x11, __c); cond_swap<_Compare>(__x8, __x12, __c); cond_swap<_Compare>(__x14, __x15, __c); cond_swap<_Compare>(__x16, __x20, __c); cond_swap<_Compare>(__x17, __x21, __c); cond_swap<_Compare>(__x18, __x22, __c); cond_swap<_Compare>(__x19, __x23, __c);
    // Layer 4: [(0,4),(1,6),(2,19),(3,20),(5,13),(9,21),(11,14),(12,16),(17,23),(18,24),(22,26)]
    cond_swap<_Compare>(__x1, __x5, __c); cond_swap<_Compare>(__x2, __x7, __c); cond_swap<_Compare>(__x3, __x20, __c); cond_swap<_Compare>(__x4, __x21, __c); cond_swap<_Compare>(__x6, __x14, __c); cond_swap<_Compare>(__x10, __x22, __c); cond_swap<_Compare>(__x12, __x15, __c); cond_swap<_Compare>(__x13, __x17, __c); cond_swap<_Compare>(__x18, __x24, __c); cond_swap<_Compare>(__x19, __x25, __c); cond_swap<_Compare>(__x23, __x27, __c);
    // Layer 5: [(5,17),(6,16),(7,22),(9,25),(10,24),(12,15),(13,20),(14,26)]
    cond_swap<_Compare>(__x6, __x18, __c); cond_swap<_Compare>(__x7, __x17, __c); cond_swap<_Compare>(__x8, __x23, __c); cond_swap<_Compare>(__x10, __x26, __c); cond_swap<_Compare>(__x11, __x25, __c); cond_swap<_Compare>(__x13, __x16, __c); cond_swap<_Compare>(__x14, __x21, __c); cond_swap<_Compare>(__x15, __x27, __c);
    // Layer 6: [(1,12),(4,15),(7,23),(10,19),(11,16),(13,18),(20,24),(22,25)]
    cond_swap<_Compare>(__x2, __x13, __c); cond_swap<_Compare>(__x5, __x16, __c); cond_swap<_Compare>(__x8, __x24, __c); cond_swap<_Compare>(__x11, __x20, __c); cond_swap<_Compare>(__x12, __x17, __c); cond_swap<_Compare>(__x14, __x19, __c); cond_swap<_Compare>(__x21, __x25, __c); cond_swap<_Compare>(__x23, __x26, __c);
    // Layer 7: [(0,1),(6,12),(8,11),(9,15),(10,17),(14,24),(16,21),(18,19)]
    cond_swap<_Compare>(__x1, __x2, __c); cond_swap<_Compare>(__x7, __x13, __c); cond_swap<_Compare>(__x9, __x12, __c); cond_swap<_Compare>(__x10, __x16, __c); cond_swap<_Compare>(__x11, __x18, __c); cond_swap<_Compare>(__x15, __x25, __c); cond_swap<_Compare>(__x17, __x22, __c); cond_swap<_Compare>(__x19, __x20, __c);
    // Layer 8: [(1,4),(2,8),(3,11),(12,15),(14,20),(16,22),(21,25)]
    cond_swap<_Compare>(__x2, __x5, __c); cond_swap<_Compare>(__x3, __x9, __c); cond_swap<_Compare>(__x4, __x12, __c); cond_swap<_Compare>(__x13, __x16, __c); cond_swap<_Compare>(__x15, __x21, __c); cond_swap<_Compare>(__x17, __x23, __c); cond_swap<_Compare>(__x22, __x26, __c);
    // Layer 9: [(2,5),(3,17),(8,13),(11,23),(21,22),(24,25)]
    cond_swap<_Compare>(__x3, __x6, __c); cond_swap<_Compare>(__x4, __x18, __c); cond_swap<_Compare>(__x9, __x14, __c); cond_swap<_Compare>(__x12, __x24, __c); cond_swap<_Compare>(__x22, __x23, __c); cond_swap<_Compare>(__x25, __x26, __c);
    // Layer 10: [(1,2),(3,10),(5,6),(7,13),(11,15),(14,21),(18,23),(20,22)]
    cond_swap<_Compare>(__x2, __x3, __c); cond_swap<_Compare>(__x4, __x11, __c); cond_swap<_Compare>(__x6, __x7, __c); cond_swap<_Compare>(__x8, __x14, __c); cond_swap<_Compare>(__x12, __x16, __c); cond_swap<_Compare>(__x15, __x22, __c); cond_swap<_Compare>(__x19, __x24, __c); cond_swap<_Compare>(__x21, __x23, __c);
    // Layer 11: [(4,5),(6,9),(7,8),(13,17),(14,16),(19,23),(22,24)]
    cond_swap<_Compare>(__x5, __x6, __c); cond_swap<_Compare>(__x7, __x10, __c); cond_swap<_Compare>(__x8, __x9, __c); cond_swap<_Compare>(__x14, __x18, __c); cond_swap<_Compare>(__x15, __x17, __c); cond_swap<_Compare>(__x20, __x24, __c); cond_swap<_Compare>(__x23, __x25, __c);
    // Layer 12: [(2,4),(3,6),(5,7),(8,12),(9,10),(11,13),(14,18),(15,17),(16,19),(21,23)]
    cond_swap<_Compare>(__x3, __x5, __c); cond_swap<_Compare>(__x4, __x7, __c); cond_swap<_Compare>(__x6, __x8, __c); cond_swap<_Compare>(__x9, __x13, __c); cond_swap<_Compare>(__x10, __x11, __c); cond_swap<_Compare>(__x12, __x14, __c); cond_swap<_Compare>(__x15, __x19, __c); cond_swap<_Compare>(__x16, __x18, __c); cond_swap<_Compare>(__x17, __x20, __c); cond_swap<_Compare>(__x22, __x24, __c);
    // Layer 13: [(3,5),(6,8),(7,9),(10,12),(11,14),(13,16),(15,18),(17,19),(20,21),(22,23)]
    cond_swap<_Compare>(__x4, __x6, __c); cond_swap<_Compare>(__x7, __x9, __c); cond_swap<_Compare>(__x8, __x10, __c); cond_swap<_Compare>(__x11, __x13, __c); cond_swap<_Compare>(__x12, __x15, __c); cond_swap<_Compare>(__x14, __x17, __c); cond_swap<_Compare>(__x16, __x19, __c); cond_swap<_Compare>(__x18, __x20, __c); cond_swap<_Compare>(__x21, __x22, __c); cond_swap<_Compare>(__x23, __x24, __c);
    // Layer 14: [(5,6),(8,11),(9,10),(12,14),(13,15),(17,18),(19,21)]
    cond_swap<_Compare>(__x6, __x7, __c); cond_swap<_Compare>(__x9, __x12, __c); cond_swap<_Compare>(__x10, __x11, __c); cond_swap<_Compare>(__x13, __x15, __c); cond_swap<_Compare>(__x14, __x16, __c); cond_swap<_Compare>(__x18, __x19, __c); cond_swap<_Compare>(__x20, __x22, __c);
    // Layer 15: [(4,5),(6,7),(8,9),(10,11),(12,13),(14,15),(16,17),(18,20),(21,22)]
    cond_swap<_Compare>(__x5, __x6, __c); cond_swap<_Compare>(__x7, __x8, __c); cond_swap<_Compare>(__x9, __x10, __c); cond_swap<_Compare>(__x11, __x12, __c); cond_swap<_Compare>(__x13, __x14, __c); cond_swap<_Compare>(__x15, __x16, __c); cond_swap<_Compare>(__x17, __x18, __c); cond_swap<_Compare>(__x19, __x21, __c); cond_swap<_Compare>(__x22, __x23, __c);
    // Layer 16: [(3,4),(5,6),(7,8),(9,10),(11,12),(13,14),(15,16),(17,18),(19,20)]
    cond_swap<_Compare>(__x4, __x5, __c); cond_swap<_Compare>(__x6, __x7, __c); cond_swap<_Compare>(__x8, __x9, __c); cond_swap<_Compare>(__x10, __x11, __c); cond_swap<_Compare>(__x12, __x13, __c); cond_swap<_Compare>(__x14, __x15, __c); cond_swap<_Compare>(__x16, __x17, __c); cond_swap<_Compare>(__x18, __x19, __c); cond_swap<_Compare>(__x20, __x21, __c);
}


// N=28 (S(28)=155, depth 14)
// Rete: Dal testo fornito
template <class, class _Compare, class _RandomAccessIterator>
inline void
sort28branchless(_RandomAccessIterator __x1, _RandomAccessIterator __x2, _RandomAccessIterator __x3,
                 _RandomAccessIterator __x4, _RandomAccessIterator __x5, _RandomAccessIterator __x6,
                 _RandomAccessIterator __x7, _RandomAccessIterator __x8, _RandomAccessIterator __x9,
                 _RandomAccessIterator __x10, _RandomAccessIterator __x11, _RandomAccessIterator __x12,
                 _RandomAccessIterator __x13, _RandomAccessIterator __x14, _RandomAccessIterator __x15,
                 _RandomAccessIterator __x16, _RandomAccessIterator __x17, _RandomAccessIterator __x18,
                 _RandomAccessIterator __x19, _RandomAccessIterator __x20, _RandomAccessIterator __x21,
                 _RandomAccessIterator __x22, _RandomAccessIterator __x23, _RandomAccessIterator __x24,
                 _RandomAccessIterator __x25, _RandomAccessIterator __x26, _RandomAccessIterator __x27,
                 _RandomAccessIterator __x28,
                 _Compare __c) {
    // Layer 1: [(0,1),(2,3),(4,5),(6,7),(8,9),(10,11),(12,13),(14,15),(16,17),(18,19),(20,21),(22,23),(24,25),(26,27)]
    cond_swap<_Compare>(__x1, __x2, __c); cond_swap<_Compare>(__x3, __x4, __c); cond_swap<_Compare>(__x5, __x6, __c); cond_swap<_Compare>(__x7, __x8, __c); cond_swap<_Compare>(__x9, __x10, __c); cond_swap<_Compare>(__x11, __x12, __c); cond_swap<_Compare>(__x13, __x14, __c); cond_swap<_Compare>(__x15, __x16, __c); cond_swap<_Compare>(__x17, __x18, __c); cond_swap<_Compare>(__x19, __x20, __c); cond_swap<_Compare>(__x21, __x22, __c); cond_swap<_Compare>(__x23, __x24, __c); cond_swap<_Compare>(__x25, __x26, __c); cond_swap<_Compare>(__x27, __x28, __c);
    // Layer 2: [(0,2),(1,3),(4,6),(5,7),(8,10),(9,11),(12,14),(13,15),(16,18),(17,19),(20,22),(21,23),(24,26),(25,27)]
    cond_swap<_Compare>(__x1, __x3, __c); cond_swap<_Compare>(__x2, __x4, __c); cond_swap<_Compare>(__x5, __x7, __c); cond_swap<_Compare>(__x6, __x8, __c); cond_swap<_Compare>(__x9, __x11, __c); cond_swap<_Compare>(__x10, __x12, __c); cond_swap<_Compare>(__x13, __x15, __c); cond_swap<_Compare>(__x14, __x16, __c); cond_swap<_Compare>(__x17, __x19, __c); cond_swap<_Compare>(__x18, __x20, __c); cond_swap<_Compare>(__x21, __x23, __c); cond_swap<_Compare>(__x22, __x24, __c); cond_swap<_Compare>(__x25, __x27, __c); cond_swap<_Compare>(__x26, __x28, __c);
    // Layer 3: [(0,4),(1,5),(2,6),(3,7),(8,12),(9,13),(14,18),(15,19),(20,24),(21,25),(22,26),(23,27)]
    cond_swap<_Compare>(__x1, __x5, __c); cond_swap<_Compare>(__x2, __x6, __c); cond_swap<_Compare>(__x3, __x7, __c); cond_swap<_Compare>(__x4, __x8, __c); cond_swap<_Compare>(__x9, __x13, __c); cond_swap<_Compare>(__x10, __x14, __c); cond_swap<_Compare>(__x15, __x19, __c); cond_swap<_Compare>(__x16, __x20, __c); cond_swap<_Compare>(__x21, __x25, __c); cond_swap<_Compare>(__x22, __x26, __c); cond_swap<_Compare>(__x23, __x27, __c); cond_swap<_Compare>(__x24, __x28, __c);
    // Layer 4: [(0,20),(1,21),(2,22),(3,23),(4,24),(5,25),(6,26),(7,27),(9,17),(10,18),(11,15),(12,16)]
    cond_swap<_Compare>(__x1, __x21, __c); cond_swap<_Compare>(__x2, __x22, __c); cond_swap<_Compare>(__x3, __x23, __c); cond_swap<_Compare>(__x4, __x24, __c); cond_swap<_Compare>(__x5, __x25, __c); cond_swap<_Compare>(__x6, __x26, __c); cond_swap<_Compare>(__x7, __x27, __c); cond_swap<_Compare>(__x8, __x28, __c); cond_swap<_Compare>(__x10, __x18, __c); cond_swap<_Compare>(__x11, __x19, __c); cond_swap<_Compare>(__x12, __x16, __c); cond_swap<_Compare>(__x13, __x17, __c);
    // Layer 5: [(1,2),(4,20),(5,6),(7,23),(8,12),(9,16),(10,14),(11,18),(13,17),(15,19),(21,22),(25,26)]
    cond_swap<_Compare>(__x2, __x3, __c); cond_swap<_Compare>(__x5, __x21, __c); cond_swap<_Compare>(__x6, __x7, __c); cond_swap<_Compare>(__x8, __x24, __c); cond_swap<_Compare>(__x9, __x13, __c); cond_swap<_Compare>(__x10, __x17, __c); cond_swap<_Compare>(__x11, __x15, __c); cond_swap<_Compare>(__x12, __x19, __c); cond_swap<_Compare>(__x14, __x18, __c); cond_swap<_Compare>(__x16, __x20, __c); cond_swap<_Compare>(__x22, __x23, __c); cond_swap<_Compare>(__x26, __x27, __c);
    // Layer 6: [(0,8),(1,9),(2,12),(3,5),(4,10),(6,16),(7,13),(11,21),(14,20),(15,25),(17,23),(18,26),(19,27),(22,24)]
    cond_swap<_Compare>(__x1, __x9, __c); cond_swap<_Compare>(__x2, __x10, __c); cond_swap<_Compare>(__x3, __x13, __c); cond_swap<_Compare>(__x4, __x6, __c); cond_swap<_Compare>(__x5, __x11, __c); cond_swap<_Compare>(__x7, __x17, __c); cond_swap<_Compare>(__x8, __x14, __c); cond_swap<_Compare>(__x12, __x22, __c); cond_swap<_Compare>(__x15, __x21, __c); cond_swap<_Compare>(__x16, __x26, __c); cond_swap<_Compare>(__x18, __x24, __c); cond_swap<_Compare>(__x19, __x27, __c); cond_swap<_Compare>(__x20, __x28, __c); cond_swap<_Compare>(__x23, __x25, __c);
    // Layer 7: [(2,4),(3,7),(5,17),(8,14),(9,11),(10,22),(13,19),(16,18),(20,24),(23,25)]
    cond_swap<_Compare>(__x3, __x5, __c); cond_swap<_Compare>(__x4, __x8, __c); cond_swap<_Compare>(__x6, __x18, __c); cond_swap<_Compare>(__x9, __x15, __c); cond_swap<_Compare>(__x10, __x12, __c); cond_swap<_Compare>(__x11, __x23, __c); cond_swap<_Compare>(__x14, __x20, __c); cond_swap<_Compare>(__x17, __x19, __c); cond_swap<_Compare>(__x21, __x25, __c); cond_swap<_Compare>(__x24, __x26, __c);
    // Layer 8: [(1,8),(3,9),(5,11),(6,10),(7,15),(12,20),(16,22),(17,21),(18,24),(19,26)]
    cond_swap<_Compare>(__x2, __x9, __c); cond_swap<_Compare>(__x4, __x10, __c); cond_swap<_Compare>(__x6, __x12, __c); cond_swap<_Compare>(__x7, __x11, __c); cond_swap<_Compare>(__x8, __x16, __c); cond_swap<_Compare>(__x13, __x21, __c); cond_swap<_Compare>(__x17, __x23, __c); cond_swap<_Compare>(__x18, __x22, __c); cond_swap<_Compare>(__x19, __x25, __c); cond_swap<_Compare>(__x20, __x27, __c);
    // Layer 9: [(1,2),(4,6),(5,9),(10,16),(11,17),(12,14),(13,15),(18,22),(21,23),(25,26)]
    cond_swap<_Compare>(__x2, __x3, __c); cond_swap<_Compare>(__x5, __x7, __c); cond_swap<_Compare>(__x6, __x10, __c); cond_swap<_Compare>(__x11, __x17, __c); cond_swap<_Compare>(__x12, __x18, __c); cond_swap<_Compare>(__x13, __x15, __c); cond_swap<_Compare>(__x14, __x16, __c); cond_swap<_Compare>(__x19, __x23, __c); cond_swap<_Compare>(__x22, __x24, __c); cond_swap<_Compare>(__x26, __x27, __c);
    // Layer 10: [(4,8),(6,12),(7,11),(10,14),(13,17),(15,21),(16,20),(19,23)]
    cond_swap<_Compare>(__x5, __x9, __c); cond_swap<_Compare>(__x7, __x13, __c); cond_swap<_Compare>(__x8, __x12, __c); cond_swap<_Compare>(__x11, __x15, __c); cond_swap<_Compare>(__x14, __x18, __c); cond_swap<_Compare>(__x16, __x22, __c); cond_swap<_Compare>(__x17, __x21, __c); cond_swap<_Compare>(__x20, __x24, __c);
    // Layer 11: [(2,4),(6,8),(7,16),(9,14),(10,12),(11,20),(13,18),(15,17),(19,21),(23,25)]
    cond_swap<_Compare>(__x3, __x5, __c); cond_swap<_Compare>(__x7, __x9, __c); cond_swap<_Compare>(__x8, __x17, __c); cond_swap<_Compare>(__x10, __x15, __c); cond_swap<_Compare>(__x11, __x13, __c); cond_swap<_Compare>(__x12, __x21, __c); cond_swap<_Compare>(__x14, __x19, __c); cond_swap<_Compare>(__x16, __x18, __c); cond_swap<_Compare>(__x20, __x22, __c); cond_swap<_Compare>(__x24, __x26, __c);
    // Layer 12: [(3,10),(5,12),(7,9),(11,13),(14,16),(15,22),(17,24),(18,20)]
    cond_swap<_Compare>(__x4, __x11, __c); cond_swap<_Compare>(__x6, __x13, __c); cond_swap<_Compare>(__x8, __x10, __c); cond_swap<_Compare>(__x12, __x14, __c); cond_swap<_Compare>(__x15, __x17, __c); cond_swap<_Compare>(__x16, __x23, __c); cond_swap<_Compare>(__x18, __x25, __c); cond_swap<_Compare>(__x19, __x21, __c);
    // Layer 13: [(3,6),(5,8),(7,10),(9,12),(11,14),(13,16),(15,18),(17,20),(19,22),(21,24)]
    cond_swap<_Compare>(__x4, __x7, __c); cond_swap<_Compare>(__x6, __x9, __c); cond_swap<_Compare>(__x8, __x11, __c); cond_swap<_Compare>(__x10, __x13, __c); cond_swap<_Compare>(__x12, __x15, __c); cond_swap<_Compare>(__x14, __x17, __c); cond_swap<_Compare>(__x16, __x19, __c); cond_swap<_Compare>(__x18, __x21, __c); cond_swap<_Compare>(__x20, __x23, __c); cond_swap<_Compare>(__x22, __x25, __c);
    // Layer 14: [(3,4),(5,6),(7,8),(9,10),(11,12),(13,14),(15,16),(17,18),(19,20),(21,22),(23,24)]
    cond_swap<_Compare>(__x4, __x5, __c); cond_swap<_Compare>(__x6, __x7, __c); cond_swap<_Compare>(__x8, __x9, __c); cond_swap<_Compare>(__x10, __x11, __c); cond_swap<_Compare>(__x12, __x13, __c); cond_swap<_Compare>(__x14, __x15, __c); cond_swap<_Compare>(__x16, __x17, __c); cond_swap<_Compare>(__x18, __x19, __c); cond_swap<_Compare>(__x20, __x21, __c); cond_swap<_Compare>(__x22, __x23, __c); cond_swap<_Compare>(__x24, __x25, __c);
}

// N=29 (S(29)=164, depth 15)
// Rete: Dal testo fornito
template <class, class _Compare, class _RandomAccessIterator>
inline void
sort29branchless(_RandomAccessIterator __x1, _RandomAccessIterator __x2, _RandomAccessIterator __x3,
                 _RandomAccessIterator __x4, _RandomAccessIterator __x5, _RandomAccessIterator __x6,
                 _RandomAccessIterator __x7, _RandomAccessIterator __x8, _RandomAccessIterator __x9,
                 _RandomAccessIterator __x10, _RandomAccessIterator __x11, _RandomAccessIterator __x12,
                 _RandomAccessIterator __x13, _RandomAccessIterator __x14, _RandomAccessIterator __x15,
                 _RandomAccessIterator __x16, _RandomAccessIterator __x17, _RandomAccessIterator __x18,
                 _RandomAccessIterator __x19, _RandomAccessIterator __x20, _RandomAccessIterator __x21,
                 _RandomAccessIterator __x22, _RandomAccessIterator __x23, _RandomAccessIterator __x24,
                 _RandomAccessIterator __x25, _RandomAccessIterator __x26, _RandomAccessIterator __x27,
                 _RandomAccessIterator __x28, _RandomAccessIterator __x29,
                 _Compare __c) {
    // Layer 1: [(0,1),(2,3),(4,5),(6,7),(8,9),(10,11),(12,13),(14,15),(16,28),(17,26),(18,25),(19,23),(21,27),(22,24)]
    cond_swap<_Compare>(__x1, __x2, __c); cond_swap<_Compare>(__x3, __x4, __c); cond_swap<_Compare>(__x5, __x6, __c); cond_swap<_Compare>(__x7, __x8, __c); cond_swap<_Compare>(__x9, __x10, __c); cond_swap<_Compare>(__x11, __x12, __c); cond_swap<_Compare>(__x13, __x14, __c); cond_swap<_Compare>(__x15, __x16, __c); cond_swap<_Compare>(__x17, __x29, __c); cond_swap<_Compare>(__x18, __x27, __c); cond_swap<_Compare>(__x19, __x26, __c); cond_swap<_Compare>(__x20, __x24, __c); cond_swap<_Compare>(__x22, __x28, __c); cond_swap<_Compare>(__x23, __x25, __c);
    // Layer 2: [(0,2),(1,3),(4,6),(5,7),(8,10),(9,11),(12,14),(13,15),(17,22),(18,19),(20,27),(23,25),(24,26)]
    cond_swap<_Compare>(__x1, __x3, __c); cond_swap<_Compare>(__x2, __x4, __c); cond_swap<_Compare>(__x5, __x7, __c); cond_swap<_Compare>(__x6, __x8, __c); cond_swap<_Compare>(__x9, __x11, __c); cond_swap<_Compare>(__x10, __x12, __c); cond_swap<_Compare>(__x13, __x15, __c); cond_swap<_Compare>(__x14, __x16, __c); cond_swap<_Compare>(__x18, __x23, __c); cond_swap<_Compare>(__x19, __x20, __c); cond_swap<_Compare>(__x21, __x28, __c); cond_swap<_Compare>(__x24, __x26, __c); cond_swap<_Compare>(__x25, __x27, __c);
    // Layer 3: [(0,4),(1,5),(2,6),(3,7),(8,12),(9,13),(10,14),(11,15),(16,20),(17,18),(19,22),(23,24),(25,26),(27,28)]
    cond_swap<_Compare>(__x1, __x5, __c); cond_swap<_Compare>(__x2, __x6, __c); cond_swap<_Compare>(__x3, __x7, __c); cond_swap<_Compare>(__x4, __x8, __c); cond_swap<_Compare>(__x9, __x13, __c); cond_swap<_Compare>(__x10, __x14, __c); cond_swap<_Compare>(__x11, __x15, __c); cond_swap<_Compare>(__x12, __x16, __c); cond_swap<_Compare>(__x17, __x21, __c); cond_swap<_Compare>(__x18, __x19, __c); cond_swap<_Compare>(__x20, __x23, __c); cond_swap<_Compare>(__x24, __x25, __c); cond_swap<_Compare>(__x26, __x27, __c); cond_swap<_Compare>(__x28, __x29, __c);
    // Layer 4: [(0,8),(1,9),(2,10),(3,11),(4,12),(5,13),(6,14),(7,15),(16,18),(20,22),(21,25),(24,27),(26,28)]
    cond_swap<_Compare>(__x1, __x9, __c); cond_swap<_Compare>(__x2, __x10, __c); cond_swap<_Compare>(__x3, __x11, __c); cond_swap<_Compare>(__x4, __x12, __c); cond_swap<_Compare>(__x5, __x13, __c); cond_swap<_Compare>(__x6, __x14, __c); cond_swap<_Compare>(__x7, __x15, __c); cond_swap<_Compare>(__x8, __x16, __c); cond_swap<_Compare>(__x17, __x19, __c); cond_swap<_Compare>(__x21, __x23, __c); cond_swap<_Compare>(__x22, __x26, __c); cond_swap<_Compare>(__x25, __x28, __c); cond_swap<_Compare>(__x27, __x29, __c);
    // Layer 5: [(1,8),(2,24),(4,19),(5,20),(6,21),(7,27),(9,18),(10,23),(11,26),(13,22),(14,25),(15,28),(16,17)]
    cond_swap<_Compare>(__x2, __x9, __c); cond_swap<_Compare>(__x3, __x25, __c); cond_swap<_Compare>(__x5, __x20, __c); cond_swap<_Compare>(__x6, __x21, __c); cond_swap<_Compare>(__x7, __x22, __c); cond_swap<_Compare>(__x8, __x28, __c); cond_swap<_Compare>(__x10, __x19, __c); cond_swap<_Compare>(__x11, __x24, __c); cond_swap<_Compare>(__x12, __x27, __c); cond_swap<_Compare>(__x14, __x23, __c); cond_swap<_Compare>(__x15, __x26, __c); cond_swap<_Compare>(__x16, __x29, __c); cond_swap<_Compare>(__x17, __x18, __c);
    // Layer 6: [(0,6),(2,4),(3,24),(5,10),(12,19),(13,18),(14,21),(15,25),(20,23),(26,27)]
    cond_swap<_Compare>(__x1, __x7, __c); cond_swap<_Compare>(__x3, __x5, __c); cond_swap<_Compare>(__x4, __x25, __c); cond_swap<_Compare>(__x6, __x11, __c); cond_swap<_Compare>(__x13, __x20, __c); cond_swap<_Compare>(__x14, __x19, __c); cond_swap<_Compare>(__x15, __x22, __c); cond_swap<_Compare>(__x16, __x26, __c); cond_swap<_Compare>(__x21, __x24, __c); cond_swap<_Compare>(__x27, __x28, __c);
    // Layer 7: [(0,16),(1,6),(3,12),(4,8),(5,17),(7,24),(14,20),(15,26),(18,21),(19,23),(25,27)]
    cond_swap<_Compare>(__x1, __x17, __c); cond_swap<_Compare>(__x2, __x7, __c); cond_swap<_Compare>(__x4, __x13, __c); cond_swap<_Compare>(__x5, __x9, __c); cond_swap<_Compare>(__x6, __x18, __c); cond_swap<_Compare>(__x8, __x25, __c); cond_swap<_Compare>(__x15, __x21, __c); cond_swap<_Compare>(__x16, __x27, __c); cond_swap<_Compare>(__x19, __x22, __c); cond_swap<_Compare>(__x20, __x24, __c); cond_swap<_Compare>(__x26, __x28, __c);
    // Layer 8: [(1,5),(2,16),(3,10),(6,9),(7,18),(8,17),(11,19),(13,14),(15,22),(21,23),(25,26)]
    cond_swap<_Compare>(__x2, __x6, __c); cond_swap<_Compare>(__x3, __x17, __c); cond_swap<_Compare>(__x4, __x11, __c); cond_swap<_Compare>(__x7, __x10, __c); cond_swap<_Compare>(__x8, __x19, __c); cond_swap<_Compare>(__x9, __x18, __c); cond_swap<_Compare>(__x12, __x20, __c); cond_swap<_Compare>(__x14, __x15, __c); cond_swap<_Compare>(__x16, __x23, __c); cond_swap<_Compare>(__x22, __x24, __c); cond_swap<_Compare>(__x26, __x27, __c);
    // Layer 9: [(1,2),(3,5),(4,8),(6,16),(7,11),(9,17),(10,12),(14,20),(15,18),(19,24),(22,27)]
    cond_swap<_Compare>(__x2, __x3, __c); cond_swap<_Compare>(__x4, __x6, __c); cond_swap<_Compare>(__x5, __x9, __c); cond_swap<_Compare>(__x7, __x17, __c); cond_swap<_Compare>(__x8, __x12, __c); cond_swap<_Compare>(__x10, __x18, __c); cond_swap<_Compare>(__x11, __x13, __c); cond_swap<_Compare>(__x15, __x21, __c); cond_swap<_Compare>(__x16, __x19, __c); cond_swap<_Compare>(__x20, __x25, __c); cond_swap<_Compare>(__x23, __x28, __c);
    // Layer 10: [(4,6),(9,16),(10,13),(11,19),(12,14),(20,21),(22,26),(23,24)]
    cond_swap<_Compare>(__x5, __x7, __c); cond_swap<_Compare>(__x10, __x17, __c); cond_swap<_Compare>(__x11, __x14, __c); cond_swap<_Compare>(__x12, __x20, __c); cond_swap<_Compare>(__x13, __x15, __c); cond_swap<_Compare>(__x21, __x22, __c); cond_swap<_Compare>(__x23, __x27, __c); cond_swap<_Compare>(__x24, __x25, __c);
    // Layer 11: [(2,4),(3,6),(7,16),(8,9),(11,17),(15,19),(18,23),(24,25)]
    cond_swap<_Compare>(__x3, __x5, __c); cond_swap<_Compare>(__x4, __x7, __c); cond_swap<_Compare>(__x8, __x17, __c); cond_swap<_Compare>(__x9, __x10, __c); cond_swap<_Compare>(__x12, __x18, __c); cond_swap<_Compare>(__x16, __x20, __c); cond_swap<_Compare>(__x19, __x24, __c); cond_swap<_Compare>(__x25, __x26, __c);
    // Layer 12: [(3,4),(5,9),(7,10),(11,13),(12,16),(14,17),(15,20),(19,21),(22,24)]
    cond_swap<_Compare>(__x4, __x5, __c); cond_swap<_Compare>(__x6, __x10, __c); cond_swap<_Compare>(__x8, __x11, __c); cond_swap<_Compare>(__x12, __x14, __c); cond_swap<_Compare>(__x13, __x17, __c); cond_swap<_Compare>(__x15, __x18, __c); cond_swap<_Compare>(__x16, __x21, __c); cond_swap<_Compare>(__x20, __x22, __c); cond_swap<_Compare>(__x23, __x25, __c);
    // Layer 13: [(5,8),(6,7),(9,12),(10,11),(13,14),(15,16),(17,20),(18,19),(21,23),(24,25)]
    cond_swap<_Compare>(__x6, __x9, __c); cond_swap<_Compare>(__x7, __x8, __c); cond_swap<_Compare>(__x10, __x13, __c); cond_swap<_Compare>(__x11, __x12, __c); cond_swap<_Compare>(__x14, __x15, __c); cond_swap<_Compare>(__x16, __x17, __c); cond_swap<_Compare>(__x18, __x21, __c); cond_swap<_Compare>(__x19, __x20, __c); cond_swap<_Compare>(__x22, __x24, __c); cond_swap<_Compare>(__x25, __x26, __c);
    // Layer 14: [(5,6),(7,8),(9,10),(11,12),(13,15),(14,16),(17,18),(19,20),(21,22)]
    cond_swap<_Compare>(__x6, __x7, __c); cond_swap<_Compare>(__x8, __x9, __c); cond_swap<_Compare>(__x10, __x11, __c); cond_swap<_Compare>(__x12, __x13, __c); cond_swap<_Compare>(__x14, __x16, __c); cond_swap<_Compare>(__x15, __x17, __c); cond_swap<_Compare>(__x18, __x19, __c); cond_swap<_Compare>(__x20, __x21, __c); cond_swap<_Compare>(__x22, __x23, __c);
    // Layer 15: [(4,5),(6,7),(8,9),(10,11),(12,13),(14,15),(16,17),(18,19),(20,21),(22,23)]
    cond_swap<_Compare>(__x5, __x6, __c); cond_swap<_Compare>(__x7, __x8, __c); cond_swap<_Compare>(__x9, __x10, __c); cond_swap<_Compare>(__x11, __x12, __c); cond_swap<_Compare>(__x13, __x14, __c); cond_swap<_Compare>(__x15, __x16, __c); cond_swap<_Compare>(__x17, __x18, __c); cond_swap<_Compare>(__x19, __x20, __c); cond_swap<_Compare>(__x21, __x22, __c); cond_swap<_Compare>(__x23, __x24, __c);
}

// N=30 (S(30)=172, depth 14)
// Rete: Dal testo fornito
template <class, class _Compare, class _RandomAccessIterator>
inline void
sort30branchless(_RandomAccessIterator __x1, _RandomAccessIterator __x2, _RandomAccessIterator __x3,
                 _RandomAccessIterator __x4, _RandomAccessIterator __x5, _RandomAccessIterator __x6,
                 _RandomAccessIterator __x7, _RandomAccessIterator __x8, _RandomAccessIterator __x9,
                 _RandomAccessIterator __x10, _RandomAccessIterator __x11, _RandomAccessIterator __x12,
                 _RandomAccessIterator __x13, _RandomAccessIterator __x14, _RandomAccessIterator __x15,
                 _RandomAccessIterator __x16, _RandomAccessIterator __x17, _RandomAccessIterator __x18,
                 _RandomAccessIterator __x19, _RandomAccessIterator __x20, _RandomAccessIterator __x21,
                 _RandomAccessIterator __x22, _RandomAccessIterator __x23, _RandomAccessIterator __x24,
                 _RandomAccessIterator __x25, _RandomAccessIterator __x26, _RandomAccessIterator __x27,
                 _RandomAccessIterator __x28, _RandomAccessIterator __x29, _RandomAccessIterator __x30,
                 _Compare __c) {
    // Layer 1: [(0,1),(2,3),(4,5),(6,7),(8,9),(10,11),(12,13),(14,15),(16,17),(18,19),(20,21),(22,23),(24,25),(26,27),(28,29)]
    cond_swap<_Compare>(__x1, __x2, __c); cond_swap<_Compare>(__x3, __x4, __c); cond_swap<_Compare>(__x5, __x6, __c); cond_swap<_Compare>(__x7, __x8, __c); cond_swap<_Compare>(__x9, __x10, __c); cond_swap<_Compare>(__x11, __x12, __c); cond_swap<_Compare>(__x13, __x14, __c); cond_swap<_Compare>(__x15, __x16, __c); cond_swap<_Compare>(__x17, __x18, __c); cond_swap<_Compare>(__x19, __x20, __c); cond_swap<_Compare>(__x21, __x22, __c); cond_swap<_Compare>(__x23, __x24, __c); cond_swap<_Compare>(__x25, __x26, __c); cond_swap<_Compare>(__x27, __x28, __c); cond_swap<_Compare>(__x29, __x30, __c);
    // Layer 2: [(0,2),(1,3),(4,6),(5,7),(8,10),(9,11),(13,15),(14,16),(18,20),(19,21),(22,24),(23,25),(26,28),(27,29)]
    cond_swap<_Compare>(__x1, __x3, __c); cond_swap<_Compare>(__x2, __x4, __c); cond_swap<_Compare>(__x5, __x7, __c); cond_swap<_Compare>(__x6, __x8, __c); cond_swap<_Compare>(__x9, __x11, __c); cond_swap<_Compare>(__x10, __x12, __c); cond_swap<_Compare>(__x14, __x16, __c); cond_swap<_Compare>(__x15, __x17, __c); cond_swap<_Compare>(__x19, __x21, __c); cond_swap<_Compare>(__x20, __x22, __c); cond_swap<_Compare>(__x23, __x25, __c); cond_swap<_Compare>(__x24, __x26, __c); cond_swap<_Compare>(__x27, __x29, __c); cond_swap<_Compare>(__x28, __x30, __c);
    // Layer 3: [(0,4),(1,5),(2,6),(3,7),(8,14),(9,17),(10,16),(12,20),(13,19),(15,21),(22,26),(23,27),(24,28),(25,29)]
    cond_swap<_Compare>(__x1, __x5, __c); cond_swap<_Compare>(__x2, __x6, __c); cond_swap<_Compare>(__x3, __x7, __c); cond_swap<_Compare>(__x4, __x8, __c); cond_swap<_Compare>(__x9, __x15, __c); cond_swap<_Compare>(__x10, __x18, __c); cond_swap<_Compare>(__x11, __x17, __c); cond_swap<_Compare>(__x13, __x21, __c); cond_swap<_Compare>(__x14, __x20, __c); cond_swap<_Compare>(__x16, __x22, __c); cond_swap<_Compare>(__x23, __x27, __c); cond_swap<_Compare>(__x24, __x28, __c); cond_swap<_Compare>(__x25, __x29, __c); cond_swap<_Compare>(__x26, __x30, __c);
    // Layer 4: [(0,8),(1,9),(2,14),(3,17),(4,10),(5,11),(6,16),(12,26),(13,23),(15,27),(18,24),(19,25),(20,28),(21,29)]
    cond_swap<_Compare>(__x1, __x9, __c); cond_swap<_Compare>(__x2, __x10, __c); cond_swap<_Compare>(__x3, __x15, __c); cond_swap<_Compare>(__x4, __x18, __c); cond_swap<_Compare>(__x5, __x11, __c); cond_swap<_Compare>(__x6, __x12, __c); cond_swap<_Compare>(__x7, __x17, __c); cond_swap<_Compare>(__x13, __x27, __c); cond_swap<_Compare>(__x14, __x24, __c); cond_swap<_Compare>(__x16, __x28, __c); cond_swap<_Compare>(__x19, __x25, __c); cond_swap<_Compare>(__x20, __x26, __c); cond_swap<_Compare>(__x21, __x29, __c); cond_swap<_Compare>(__x22, __x30, __c);
    // Layer 5: [(1,13),(2,12),(3,15),(4,18),(5,19),(6,20),(7,21),(8,22),(9,23),(10,24),(11,25),(14,26),(16,28),(17,27)]
    cond_swap<_Compare>(__x2, __x14, __c); cond_swap<_Compare>(__x3, __x13, __c); cond_swap<_Compare>(__x4, __x16, __c); cond_swap<_Compare>(__x5, __x19, __c); cond_swap<_Compare>(__x6, __x20, __c); cond_swap<_Compare>(__x7, __x21, __c); cond_swap<_Compare>(__x8, __x22, __c); cond_swap<_Compare>(__x9, __x23, __c); cond_swap<_Compare>(__x10, __x24, __c); cond_swap<_Compare>(__x11, __x25, __c); cond_swap<_Compare>(__x12, __x26, __c); cond_swap<_Compare>(__x15, __x27, __c); cond_swap<_Compare>(__x17, __x29, __c); cond_swap<_Compare>(__x18, __x28, __c);
    // Layer 6: [(0,4),(2,8),(3,13),(5,9),(6,22),(7,23),(10,12),(11,15),(14,18),(16,26),(17,19),(20,24),(21,27),(25,29)]
    cond_swap<_Compare>(__x1, __x5, __c); cond_swap<_Compare>(__x3, __x9, __c); cond_swap<_Compare>(__x4, __x14, __c); cond_swap<_Compare>(__x6, __x10, __c); cond_swap<_Compare>(__x7, __x23, __c); cond_swap<_Compare>(__x8, __x24, __c); cond_swap<_Compare>(__x11, __x13, __c); cond_swap<_Compare>(__x12, __x16, __c); cond_swap<_Compare>(__x15, __x19, __c); cond_swap<_Compare>(__x17, __x27, __c); cond_swap<_Compare>(__x18, __x20, __c); cond_swap<_Compare>(__x21, __x25, __c); cond_swap<_Compare>(__x22, __x28, __c); cond_swap<_Compare>(__x26, __x30, __c);
    // Layer 7: [(0,2),(1,14),(3,5),(4,8),(9,13),(11,17),(12,18),(15,28),(16,20),(21,25),(24,26),(27,29)]
    cond_swap<_Compare>(__x1, __x3, __c); cond_swap<_Compare>(__x2, __x15, __c); cond_swap<_Compare>(__x4, __x6, __c); cond_swap<_Compare>(__x5, __x9, __c); cond_swap<_Compare>(__x10, __x14, __c); cond_swap<_Compare>(__x12, __x18, __c); cond_swap<_Compare>(__x13, __x19, __c); cond_swap<_Compare>(__x16, __x29, __c); cond_swap<_Compare>(__x17, __x21, __c); cond_swap<_Compare>(__x22, __x26, __c); cond_swap<_Compare>(__x25, __x27, __c); cond_swap<_Compare>(__x28, __x30, __c);
    // Layer 8: [(2,4),(5,9),(6,14),(7,13),(8,10),(15,23),(16,22),(19,21),(20,24),(25,27)]
    cond_swap<_Compare>(__x3, __x5, __c); cond_swap<_Compare>(__x6, __x10, __c); cond_swap<_Compare>(__x7, __x15, __c); cond_swap<_Compare>(__x8, __x14, __c); cond_swap<_Compare>(__x9, __x11, __c); cond_swap<_Compare>(__x16, __x24, __c); cond_swap<_Compare>(__x17, __x23, __c); cond_swap<_Compare>(__x20, __x22, __c); cond_swap<_Compare>(__x21, __x25, __c); cond_swap<_Compare>(__x26, __x28, __c);
    // Layer 9: [(6,8),(7,11),(10,14),(12,16),(13,17),(15,19),(18,22),(21,23)]
    cond_swap<_Compare>(__x7, __x9, __c); cond_swap<_Compare>(__x8, __x12, __c); cond_swap<_Compare>(__x11, __x15, __c); cond_swap<_Compare>(__x13, __x17, __c); cond_swap<_Compare>(__x14, __x18, __c); cond_swap<_Compare>(__x16, __x20, __c); cond_swap<_Compare>(__x19, __x23, __c); cond_swap<_Compare>(__x22, __x24, __c);
    // Layer 10: [(4,6),(7,9),(8,10),(11,13),(12,14),(15,17),(16,18),(19,21),(20,22),(23,25)]
    cond_swap<_Compare>(__x5, __x7, __c); cond_swap<_Compare>(__x8, __x10, __c); cond_swap<_Compare>(__x9, __x11, __c); cond_swap<_Compare>(__x12, __x14, __c); cond_swap<_Compare>(__x13, __x15, __c); cond_swap<_Compare>(__x16, __x18, __c); cond_swap<_Compare>(__x17, __x19, __c); cond_swap<_Compare>(__x20, __x22, __c); cond_swap<_Compare>(__x21, __x23, __c); cond_swap<_Compare>(__x24, __x26, __c);
    // Layer 11: [(1,8),(3,18),(5,20),(7,22),(9,24),(10,12),(11,26),(13,15),(14,16),(17,19),(21,28)]
    cond_swap<_Compare>(__x2, __x9, __c); cond_swap<_Compare>(__x4, __x19, __c); cond_swap<_Compare>(__x6, __x21, __c); cond_swap<_Compare>(__x8, __x23, __c); cond_swap<_Compare>(__x10, __x25, __c); cond_swap<_Compare>(__x11, __x13, __c); cond_swap<_Compare>(__x12, __x27, __c); cond_swap<_Compare>(__x14, __x16, __c); cond_swap<_Compare>(__x15, __x17, __c); cond_swap<_Compare>(__x18, __x20, __c); cond_swap<_Compare>(__x22, __x29, __c);
    // Layer 12: [(1,2),(3,10),(5,12),(7,14),(9,16),(11,18),(13,20),(15,22),(17,24),(19,26),(27,28)]
    cond_swap<_Compare>(__x2, __x3, __c); cond_swap<_Compare>(__x4, __x11, __c); cond_swap<_Compare>(__x6, __x13, __c); cond_swap<_Compare>(__x8, __x15, __c); cond_swap<_Compare>(__x10, __x17, __c); cond_swap<_Compare>(__x12, __x19, __c); cond_swap<_Compare>(__x14, __x21, __c); cond_swap<_Compare>(__x16, __x23, __c); cond_swap<_Compare>(__x18, __x25, __c); cond_swap<_Compare>(__x20, __x27, __c); cond_swap<_Compare>(__x28, __x29, __c);
    // Layer 13: [(2,4),(3,6),(5,8),(7,10),(9,12),(11,14),(13,16),(15,18),(17,20),(19,22),(21,24),(23,26),(25,27)]
    cond_swap<_Compare>(__x3, __x5, __c); cond_swap<_Compare>(__x4, __x7, __c); cond_swap<_Compare>(__x6, __x9, __c); cond_swap<_Compare>(__x8, __x11, __c); cond_swap<_Compare>(__x10, __x13, __c); cond_swap<_Compare>(__x12, __x15, __c); cond_swap<_Compare>(__x14, __x17, __c); cond_swap<_Compare>(__x16, __x19, __c); cond_swap<_Compare>(__x18, __x21, __c); cond_swap<_Compare>(__x20, __x23, __c); cond_swap<_Compare>(__x22, __x25, __c); cond_swap<_Compare>(__x24, __x27, __c); cond_swap<_Compare>(__x26, __x28, __c);
    // Layer 14: [(3,4),(5,6),(7,8),(9,10),(11,12),(13,14),(15,16),(17,18),(19,20),(21,22),(23,24),(25,26)]
    cond_swap<_Compare>(__x4, __x5, __c); cond_swap<_Compare>(__x6, __x7, __c); cond_swap<_Compare>(__x8, __x9, __c); cond_swap<_Compare>(__x10, __x11, __c); cond_swap<_Compare>(__x12, __x13, __c); cond_swap<_Compare>(__x14, __x15, __c); cond_swap<_Compare>(__x16, __x17, __c); cond_swap<_Compare>(__x18, __x19, __c); cond_swap<_Compare>(__x20, __x21, __c); cond_swap<_Compare>(__x22, __x23, __c); cond_swap<_Compare>(__x24, __x25, __c); cond_swap<_Compare>(__x26, __x27, __c);
}

// N=31 (S(31)=180, depth 14)
// Rete: Dal testo fornito
template <class, class _Compare, class _RandomAccessIterator>
inline void
sort31branchless(_RandomAccessIterator __x1, _RandomAccessIterator __x2, _RandomAccessIterator __x3,
                 _RandomAccessIterator __x4, _RandomAccessIterator __x5, _RandomAccessIterator __x6,
                 _RandomAccessIterator __x7, _RandomAccessIterator __x8, _RandomAccessIterator __x9,
                 _RandomAccessIterator __x10, _RandomAccessIterator __x11, _RandomAccessIterator __x12,
                 _RandomAccessIterator __x13, _RandomAccessIterator __x14, _RandomAccessIterator __x15,
                 _RandomAccessIterator __x16, _RandomAccessIterator __x17, _RandomAccessIterator __x18,
                 _RandomAccessIterator __x19, _RandomAccessIterator __x20, _RandomAccessIterator __x21,
                 _RandomAccessIterator __x22, _RandomAccessIterator __x23, _RandomAccessIterator __x24,
                 _RandomAccessIterator __x25, _RandomAccessIterator __x26, _RandomAccessIterator __x27,
                 _RandomAccessIterator __x28, _RandomAccessIterator __x29, _RandomAccessIterator __x30,
                 _RandomAccessIterator __x31,
                 _Compare __c) {
    // Layer 1: [(0,1),(2,3),(4,5),(6,7),(8,9),(10,11),(12,13),(14,15),(16,17),(18,19),(20,21),(22,23),(24,25),(26,27),(28,29)]
    cond_swap<_Compare>(__x1, __x2, __c); cond_swap<_Compare>(__x3, __x4, __c); cond_swap<_Compare>(__x5, __x6, __c); cond_swap<_Compare>(__x7, __x8, __c); cond_swap<_Compare>(__x9, __x10, __c); cond_swap<_Compare>(__x11, __x12, __c); cond_swap<_Compare>(__x13, __x14, __c); cond_swap<_Compare>(__x15, __x16, __c); cond_swap<_Compare>(__x17, __x18, __c); cond_swap<_Compare>(__x19, __x20, __c); cond_swap<_Compare>(__x21, __x22, __c); cond_swap<_Compare>(__x23, __x24, __c); cond_swap<_Compare>(__x25, __x26, __c); cond_swap<_Compare>(__x27, __x28, __c); cond_swap<_Compare>(__x29, __x30, __c);
    // Layer 2: [(0,2),(1,3),(4,6),(5,7),(8,10),(9,11),(12,14),(13,15),(16,18),(17,19),(20,22),(21,23),(24,26),(25,27),(28,30)]
    cond_swap<_Compare>(__x1, __x3, __c); cond_swap<_Compare>(__x2, __x4, __c); cond_swap<_Compare>(__x5, __x7, __c); cond_swap<_Compare>(__x6, __x8, __c); cond_swap<_Compare>(__x9, __x11, __c); cond_swap<_Compare>(__x10, __x12, __c); cond_swap<_Compare>(__x13, __x15, __c); cond_swap<_Compare>(__x14, __x16, __c); cond_swap<_Compare>(__x17, __x19, __c); cond_swap<_Compare>(__x18, __x20, __c); cond_swap<_Compare>(__x21, __x23, __c); cond_swap<_Compare>(__x22, __x24, __c); cond_swap<_Compare>(__x25, __x27, __c); cond_swap<_Compare>(__x26, __x28, __c); cond_swap<_Compare>(__x29, __x31, __c);
    // Layer 3: [(0,4),(1,5),(2,6),(3,7),(8,12),(9,13),(10,14),(11,15),(16,20),(17,21),(18,22),(19,23),(24,28),(25,29),(26,30)]
    cond_swap<_Compare>(__x1, __x5, __c); cond_swap<_Compare>(__x2, __x6, __c); cond_swap<_Compare>(__x3, __x7, __c); cond_swap<_Compare>(__x4, __x8, __c); cond_swap<_Compare>(__x9, __x13, __c); cond_swap<_Compare>(__x10, __x14, __c); cond_swap<_Compare>(__x11, __x15, __c); cond_swap<_Compare>(__x12, __x16, __c); cond_swap<_Compare>(__x17, __x21, __c); cond_swap<_Compare>(__x18, __x22, __c); cond_swap<_Compare>(__x19, __x23, __c); cond_swap<_Compare>(__x20, __x24, __c); cond_swap<_Compare>(__x25, __x29, __c); cond_swap<_Compare>(__x26, __x30, __c); cond_swap<_Compare>(__x27, __x31, __c);
    // Layer 4: [(0,8),(1,9),(2,10),(3,11),(4,12),(5,13),(6,14),(7,15),(16,24),(17,25),(18,26),(19,27),(20,28),(21,29),(22,30)]
    cond_swap<_Compare>(__x1, __x9, __c); cond_swap<_Compare>(__x2, __x10, __c); cond_swap<_Compare>(__x3, __x11, __c); cond_swap<_Compare>(__x4, __x12, __c); cond_swap<_Compare>(__x5, __x13, __c); cond_swap<_Compare>(__x6, __x14, __c); cond_swap<_Compare>(__x7, __x15, __c); cond_swap<_Compare>(__x8, __x16, __c); cond_swap<_Compare>(__x17, __x25, __c); cond_swap<_Compare>(__x18, __x26, __c); cond_swap<_Compare>(__x19, __x27, __c); cond_swap<_Compare>(__x20, __x28, __c); cond_swap<_Compare>(__x21, __x29, __c); cond_swap<_Compare>(__x22, __x30, __c); cond_swap<_Compare>(__x23, __x31, __c);
    // Layer 5: [(0,16),(1,8),(2,4),(3,12),(5,10),(6,9),(7,14),(11,13),(17,24),(18,20),(19,28),(21,26),(22,25),(23,30),(27,29)]
    cond_swap<_Compare>(__x1, __x17, __c); cond_swap<_Compare>(__x2, __x9, __c); cond_swap<_Compare>(__x3, __x5, __c); cond_swap<_Compare>(__x4, __x13, __c); cond_swap<_Compare>(__x6, __x11, __c); cond_swap<_Compare>(__x7, __x10, __c); cond_swap<_Compare>(__x8, __x15, __c); cond_swap<_Compare>(__x12, __x14, __c); cond_swap<_Compare>(__x18, __x25, __c); cond_swap<_Compare>(__x19, __x21, __c); cond_swap<_Compare>(__x20, __x29, __c); cond_swap<_Compare>(__x22, __x27, __c); cond_swap<_Compare>(__x23, __x26, __c); cond_swap<_Compare>(__x24, __x31, __c); cond_swap<_Compare>(__x28, __x30, __c);
    // Layer 6: [(1,2),(3,5),(4,8),(6,22),(7,11),(9,25),(10,12),(13,14),(17,18),(19,21),(20,24),(23,27),(26,28),(29,30)]
    cond_swap<_Compare>(__x2, __x3, __c); cond_swap<_Compare>(__x4, __x6, __c); cond_swap<_Compare>(__x5, __x9, __c); cond_swap<_Compare>(__x7, __x23, __c); cond_swap<_Compare>(__x8, __x12, __c); cond_swap<_Compare>(__x10, __x26, __c); cond_swap<_Compare>(__x11, __x13, __c); cond_swap<_Compare>(__x14, __x15, __c); cond_swap<_Compare>(__x18, __x19, __c); cond_swap<_Compare>(__x20, __x22, __c); cond_swap<_Compare>(__x21, __x25, __c); cond_swap<_Compare>(__x24, __x28, __c); cond_swap<_Compare>(__x27, __x29, __c); cond_swap<_Compare>(__x30, __x31, __c);
    // Layer 7: [(1,17),(2,18),(3,19),(4,20),(5,10),(7,23),(8,24),(11,27),(12,28),(13,29),(14,30),(21,26)]
    cond_swap<_Compare>(__x2, __x18, __c); cond_swap<_Compare>(__x3, __x19, __c); cond_swap<_Compare>(__x4, __x20, __c); cond_swap<_Compare>(__x5, __x21, __c); cond_swap<_Compare>(__x6, __x11, __c); cond_swap<_Compare>(__x8, __x24, __c); cond_swap<_Compare>(__x9, __x25, __c); cond_swap<_Compare>(__x12, __x28, __c); cond_swap<_Compare>(__x13, __x29, __c); cond_swap<_Compare>(__x14, __x30, __c); cond_swap<_Compare>(__x15, __x31, __c); cond_swap<_Compare>(__x22, __x27, __c);
    // Layer 8: [(3,17),(4,16),(5,21),(6,18),(7,9),(8,20),(10,26),(11,23),(13,25),(14,28),(15,27),(22,24)]
    cond_swap<_Compare>(__x4, __x18, __c); cond_swap<_Compare>(__x5, __x17, __c); cond_swap<_Compare>(__x6, __x22, __c); cond_swap<_Compare>(__x7, __x19, __c); cond_swap<_Compare>(__x8, __x10, __c); cond_swap<_Compare>(__x9, __x21, __c); cond_swap<_Compare>(__x11, __x27, __c); cond_swap<_Compare>(__x12, __x24, __c); cond_swap<_Compare>(__x14, __x26, __c); cond_swap<_Compare>(__x15, __x29, __c); cond_swap<_Compare>(__x16, __x28, __c); cond_swap<_Compare>(__x23, __x25, __c);
    // Layer 9: [(1,4),(3,8),(5,16),(7,17),(9,21),(10,22),(11,19),(12,20),(14,24),(15,26),(23,28),(27,30)]
    cond_swap<_Compare>(__x2, __x5, __c); cond_swap<_Compare>(__x4, __x9, __c); cond_swap<_Compare>(__x6, __x17, __c); cond_swap<_Compare>(__x8, __x18, __c); cond_swap<_Compare>(__x10, __x22, __c); cond_swap<_Compare>(__x11, __x23, __c); cond_swap<_Compare>(__x12, __x20, __c); cond_swap<_Compare>(__x13, __x21, __c); cond_swap<_Compare>(__x15, __x25, __c); cond_swap<_Compare>(__x16, __x27, __c); cond_swap<_Compare>(__x24, __x29, __c); cond_swap<_Compare>(__x28, __x31, __c);
    // Layer 10: [(2,5),(7,8),(9,18),(11,17),(12,16),(13,22),(14,20),(15,19),(23,24),(26,29)]
    cond_swap<_Compare>(__x3, __x6, __c); cond_swap<_Compare>(__x8, __x9, __c); cond_swap<_Compare>(__x10, __x19, __c); cond_swap<_Compare>(__x12, __x18, __c); cond_swap<_Compare>(__x13, __x17, __c); cond_swap<_Compare>(__x14, __x23, __c); cond_swap<_Compare>(__x15, __x21, __c); cond_swap<_Compare>(__x16, __x20, __c); cond_swap<_Compare>(__x24, __x25, __c); cond_swap<_Compare>(__x27, __x30, __c);
    // Layer 11: [(2,4),(6,12),(9,16),(10,11),(13,17),(14,18),(15,22),(19,25),(20,21),(27,29)]
    cond_swap<_Compare>(__x3, __x5, __c); cond_swap<_Compare>(__x7, __x13, __c); cond_swap<_Compare>(__x10, __x17, __c); cond_swap<_Compare>(__x11, __x12, __c); cond_swap<_Compare>(__x14, __x18, __c); cond_swap<_Compare>(__x15, __x19, __c); cond_swap<_Compare>(__x16, __x23, __c); cond_swap<_Compare>(__x20, __x26, __c); cond_swap<_Compare>(__x21, __x22, __c); cond_swap<_Compare>(__x28, __x30, __c);
    // Layer 12: [(5,6),(8,12),(9,10),(11,13),(14,16),(15,17),(18,20),(19,23),(21,22),(25,26)]
    cond_swap<_Compare>(__x6, __x7, __c); cond_swap<_Compare>(__x9, __x13, __c); cond_swap<_Compare>(__x10, __x11, __c); cond_swap<_Compare>(__x12, __x14, __c); cond_swap<_Compare>(__x15, __x17, __c); cond_swap<_Compare>(__x16, __x18, __c); cond_swap<_Compare>(__x19, __x21, __c); cond_swap<_Compare>(__x20, __x24, __c); cond_swap<_Compare>(__x22, __x23, __c); cond_swap<_Compare>(__x26, __x27, __c);
    // Layer 13: [(3,5),(6,7),(8,9),(10,12),(11,14),(13,16),(15,18),(17,20),(19,21),(22,23),(24,25),(26,28)]
    cond_swap<_Compare>(__x4, __x6, __c); cond_swap<_Compare>(__x7, __x8, __c); cond_swap<_Compare>(__x9, __x10, __c); cond_swap<_Compare>(__x11, __x13, __c); cond_swap<_Compare>(__x12, __x15, __c); cond_swap<_Compare>(__x14, __x17, __c); cond_swap<_Compare>(__x16, __x19, __c); cond_swap<_Compare>(__x18, __x21, __c); cond_swap<_Compare>(__x20, __x22, __c); cond_swap<_Compare>(__x23, __x24, __c); cond_swap<_Compare>(__x25, __x26, __c); cond_swap<_Compare>(__x27, __x29, __c);
    // Layer 14: [(3,4),(5,6),(7,8),(9,10),(11,12),(13,14),(15,16),(17,18),(19,20),(21,22),(23,24),(25,26),(27,28)]
    cond_swap<_Compare>(__x4, __x5, __c); cond_swap<_Compare>(__x6, __x7, __c); cond_swap<_Compare>(__x8, __x9, __c); cond_swap<_Compare>(__x10, __x11, __c); cond_swap<_Compare>(__x12, __x13, __c); cond_swap<_Compare>(__x14, __x15, __c); cond_swap<_Compare>(__x16, __x17, __c); cond_swap<_Compare>(__x18, __x19, __c); cond_swap<_Compare>(__x20, __x21, __c); cond_swap<_Compare>(__x22, __x23, __c); cond_swap<_Compare>(__x24, __x25, __c); cond_swap<_Compare>(__x26, __x27, __c); cond_swap<_Compare>(__x28, __x29, __c);
}

// N=32 (S(32)=185, depth 14)
// Rete: Dal testo fornito
template <class, class _Compare, class _RandomAccessIterator>
inline void
sort32branchless(_RandomAccessIterator __x1, _RandomAccessIterator __x2, _RandomAccessIterator __x3,
                 _RandomAccessIterator __x4, _RandomAccessIterator __x5, _RandomAccessIterator __x6,
                 _RandomAccessIterator __x7, _RandomAccessIterator __x8, _RandomAccessIterator __x9,
                 _RandomAccessIterator __x10, _RandomAccessIterator __x11, _RandomAccessIterator __x12,
                 _RandomAccessIterator __x13, _RandomAccessIterator __x14, _RandomAccessIterator __x15,
                 _RandomAccessIterator __x16, _RandomAccessIterator __x17, _RandomAccessIterator __x18,
                 _RandomAccessIterator __x19, _RandomAccessIterator __x20, _RandomAccessIterator __x21,
                 _RandomAccessIterator __x22, _RandomAccessIterator __x23, _RandomAccessIterator __x24,
                 _RandomAccessIterator __x25, _RandomAccessIterator __x26, _RandomAccessIterator __x27,
                 _RandomAccessIterator __x28, _RandomAccessIterator __x29, _RandomAccessIterator __x30,
                 _RandomAccessIterator __x31, _RandomAccessIterator __x32,
                 _Compare __c) {
    // Layer 1: [(0,1),(2,3),(4,5),(6,7),(8,9),(10,11),(12,13),(14,15),(16,17),(18,19),(20,21),(22,23),(24,25),(26,27),(28,29),(30,31)]
    cond_swap<_Compare>(__x1, __x2, __c); cond_swap<_Compare>(__x3, __x4, __c); cond_swap<_Compare>(__x5, __x6, __c); cond_swap<_Compare>(__x7, __x8, __c); cond_swap<_Compare>(__x9, __x10, __c); cond_swap<_Compare>(__x11, __x12, __c); cond_swap<_Compare>(__x13, __x14, __c); cond_swap<_Compare>(__x15, __x16, __c); cond_swap<_Compare>(__x17, __x18, __c); cond_swap<_Compare>(__x19, __x20, __c); cond_swap<_Compare>(__x21, __x22, __c); cond_swap<_Compare>(__x23, __x24, __c); cond_swap<_Compare>(__x25, __x26, __c); cond_swap<_Compare>(__x27, __x28, __c); cond_swap<_Compare>(__x29, __x30, __c); cond_swap<_Compare>(__x31, __x32, __c);
    // Layer 2: [(0,2),(1,3),(4,6),(5,7),(8,10),(9,11),(12,14),(13,15),(16,18),(17,19),(20,22),(21,23),(24,26),(25,27),(28,30),(29,31)]
    cond_swap<_Compare>(__x1, __x3, __c); cond_swap<_Compare>(__x2, __x4, __c); cond_swap<_Compare>(__x5, __x7, __c); cond_swap<_Compare>(__x6, __x8, __c); cond_swap<_Compare>(__x9, __x11, __c); cond_swap<_Compare>(__x10, __x12, __c); cond_swap<_Compare>(__x13, __x15, __c); cond_swap<_Compare>(__x14, __x16, __c); cond_swap<_Compare>(__x17, __x19, __c); cond_swap<_Compare>(__x18, __x20, __c); cond_swap<_Compare>(__x21, __x23, __c); cond_swap<_Compare>(__x22, __x24, __c); cond_swap<_Compare>(__x25, __x27, __c); cond_swap<_Compare>(__x26, __x28, __c); cond_swap<_Compare>(__x29, __x31, __c); cond_swap<_Compare>(__x30, __x32, __c);
    // Layer 3: [(0,4),(1,5),(2,6),(3,7),(8,12),(9,13),(10,14),(11,15),(16,20),(17,21),(18,22),(19,23),(24,28),(25,29),(26,30),(27,31)]
    cond_swap<_Compare>(__x1, __x5, __c); cond_swap<_Compare>(__x2, __x6, __c); cond_swap<_Compare>(__x3, __x7, __c); cond_swap<_Compare>(__x4, __x8, __c); cond_swap<_Compare>(__x9, __x13, __c); cond_swap<_Compare>(__x10, __x14, __c); cond_swap<_Compare>(__x11, __x15, __c); cond_swap<_Compare>(__x12, __x16, __c); cond_swap<_Compare>(__x17, __x21, __c); cond_swap<_Compare>(__x18, __x22, __c); cond_swap<_Compare>(__x19, __x23, __c); cond_swap<_Compare>(__x20, __x24, __c); cond_swap<_Compare>(__x25, __x29, __c); cond_swap<_Compare>(__x26, __x30, __c); cond_swap<_Compare>(__x27, __x31, __c); cond_swap<_Compare>(__x28, __x32, __c);
    // Layer 4: [(0,8),(1,9),(2,10),(3,11),(4,12),(5,13),(6,14),(7,15),(16,24),(17,25),(18,26),(19,27),(20,28),(21,29),(22,30),(23,31)]
    cond_swap<_Compare>(__x1, __x9, __c); cond_swap<_Compare>(__x2, __x10, __c); cond_swap<_Compare>(__x3, __x11, __c); cond_swap<_Compare>(__x4, __x12, __c); cond_swap<_Compare>(__x5, __x13, __c); cond_swap<_Compare>(__x6, __x14, __c); cond_swap<_Compare>(__x7, __x15, __c); cond_swap<_Compare>(__x8, __x16, __c); cond_swap<_Compare>(__x17, __x25, __c); cond_swap<_Compare>(__x18, __x26, __c); cond_swap<_Compare>(__x19, __x27, __c); cond_swap<_Compare>(__x20, __x28, __c); cond_swap<_Compare>(__x21, __x29, __c); cond_swap<_Compare>(__x22, __x30, __c); cond_swap<_Compare>(__x23, __x31, __c); cond_swap<_Compare>(__x24, __x32, __c);
    // Layer 5: [(0,16),(1,8),(2,4),(3,12),(5,10),(6,9),(7,14),(11,13),(15,31),(17,24),(18,20),(19,28),(21,26),(22,25),(23,30),(27,29)]
    cond_swap<_Compare>(__x1, __x17, __c); cond_swap<_Compare>(__x2, __x9, __c); cond_swap<_Compare>(__x3, __x5, __c); cond_swap<_Compare>(__x4, __x13, __c); cond_swap<_Compare>(__x6, __x11, __c); cond_swap<_Compare>(__x7, __x10, __c); cond_swap<_Compare>(__x8, __x15, __c); cond_swap<_Compare>(__x12, __x14, __c); cond_swap<_Compare>(__x16, __x32, __c); cond_swap<_Compare>(__x18, __x25, __c); cond_swap<_Compare>(__x19, __x21, __c); cond_swap<_Compare>(__x20, __x29, __c); cond_swap<_Compare>(__x22, __x27, __c); cond_swap<_Compare>(__x23, __x26, __c); cond_swap<_Compare>(__x24, __x31, __c); cond_swap<_Compare>(__x28, __x30, __c);
    // Layer 6: [(1,2),(3,5),(4,8),(6,22),(7,11),(9,25),(10,12),(13,14),(17,18),(19,21),(20,24),(23,27),(26,28),(29,30)]
    cond_swap<_Compare>(__x2, __x3, __c); cond_swap<_Compare>(__x4, __x6, __c); cond_swap<_Compare>(__x5, __x9, __c); cond_swap<_Compare>(__x7, __x23, __c); cond_swap<_Compare>(__x8, __x12, __c); cond_swap<_Compare>(__x10, __x26, __c); cond_swap<_Compare>(__x11, __x13, __c); cond_swap<_Compare>(__x14, __x15, __c); cond_swap<_Compare>(__x18, __x19, __c); cond_swap<_Compare>(__x20, __x22, __c); cond_swap<_Compare>(__x21, __x25, __c); cond_swap<_Compare>(__x24, __x28, __c); cond_swap<_Compare>(__x27, __x29, __c); cond_swap<_Compare>(__x30, __x31, __c);
    // Layer 7: [(1,17),(2,18),(3,19),(4,20),(5,10),(7,23),(8,24),(11,27),(12,28),(13,29),(14,30),(21,26)]
    cond_swap<_Compare>(__x2, __x18, __c); cond_swap<_Compare>(__x3, __x19, __c); cond_swap<_Compare>(__x4, __x20, __c); cond_swap<_Compare>(__x5, __x21, __c); cond_swap<_Compare>(__x6, __x11, __c); cond_swap<_Compare>(__x8, __x24, __c); cond_swap<_Compare>(__x9, __x25, __c); cond_swap<_Compare>(__x12, __x28, __c); cond_swap<_Compare>(__x13, __x29, __c); cond_swap<_Compare>(__x14, __x30, __c); cond_swap<_Compare>(__x15, __x31, __c); cond_swap<_Compare>(__x22, __x27, __c);
    // Layer 8: [(3,17),(4,16),(5,21),(6,18),(7,9),(8,20),(10,26),(11,23),(13,25),(14,28),(15,27),(22,24)]
    cond_swap<_Compare>(__x4, __x18, __c); cond_swap<_Compare>(__x5, __x17, __c); cond_swap<_Compare>(__x6, __x22, __c); cond_swap<_Compare>(__x7, __x19, __c); cond_swap<_Compare>(__x8, __x10, __c); cond_swap<_Compare>(__x9, __x21, __c); cond_swap<_Compare>(__x11, __x27, __c); cond_swap<_Compare>(__x12, __x24, __c); cond_swap<_Compare>(__x14, __x26, __c); cond_swap<_Compare>(__x15, __x29, __c); cond_swap<_Compare>(__x16, __x28, __c); cond_swap<_Compare>(__x23, __x25, __c);
    // Layer 9: [(1,4),(3,8),(5,16),(7,17),(9,21),(10,22),(11,19),(12,20),(14,24),(15,26),(23,28),(27,30)]
    cond_swap<_Compare>(__x2, __x5, __c); cond_swap<_Compare>(__x4, __x9, __c); cond_swap<_Compare>(__x6, __x17, __c); cond_swap<_Compare>(__x8, __x18, __c); cond_swap<_Compare>(__x10, __x22, __c); cond_swap<_Compare>(__x11, __x23, __c); cond_swap<_Compare>(__x12, __x20, __c); cond_swap<_Compare>(__x13, __x21, __c); cond_swap<_Compare>(__x15, __x25, __c); cond_swap<_Compare>(__x16, __x27, __c); cond_swap<_Compare>(__x24, __x29, __c); cond_swap<_Compare>(__x28, __x31, __c);
    // Layer 10: [(2,5),(7,8),(9,18),(11,17),(12,16),(13,22),(14,20),(15,19),(23,24),(26,29)]
    cond_swap<_Compare>(__x3, __x6, __c); cond_swap<_Compare>(__x8, __x9, __c); cond_swap<_Compare>(__x10, __x19, __c); cond_swap<_Compare>(__x12, __x18, __c); cond_swap<_Compare>(__x13, __x17, __c); cond_swap<_Compare>(__x14, __x23, __c); cond_swap<_Compare>(__x15, __x21, __c); cond_swap<_Compare>(__x16, __x20, __c); cond_swap<_Compare>(__x24, __x25, __c); cond_swap<_Compare>(__x27, __x30, __c);
    // Layer 11: [(2,4),(6,12),(9,16),(10,11),(13,17),(14,18),(15,22),(19,25),(20,21),(27,29)]
    cond_swap<_Compare>(__x3, __x5, __c); cond_swap<_Compare>(__x7, __x13, __c); cond_swap<_Compare>(__x10, __x17, __c); cond_swap<_Compare>(__x11, __x12, __c); cond_swap<_Compare>(__x14, __x18, __c); cond_swap<_Compare>(__x15, __x19, __c); cond_swap<_Compare>(__x16, __x23, __c); cond_swap<_Compare>(__x20, __x26, __c); cond_swap<_Compare>(__x21, __x22, __c); cond_swap<_Compare>(__x28, __x30, __c);
    // Layer 12: [(5,6),(8,12),(9,10),(11,13),(14,16),(15,17),(18,20),(19,23),(21,22),(25,26)]
    cond_swap<_Compare>(__x6, __x7, __c); cond_swap<_Compare>(__x9, __x13, __c); cond_swap<_Compare>(__x10, __x11, __c); cond_swap<_Compare>(__x12, __x14, __c); cond_swap<_Compare>(__x15, __x17, __c); cond_swap<_Compare>(__x16, __x18, __c); cond_swap<_Compare>(__x19, __x21, __c); cond_swap<_Compare>(__x20, __x24, __c); cond_swap<_Compare>(__x22, __x23, __c); cond_swap<_Compare>(__x26, __x27, __c);
    // Layer 13: [(3,5),(6,7),(8,9),(10,12),(11,14),(13,16),(15,18),(17,20),(19,21),(22,23),(24,25),(26,28)]
    cond_swap<_Compare>(__x4, __x6, __c); cond_swap<_Compare>(__x7, __x8, __c); cond_swap<_Compare>(__x9, __x10, __c); cond_swap<_Compare>(__x11, __x13, __c); cond_swap<_Compare>(__x12, __x15, __c); cond_swap<_Compare>(__x14, __x17, __c); cond_swap<_Compare>(__x16, __x19, __c); cond_swap<_Compare>(__x18, __x21, __c); cond_swap<_Compare>(__x20, __x22, __c); cond_swap<_Compare>(__x23, __x24, __c); cond_swap<_Compare>(__x25, __x26, __c); cond_swap<_Compare>(__x27, __x29, __c);
    // Layer 14: [(3,4),(5,6),(7,8),(9,10),(11,12),(13,14),(15,16),(17,18),(19,20),(21,22),(23,24),(25,26),(27,28)]
    cond_swap<_Compare>(__x4, __x5, __c); cond_swap<_Compare>(__x6, __x7, __c); cond_swap<_Compare>(__x8, __x9, __c); cond_swap<_Compare>(__x10, __x11, __c); cond_swap<_Compare>(__x12, __x13, __c); cond_swap<_Compare>(__x14, __x15, __c); cond_swap<_Compare>(__x16, __x17, __c); cond_swap<_Compare>(__x18, __x19, __c); cond_swap<_Compare>(__x20, __x21, __c); cond_swap<_Compare>(__x22, __x23, __c); cond_swap<_Compare>(__x24, __x25, __c); cond_swap<_Compare>(__x26, __x27, __c); cond_swap<_Compare>(__x28, __x29, __c);
}

// N=64 (S(64)=525, depth 20)
// Rete: Dal testo fornito
template <class, class _Compare, class _RandomAccessIterator>
inline void
sort64branchless(_RandomAccessIterator __x1, _RandomAccessIterator __x2, _RandomAccessIterator __x3, _RandomAccessIterator __x4, _RandomAccessIterator __x5, _RandomAccessIterator __x6, _RandomAccessIterator __x7, _RandomAccessIterator __x8,
                 _RandomAccessIterator __x9, _RandomAccessIterator __x10, _RandomAccessIterator __x11, _RandomAccessIterator __x12, _RandomAccessIterator __x13, _RandomAccessIterator __x14, _RandomAccessIterator __x15, _RandomAccessIterator __x16,
                 _RandomAccessIterator __x17, _RandomAccessIterator __x18, _RandomAccessIterator __x19, _RandomAccessIterator __x20, _RandomAccessIterator __x21, _RandomAccessIterator __x22, _RandomAccessIterator __x23, _RandomAccessIterator __x24,
                 _RandomAccessIterator __x25, _RandomAccessIterator __x26, _RandomAccessIterator __x27, _RandomAccessIterator __x28, _RandomAccessIterator __x29, _RandomAccessIterator __x30, _RandomAccessIterator __x31, _RandomAccessIterator __x32,
                 _RandomAccessIterator __x33, _RandomAccessIterator __x34, _RandomAccessIterator __x35, _RandomAccessIterator __x36, _RandomAccessIterator __x37, _RandomAccessIterator __x38, _RandomAccessIterator __x39, _RandomAccessIterator __x40,
                 _RandomAccessIterator __x41, _RandomAccessIterator __x42, _RandomAccessIterator __x43, _RandomAccessIterator __x44, _RandomAccessIterator __x45, _RandomAccessIterator __x46, _RandomAccessIterator __x47, _RandomAccessIterator __x48,
                 _RandomAccessIterator __x49, _RandomAccessIterator __x50, _RandomAccessIterator __x51, _RandomAccessIterator __x52, _RandomAccessIterator __x53, _RandomAccessIterator __x54, _RandomAccessIterator __x55, _RandomAccessIterator __x56,
                 _RandomAccessIterator __x57, _RandomAccessIterator __x58, _RandomAccessIterator __x59, _RandomAccessIterator __x60, _RandomAccessIterator __x61, _RandomAccessIterator __x62, _RandomAccessIterator __x63, _RandomAccessIterator __x64,
                 _Compare __c) {
    // Layer 1: [(0,2),(1,3),(4,6),(5,7),(8,10),(9,11),(12,14),(13,15),(16,18),(17,19),(20,22),(21,23),(24,26),(25,27),(28,30),(29,31),(32,34),(33,35),(36,38),(37,39),(40,42),(41,43),(44,46),(45,47),(48,50),(49,51),(52,54),(53,55),(56,58),(57,59),(60,62),(61,63)]
    cond_swap<_Compare>(__x1, __x3, __c); cond_swap<_Compare>(__x2, __x4, __c); cond_swap<_Compare>(__x5, __x7, __c); cond_swap<_Compare>(__x6, __x8, __c); cond_swap<_Compare>(__x9, __x11, __c); cond_swap<_Compare>(__x10, __x12, __c); cond_swap<_Compare>(__x13, __x15, __c); cond_swap<_Compare>(__x14, __x16, __c); cond_swap<_Compare>(__x17, __x19, __c); cond_swap<_Compare>(__x18, __x20, __c); cond_swap<_Compare>(__x21, __x23, __c); cond_swap<_Compare>(__x22, __x24, __c); cond_swap<_Compare>(__x25, __x27, __c); cond_swap<_Compare>(__x26, __x28, __c); cond_swap<_Compare>(__x29, __x31, __c); cond_swap<_Compare>(__x30, __x32, __c); cond_swap<_Compare>(__x33, __x35, __c); cond_swap<_Compare>(__x34, __x36, __c); cond_swap<_Compare>(__x37, __x39, __c); cond_swap<_Compare>(__x38, __x40, __c); cond_swap<_Compare>(__x41, __x43, __c); cond_swap<_Compare>(__x42, __x44, __c); cond_swap<_Compare>(__x45, __x47, __c); cond_swap<_Compare>(__x46, __x48, __c); cond_swap<_Compare>(__x49, __x51, __c); cond_swap<_Compare>(__x50, __x52, __c); cond_swap<_Compare>(__x53, __x55, __c); cond_swap<_Compare>(__x54, __x56, __c); cond_swap<_Compare>(__x57, __x59, __c); cond_swap<_Compare>(__x58, __x60, __c); cond_swap<_Compare>(__x61, __x63, __c); cond_swap<_Compare>(__x62, __x64, __c);
    // Layer 2: [(0,1),(2,3),(4,5),(6,7),(8,9),(10,11),(12,13),(14,15),(16,17),(18,19),(20,21),(22,23),(24,25),(26,27),(28,29),(30,31),(32,33),(34,35),(36,37),(38,39),(40,41),(42,43),(44,45),(46,47),(48,49),(50,51),(52,53),(54,55),(56,57),(58,59),(60,61),(62,63)]
    cond_swap<_Compare>(__x1, __x2, __c); cond_swap<_Compare>(__x3, __x4, __c); cond_swap<_Compare>(__x5, __x6, __c); cond_swap<_Compare>(__x7, __x8, __c); cond_swap<_Compare>(__x9, __x10, __c); cond_swap<_Compare>(__x11, __x12, __c); cond_swap<_Compare>(__x13, __x14, __c); cond_swap<_Compare>(__x15, __x16, __c); cond_swap<_Compare>(__x17, __x18, __c); cond_swap<_Compare>(__x19, __x20, __c); cond_swap<_Compare>(__x21, __x22, __c); cond_swap<_Compare>(__x23, __x24, __c); cond_swap<_Compare>(__x25, __x26, __c); cond_swap<_Compare>(__x27, __x28, __c); cond_swap<_Compare>(__x29, __x30, __c); cond_swap<_Compare>(__x31, __x32, __c); cond_swap<_Compare>(__x33, __x34, __c); cond_swap<_Compare>(__x35, __x36, __c); cond_swap<_Compare>(__x37, __x38, __c); cond_swap<_Compare>(__x39, __x40, __c); cond_swap<_Compare>(__x41, __x42, __c); cond_swap<_Compare>(__x43, __x44, __c); cond_swap<_Compare>(__x45, __x46, __c); cond_swap<_Compare>(__x47, __x48, __c); cond_swap<_Compare>(__x49, __x50, __c); cond_swap<_Compare>(__x51, __x52, __c); cond_swap<_Compare>(__x53, __x54, __c); cond_swap<_Compare>(__x55, __x56, __c); cond_swap<_Compare>(__x57, __x58, __c); cond_swap<_Compare>(__x59, __x60, __c); cond_swap<_Compare>(__x61, __x62, __c); cond_swap<_Compare>(__x63, __x64, __c);
    // Layer 3: [(0,20),(1,2),(3,23),(4,16),(5,6),(7,19),(8,48),(9,10),(11,51),(12,52),(13,14),(15,55),(17,18),(21,22),(24,28),(25,26),(27,31),(29,30),(32,36),(33,34),(35,39),(37,38),(40,60),(41,42),(43,63),(44,56),(45,46),(47,59),(49,50),(53,54),(57,58),(61,62)]
    cond_swap<_Compare>(__x1, __x21, __c); cond_swap<_Compare>(__x2, __x3, __c); cond_swap<_Compare>(__x4, __x24, __c); cond_swap<_Compare>(__x5, __x17, __c); cond_swap<_Compare>(__x6, __x7, __c); cond_swap<_Compare>(__x8, __x20, __c); cond_swap<_Compare>(__x9, __x49, __c); cond_swap<_Compare>(__x10, __x11, __c); cond_swap<_Compare>(__x12, __x52, __c); cond_swap<_Compare>(__x13, __x53, __c); cond_swap<_Compare>(__x14, __x15, __c); cond_swap<_Compare>(__x16, __x56, __c); cond_swap<_Compare>(__x18, __x19, __c); cond_swap<_Compare>(__x22, __x23, __c); cond_swap<_Compare>(__x25, __x29, __c); cond_swap<_Compare>(__x26, __x27, __c); cond_swap<_Compare>(__x28, __x32, __c); cond_swap<_Compare>(__x30, __x31, __c); cond_swap<_Compare>(__x33, __x37, __c); cond_swap<_Compare>(__x34, __x35, __c); cond_swap<_Compare>(__x36, __x40, __c); cond_swap<_Compare>(__x38, __x39, __c); cond_swap<_Compare>(__x41, __x61, __c); cond_swap<_Compare>(__x42, __x43, __c); cond_swap<_Compare>(__x44, __x64, __c); cond_swap<_Compare>(__x45, __x57, __c); cond_swap<_Compare>(__x46, __x47, __c); cond_swap<_Compare>(__x48, __x60, __c); cond_swap<_Compare>(__x50, __x51, __c); cond_swap<_Compare>(__x54, __x55, __c); cond_swap<_Compare>(__x58, __x59, __c); cond_swap<_Compare>(__x62, __x63, __c);
    // Layer 4: [(0,8),(1,21),(2,22),(3,11),(4,40),(5,17),(6,18),(7,43),(9,49),(10,50),(12,24),(13,53),(14,54),(15,27),(16,28),(19,31),(20,56),(23,59),(25,29),(26,30),(32,44),(33,37),(34,38),(35,47),(36,48),(39,51),(41,61),(42,62),(45,57),(46,58),(52,60),(55,63)]
    cond_swap<_Compare>(__x1, __x9, __c); cond_swap<_Compare>(__x2, __x22, __c); cond_swap<_Compare>(__x3, __x23, __c); cond_swap<_Compare>(__x4, __x12, __c); cond_swap<_Compare>(__x5, __x41, __c); cond_swap<_Compare>(__x6, __x18, __c); cond_swap<_Compare>(__x7, __x19, __c); cond_swap<_Compare>(__x8, __x44, __c); cond_swap<_Compare>(__x10, __x50, __c); cond_swap<_Compare>(__x11, __x51, __c); cond_swap<_Compare>(__x13, __x25, __c); cond_swap<_Compare>(__x14, __x54, __c); cond_swap<_Compare>(__x15, __x55, __c); cond_swap<_Compare>(__x16, __x28, __c); cond_swap<_Compare>(__x17, __x29, __c); cond_swap<_Compare>(__x20, __x32, __c); cond_swap<_Compare>(__x21, __x57, __c); cond_swap<_Compare>(__x24, __x60, __c); cond_swap<_Compare>(__x26, __x30, __c); cond_swap<_Compare>(__x27, __x31, __c); cond_swap<_Compare>(__x33, __x45, __c); cond_swap<_Compare>(__x34, __x38, __c); cond_swap<_Compare>(__x35, __x39, __c); cond_swap<_Compare>(__x36, __x48, __c); cond_swap<_Compare>(__x37, __x49, __c); cond_swap<_Compare>(__x40, __x52, __c); cond_swap<_Compare>(__x42, __x62, __c); cond_swap<_Compare>(__x43, __x63, __c); cond_swap<_Compare>(__x46, __x58, __c); cond_swap<_Compare>(__x47, __x59, __c); cond_swap<_Compare>(__x53, __x61, __c); cond_swap<_Compare>(__x56, __x64, __c);
    // Layer 5: [(0,32),(1,9),(2,10),(3,35),(4,12),(5,41),(6,42),(7,15),(8,44),(11,47),(13,25),(14,26),(16,52),(17,29),(18,30),(19,55),(20,36),(21,57),(22,58),(23,39),(24,40),(27,43),(28,60),(31,63),(33,45),(34,46),(37,49),(38,50),(48,56),(51,59),(53,61),(54,62)]
    cond_swap<_Compare>(__x1, __x33, __c); cond_swap<_Compare>(__x2, __x10, __c); cond_swap<_Compare>(__x3, __x11, __c); cond_swap<_Compare>(__x4, __x36, __c); cond_swap<_Compare>(__x5, __x13, __c); cond_swap<_Compare>(__x6, __x42, __c); cond_swap<_Compare>(__x7, __x43, __c); cond_swap<_Compare>(__x8, __x16, __c); cond_swap<_Compare>(__x9, __x45, __c); cond_swap<_Compare>(__x12, __x48, __c); cond_swap<_Compare>(__x14, __x26, __c); cond_swap<_Compare>(__x15, __x27, __c); cond_swap<_Compare>(__x17, __x53, __c); cond_swap<_Compare>(__x18, __x30, __c); cond_swap<_Compare>(__x19, __x31, __c); cond_swap<_Compare>(__x20, __x56, __c); cond_swap<_Compare>(__x21, __x37, __c); cond_swap<_Compare>(__x22, __x58, __c); cond_swap<_Compare>(__x23, __x59, __c); cond_swap<_Compare>(__x24, __x40, __c); cond_swap<_Compare>(__x25, __x41, __c); cond_swap<_Compare>(__x28, __x44, __c); cond_swap<_Compare>(__x29, __x61, __c); cond_swap<_Compare>(__x32, __x64, __c); cond_swap<_Compare>(__x34, __x46, __c); cond_swap<_Compare>(__x35, __x47, __c); cond_swap<_Compare>(__x38, __x50, __c); cond_swap<_Compare>(__x39, __x51, __c); cond_swap<_Compare>(__x49, __x57, __c); cond_swap<_Compare>(__x52, __x60, __c); cond_swap<_Compare>(__x54, __x62, __c); cond_swap<_Compare>(__x55, __x63, __c);
    // Layer 6: [(0,4),(1,33),(2,34),(3,7),(5,13),(6,14),(8,16),(9,45),(10,46),(11,19),(12,32),(15,35),(17,53),(18,54),(20,24),(21,37),(22,38),(23,27),(25,41),(26,42),(28,48),(29,61),(30,62),(31,51),(36,40),(39,43),(44,52),(47,55),(49,57),(50,58),(56,60),(59,63)]
    cond_swap<_Compare>(__x1, __x5, __c); cond_swap<_Compare>(__x2, __x34, __c); cond_swap<_Compare>(__x3, __x35, __c); cond_swap<_Compare>(__x4, __x8, __c); cond_swap<_Compare>(__x6, __x14, __c); cond_swap<_Compare>(__x7, __x15, __c); cond_swap<_Compare>(__x9, __x17, __c); cond_swap<_Compare>(__x10, __x46, __c); cond_swap<_Compare>(__x11, __x47, __c); cond_swap<_Compare>(__x12, __x20, __c); cond_swap<_Compare>(__x13, __x33, __c); cond_swap<_Compare>(__x16, __x36, __c); cond_swap<_Compare>(__x18, __x54, __c); cond_swap<_Compare>(__x19, __x55, __c); cond_swap<_Compare>(__x21, __x25, __c); cond_swap<_Compare>(__x22, __x38, __c); cond_swap<_Compare>(__x23, __x39, __c); cond_swap<_Compare>(__x24, __x28, __c); cond_swap<_Compare>(__x26, __x42, __c); cond_swap<_Compare>(__x27, __x43, __c); cond_swap<_Compare>(__x29, __x49, __c); cond_swap<_Compare>(__x30, __x62, __c); cond_swap<_Compare>(__x31, __x63, __c); cond_swap<_Compare>(__x32, __x52, __c); cond_swap<_Compare>(__x37, __x41, __c); cond_swap<_Compare>(__x40, __x44, __c); cond_swap<_Compare>(__x45, __x53, __c); cond_swap<_Compare>(__x48, __x56, __c); cond_swap<_Compare>(__x50, __x58, __c); cond_swap<_Compare>(__x51, __x59, __c); cond_swap<_Compare>(__x57, __x61, __c); cond_swap<_Compare>(__x60, __x64, __c);
    // Layer 7: [(1,5),(2,6),(4,12),(7,15),(8,20),(9,17),(10,18),(11,23),(13,33),(14,34),(16,32),(19,35),(21,25),(22,26),(24,36),(27,39),(28,44),(29,49),(30,50),(31,47),(37,41),(38,42),(40,52),(43,55),(45,53),(46,54),(48,56),(51,59),(57,61),(58,62)]
    cond_swap<_Compare>(__x2, __x6, __c); cond_swap<_Compare>(__x3, __x7, __c); cond_swap<_Compare>(__x5, __x13, __c); cond_swap<_Compare>(__x8, __x16, __c); cond_swap<_Compare>(__x9, __x21, __c); cond_swap<_Compare>(__x10, __x18, __c); cond_swap<_Compare>(__x11, __x19, __c); cond_swap<_Compare>(__x12, __x24, __c); cond_swap<_Compare>(__x14, __x34, __c); cond_swap<_Compare>(__x15, __x35, __c); cond_swap<_Compare>(__x17, __x33, __c); cond_swap<_Compare>(__x20, __x36, __c); cond_swap<_Compare>(__x22, __x26, __c); cond_swap<_Compare>(__x23, __x27, __c); cond_swap<_Compare>(__x25, __x37, __c); cond_swap<_Compare>(__x28, __x40, __c); cond_swap<_Compare>(__x29, __x45, __c); cond_swap<_Compare>(__x30, __x50, __c); cond_swap<_Compare>(__x31, __x51, __c); cond_swap<_Compare>(__x32, __x48, __c); cond_swap<_Compare>(__x38, __x42, __c); cond_swap<_Compare>(__x39, __x43, __c); cond_swap<_Compare>(__x41, __x53, __c); cond_swap<_Compare>(__x44, __x56, __c); cond_swap<_Compare>(__x46, __x54, __c); cond_swap<_Compare>(__x47, __x55, __c); cond_swap<_Compare>(__x49, __x57, __c); cond_swap<_Compare>(__x52, __x60, __c); cond_swap<_Compare>(__x58, __x62, __c); cond_swap<_Compare>(__x59, __x63, __c);
    // Layer 8: [(4,8),(5,13),(6,14),(7,11),(9,21),(10,22),(12,20),(15,23),(16,44),(17,33),(18,34),(19,47),(24,32),(25,37),(26,38),(27,35),(28,36),(29,45),(30,46),(31,39),(40,48),(41,53),(42,54),(43,51),(49,57),(50,58),(52,56),(55,59)]
    cond_swap<_Compare>(__x5, __x9, __c); cond_swap<_Compare>(__x6, __x14, __c); cond_swap<_Compare>(__x7, __x15, __c); cond_swap<_Compare>(__x8, __x12, __c); cond_swap<_Compare>(__x10, __x22, __c); cond_swap<_Compare>(__x11, __x23, __c); cond_swap<_Compare>(__x13, __x21, __c); cond_swap<_Compare>(__x16, __x24, __c); cond_swap<_Compare>(__x17, __x45, __c); cond_swap<_Compare>(__x18, __x34, __c); cond_swap<_Compare>(__x19, __x35, __c); cond_swap<_Compare>(__x20, __x48, __c); cond_swap<_Compare>(__x25, __x33, __c); cond_swap<_Compare>(__x26, __x38, __c); cond_swap<_Compare>(__x27, __x39, __c); cond_swap<_Compare>(__x28, __x36, __c); cond_swap<_Compare>(__x29, __x37, __c); cond_swap<_Compare>(__x30, __x46, __c); cond_swap<_Compare>(__x31, __x47, __c); cond_swap<_Compare>(__x32, __x40, __c); cond_swap<_Compare>(__x41, __x49, __c); cond_swap<_Compare>(__x42, __x54, __c); cond_swap<_Compare>(__x43, __x55, __c); cond_swap<_Compare>(__x44, __x52, __c); cond_swap<_Compare>(__x50, __x58, __c); cond_swap<_Compare>(__x51, __x59, __c); cond_swap<_Compare>(__x53, __x57, __c); cond_swap<_Compare>(__x56, __x60, __c);
    // Layer 9: [(5,9),(6,10),(8,12),(11,15),(13,21),(14,22),(16,20),(17,45),(18,46),(19,23),(24,28),(25,33),(26,34),(27,31),(29,37),(30,38),(32,36),(35,39),(40,44),(41,49),(42,50),(43,47),(48,52),(51,55),(53,57),(54,58)]
    cond_swap<_Compare>(__x6, __x10, __c); cond_swap<_Compare>(__x7, __x11, __c); cond_swap<_Compare>(__x9, __x13, __c); cond_swap<_Compare>(__x12, __x16, __c); cond_swap<_Compare>(__x14, __x22, __c); cond_swap<_Compare>(__x15, __x23, __c); cond_swap<_Compare>(__x17, __x21, __c); cond_swap<_Compare>(__x18, __x46, __c); cond_swap<_Compare>(__x19, __x47, __c); cond_swap<_Compare>(__x20, __x24, __c); cond_swap<_Compare>(__x25, __x29, __c); cond_swap<_Compare>(__x26, __x34, __c); cond_swap<_Compare>(__x27, __x35, __c); cond_swap<_Compare>(__x28, __x32, __c); cond_swap<_Compare>(__x30, __x38, __c); cond_swap<_Compare>(__x31, __x39, __c); cond_swap<_Compare>(__x33, __x37, __c); cond_swap<_Compare>(__x36, __x40, __c); cond_swap<_Compare>(__x41, __x45, __c); cond_swap<_Compare>(__x42, __x50, __c); cond_swap<_Compare>(__x43, __x51, __c); cond_swap<_Compare>(__x44, __x48, __c); cond_swap<_Compare>(__x49, __x53, __c); cond_swap<_Compare>(__x52, __x56, __c); cond_swap<_Compare>(__x54, __x58, __c); cond_swap<_Compare>(__x55, __x59, __c);
    // Layer 10: [(9,13),(10,14),(16,24),(17,21),(18,22),(19,27),(20,28),(23,31),(25,29),(26,30),(32,40),(33,37),(34,38),(35,43),(36,44),(39,47),(41,45),(42,46),(49,53),(50,54)]
    cond_swap<_Compare>(__x10, __x14, __c); cond_swap<_Compare>(__x11, __x15, __c); cond_swap<_Compare>(__x17, __x25, __c); cond_swap<_Compare>(__x18, __x22, __c); cond_swap<_Compare>(__x19, __x23, __c); cond_swap<_Compare>(__x20, __x28, __c); cond_swap<_Compare>(__x21, __x29, __c); cond_swap<_Compare>(__x24, __x32, __c); cond_swap<_Compare>(__x26, __x30, __c); cond_swap<_Compare>(__x27, __x31, __c); cond_swap<_Compare>(__x33, __x41, __c); cond_swap<_Compare>(__x34, __x38, __c); cond_swap<_Compare>(__x35, __x39, __c); cond_swap<_Compare>(__x36, __x44, __c); cond_swap<_Compare>(__x37, __x45, __c); cond_swap<_Compare>(__x40, __x48, __c); cond_swap<_Compare>(__x42, __x46, __c); cond_swap<_Compare>(__x43, __x47, __c); cond_swap<_Compare>(__x50, __x54, __c); cond_swap<_Compare>(__x51, __x55, __c);
    // Layer 11: [(12,16),(15,19),(17,25),(18,26),(20,24),(21,29),(22,30),(23,27),(28,32),(31,35),(33,41),(34,42),(36,40),(37,45),(38,46),(39,43),(44,48),(47,51)]
    cond_swap<_Compare>(__x13, __x17, __c); cond_swap<_Compare>(__x16, __x20, __c); cond_swap<_Compare>(__x18, __x26, __c); cond_swap<_Compare>(__x19, __x27, __c); cond_swap<_Compare>(__x21, __x25, __c); cond_swap<_Compare>(__x22, __x30, __c); cond_swap<_Compare>(__x23, __x31, __c); cond_swap<_Compare>(__x24, __x28, __c); cond_swap<_Compare>(__x29, __x33, __c); cond_swap<_Compare>(__x32, __x36, __c); cond_swap<_Compare>(__x34, __x42, __c); cond_swap<_Compare>(__x35, __x43, __c); cond_swap<_Compare>(__x37, __x41, __c); cond_swap<_Compare>(__x38, __x46, __c); cond_swap<_Compare>(__x39, __x47, __c); cond_swap<_Compare>(__x40, __x44, __c); cond_swap<_Compare>(__x45, __x49, __c); cond_swap<_Compare>(__x48, __x52, __c);
    // Layer 12: [(1,16),(2,32),(5,20),(6,36),(9,24),(10,40),(13,17),(14,18),(21,25),(22,26),(23,53),(27,57),(29,33),(30,34),(31,61),(37,41),(38,42),(39,54),(43,58),(45,49),(46,50),(47,62)]
    cond_swap<_Compare>(__x2, __x17, __c); cond_swap<_Compare>(__x3, __x33, __c); cond_swap<_Compare>(__x6, __x21, __c); cond_swap<_Compare>(__x7, __x37, __c); cond_swap<_Compare>(__x10, __x25, __c); cond_swap<_Compare>(__x11, __x41, __c); cond_swap<_Compare>(__x14, __x18, __c); cond_swap<_Compare>(__x15, __x19, __c); cond_swap<_Compare>(__x22, __x26, __c); cond_swap<_Compare>(__x23, __x27, __c); cond_swap<_Compare>(__x24, __x54, __c); cond_swap<_Compare>(__x28, __x58, __c); cond_swap<_Compare>(__x30, __x34, __c); cond_swap<_Compare>(__x31, __x35, __c); cond_swap<_Compare>(__x32, __x62, __c); cond_swap<_Compare>(__x38, __x42, __c); cond_swap<_Compare>(__x39, __x43, __c); cond_swap<_Compare>(__x40, __x55, __c); cond_swap<_Compare>(__x44, __x59, __c); cond_swap<_Compare>(__x46, __x50, __c); cond_swap<_Compare>(__x47, __x51, __c); cond_swap<_Compare>(__x48, __x63, __c);
    // Layer 13: [(1,4),(2,8),(3,33),(6,12),(7,37),(10,24),(11,41),(13,28),(14,44),(15,45),(18,48),(19,49),(21,36),(22,52),(26,56),(27,42),(30,60),(35,50),(39,53),(51,57),(55,61),(59,62)]
    cond_swap<_Compare>(__x2, __x5, __c); cond_swap<_Compare>(__x3, __x9, __c); cond_swap<_Compare>(__x4, __x34, __c); cond_swap<_Compare>(__x7, __x13, __c); cond_swap<_Compare>(__x8, __x38, __c); cond_swap<_Compare>(__x11, __x25, __c); cond_swap<_Compare>(__x12, __x42, __c); cond_swap<_Compare>(__x14, __x29, __c); cond_swap<_Compare>(__x15, __x45, __c); cond_swap<_Compare>(__x16, __x46, __c); cond_swap<_Compare>(__x19, __x49, __c); cond_swap<_Compare>(__x20, __x50, __c); cond_swap<_Compare>(__x22, __x37, __c); cond_swap<_Compare>(__x23, __x53, __c); cond_swap<_Compare>(__x27, __x57, __c); cond_swap<_Compare>(__x28, __x43, __c); cond_swap<_Compare>(__x31, __x61, __c); cond_swap<_Compare>(__x36, __x51, __c); cond_swap<_Compare>(__x40, __x54, __c); cond_swap<_Compare>(__x52, __x58, __c); cond_swap<_Compare>(__x56, __x62, __c); cond_swap<_Compare>(__x60, __x63, __c);
    // Layer 14: [(2,4),(3,17),(5,6),(7,22),(8,16),(11,25),(12,20),(14,28),(15,29),(18,32),(19,33),(23,37),(26,40),(30,44),(31,45),(34,48),(35,49),(38,52),(41,56),(43,51),(46,60),(47,55),(57,58),(59,61)]
    cond_swap<_Compare>(__x3, __x5, __c); cond_swap<_Compare>(__x4, __x18, __c); cond_swap<_Compare>(__x6, __x7, __c); cond_swap<_Compare>(__x8, __x23, __c); cond_swap<_Compare>(__x9, __x17, __c); cond_swap<_Compare>(__x12, __x26, __c); cond_swap<_Compare>(__x13, __x21, __c); cond_swap<_Compare>(__x15, __x29, __c); cond_swap<_Compare>(__x16, __x30, __c); cond_swap<_Compare>(__x19, __x33, __c); cond_swap<_Compare>(__x20, __x34, __c); cond_swap<_Compare>(__x24, __x38, __c); cond_swap<_Compare>(__x27, __x41, __c); cond_swap<_Compare>(__x31, __x45, __c); cond_swap<_Compare>(__x32, __x46, __c); cond_swap<_Compare>(__x35, __x49, __c); cond_swap<_Compare>(__x36, __x50, __c); cond_swap<_Compare>(__x39, __x53, __c); cond_swap<_Compare>(__x42, __x57, __c); cond_swap<_Compare>(__x44, __x52, __c); cond_swap<_Compare>(__x47, __x61, __c); cond_swap<_Compare>(__x48, __x56, __c); cond_swap<_Compare>(__x58, __x59, __c); cond_swap<_Compare>(__x60, __x62, __c);
    // Layer 15: [(3,18),(7,21),(11,32),(15,30),(17,26),(19,25),(22,36),(23,29),(27,41),(31,52),(33,48),(34,40),(37,46),(38,44),(42,56),(45,60)]
    cond_swap<_Compare>(__x4, __x19, __c); cond_swap<_Compare>(__x8, __x22, __c); cond_swap<_Compare>(__x12, __x33, __c); cond_swap<_Compare>(__x16, __x31, __c); cond_swap<_Compare>(__x18, __x27, __c); cond_swap<_Compare>(__x20, __x26, __c); cond_swap<_Compare>(__x23, __x37, __c); cond_swap<_Compare>(__x24, __x30, __c); cond_swap<_Compare>(__x28, __x42, __c); cond_swap<_Compare>(__x32, __x53, __c); cond_swap<_Compare>(__x34, __x49, __c); cond_swap<_Compare>(__x35, __x41, __c); cond_swap<_Compare>(__x38, __x47, __c); cond_swap<_Compare>(__x39, __x45, __c); cond_swap<_Compare>(__x43, __x57, __c); cond_swap<_Compare>(__x46, __x61, __c);
    // Layer 16: [(3,16),(7,20),(11,24),(15,21),(17,18),(19,34),(22,28),(23,38),(25,40),(26,32),(27,33),(29,44),(30,36),(31,37),(35,41),(39,52),(42,48),(43,56),(45,46),(47,60)]
    cond_swap<_Compare>(__x4, __x17, __c); cond_swap<_Compare>(__x8, __x21, __c); cond_swap<_Compare>(__x12, __x25, __c); cond_swap<_Compare>(__x16, __x22, __c); cond_swap<_Compare>(__x18, __x19, __c); cond_swap<_Compare>(__x20, __x35, __c); cond_swap<_Compare>(__x23, __x29, __c); cond_swap<_Compare>(__x24, __x39, __c); cond_swap<_Compare>(__x26, __x41, __c); cond_swap<_Compare>(__x27, __x33, __c); cond_swap<_Compare>(__x28, __x34, __c); cond_swap<_Compare>(__x30, __x45, __c); cond_swap<_Compare>(__x31, __x37, __c); cond_swap<_Compare>(__x32, __x38, __c); cond_swap<_Compare>(__x36, __x42, __c); cond_swap<_Compare>(__x40, __x53, __c); cond_swap<_Compare>(__x43, __x49, __c); cond_swap<_Compare>(__x44, __x57, __c); cond_swap<_Compare>(__x46, __x47, __c); cond_swap<_Compare>(__x48, __x61, __c);
    // Layer 17: [(3,9),(7,13),(10,16),(11,17),(14,20),(15,22),(18,24),(19,26),(21,28),(23,30),(25,32),(27,34),(29,36),(31,38),(33,40),(35,42),(37,44),(39,45),(41,48),(43,49),(46,52),(47,53),(50,56),(54,60)]
    cond_swap<_Compare>(__x4, __x10, __c); cond_swap<_Compare>(__x8, __x14, __c); cond_swap<_Compare>(__x11, __x17, __c); cond_swap<_Compare>(__x12, __x18, __c); cond_swap<_Compare>(__x15, __x21, __c); cond_swap<_Compare>(__x16, __x23, __c); cond_swap<_Compare>(__x19, __x25, __c); cond_swap<_Compare>(__x20, __x27, __c); cond_swap<_Compare>(__x22, __x29, __c); cond_swap<_Compare>(__x24, __x31, __c); cond_swap<_Compare>(__x26, __x33, __c); cond_swap<_Compare>(__x28, __x35, __c); cond_swap<_Compare>(__x30, __x37, __c); cond_swap<_Compare>(__x32, __x39, __c); cond_swap<_Compare>(__x34, __x41, __c); cond_swap<_Compare>(__x36, __x43, __c); cond_swap<_Compare>(__x38, __x45, __c); cond_swap<_Compare>(__x40, __x46, __c); cond_swap<_Compare>(__x42, __x49, __c); cond_swap<_Compare>(__x44, __x50, __c); cond_swap<_Compare>(__x47, __x53, __c); cond_swap<_Compare>(__x48, __x54, __c); cond_swap<_Compare>(__x51, __x57, __c); cond_swap<_Compare>(__x55, __x61, __c);
    // Layer 18: [(3,8),(7,10),(9,12),(11,16),(13,14),(15,17),(18,20),(19,22),(21,24),(23,26),(25,28),(27,29),(30,32),(31,33),(34,36),(35,38),(37,40),(39,42),(41,44),(43,45),(46,48),(47,52),(49,50),(51,54),(53,56),(55,60)]
    cond_swap<_Compare>(__x4, __x9, __c); cond_swap<_Compare>(__x8, __x11, __c); cond_swap<_Compare>(__x10, __x13, __c); cond_swap<_Compare>(__x12, __x17, __c); cond_swap<_Compare>(__x14, __x15, __c); cond_swap<_Compare>(__x16, __x18, __c); cond_swap<_Compare>(__x19, __x21, __c); cond_swap<_Compare>(__x20, __x23, __c); cond_swap<_Compare>(__x22, __x25, __c); cond_swap<_Compare>(__x24, __x27, __c); cond_swap<_Compare>(__x26, __x29, __c); cond_swap<_Compare>(__x28, __x30, __c); cond_swap<_Compare>(__x31, __x33, __c); cond_swap<_Compare>(__x32, __x34, __c); cond_swap<_Compare>(__x35, __x37, __c); cond_swap<_Compare>(__x36, __x39, __c); cond_swap<_Compare>(__x38, __x41, __c); cond_swap<_Compare>(__x40, __x43, __c); cond_swap<_Compare>(__x42, __x45, __c); cond_swap<_Compare>(__x44, __x46, __c); cond_swap<_Compare>(__x47, __x49, __c); cond_swap<_Compare>(__x48, __x53, __c); cond_swap<_Compare>(__x50, __x51, __c); cond_swap<_Compare>(__x52, __x55, __c); cond_swap<_Compare>(__x54, __x57, __c); cond_swap<_Compare>(__x56, __x61, __c);
    // Layer 19: [(3,5),(6,8),(7,9),(10,12),(11,13),(14,16),(15,18),(17,20),(19,21),(22,24),(23,25),(26,28),(27,30),(29,32),(31,34),(33,36),(35,37),(38,40),(39,41),(42,44),(43,46),(45,48),(47,49),(50,52),(51,53),(54,56),(55,57),(58,60)]
    cond_swap<_Compare>(__x4, __x6, __c); cond_swap<_Compare>(__x7, __x9, __c); cond_swap<_Compare>(__x8, __x10, __c); cond_swap<_Compare>(__x11, __x13, __c); cond_swap<_Compare>(__x12, __x14, __c); cond_swap<_Compare>(__x15, __x17, __c); cond_swap<_Compare>(__x16, __x19, __c); cond_swap<_Compare>(__x18, __x21, __c); cond_swap<_Compare>(__x20, __x22, __c); cond_swap<_Compare>(__x23, __x25, __c); cond_swap<_Compare>(__x24, __x26, __c); cond_swap<_Compare>(__x27, __x29, __c); cond_swap<_Compare>(__x28, __x31, __c); cond_swap<_Compare>(__x30, __x33, __c); cond_swap<_Compare>(__x32, __x35, __c); cond_swap<_Compare>(__x34, __x37, __c); cond_swap<_Compare>(__x36, __x38, __c); cond_swap<_Compare>(__x39, __x41, __c); cond_swap<_Compare>(__x40, __x42, __c); cond_swap<_Compare>(__x43, __x45, __c); cond_swap<_Compare>(__x44, __x47, __c); cond_swap<_Compare>(__x46, __x49, __c); cond_swap<_Compare>(__x48, __x50, __c); cond_swap<_Compare>(__x51, __x53, __c); cond_swap<_Compare>(__x52, __x54, __c); cond_swap<_Compare>(__x55, __x57, __c); cond_swap<_Compare>(__x56, __x58, __c); cond_swap<_Compare>(__x59, __x61, __c);
    // Layer 20: [(3,4),(5,6),(7,8),(9,10),(11,12),(13,14),(15,16),(17,18),(19,20),(21,22),(23,24),(25,26),(27,28),(29,30),(31,32),(33,34),(35,36),(37,38),(39,40),(41,42),(43,44),(45,46),(47,48),(49,50),(51,52),(53,54),(55,56),(57,58),(59,60)]
    cond_swap<_Compare>(__x4, __x5, __c); cond_swap<_Compare>(__x6, __x7, __c); cond_swap<_Compare>(__x8, __x9, __c); cond_swap<_Compare>(__x10, __x11, __c); cond_swap<_Compare>(__x12, __x13, __c); cond_swap<_Compare>(__x14, __x15, __c); cond_swap<_Compare>(__x16, __x17, __c); cond_swap<_Compare>(__x18, __x19, __c); cond_swap<_Compare>(__x20, __x21, __c); cond_swap<_Compare>(__x22, __x23, __c); cond_swap<_Compare>(__x24, __x25, __c); cond_swap<_Compare>(__x26, __x27, __c); cond_swap<_Compare>(__x28, __x29, __c); cond_swap<_Compare>(__x30, __x31, __c); cond_swap<_Compare>(__x32, __x33, __c); cond_swap<_Compare>(__x34, __x35, __c); cond_swap<_Compare>(__x36, __x37, __c); cond_swap<_Compare>(__x38, __x39, __c); cond_swap<_Compare>(__x40, __x41, __c); cond_swap<_Compare>(__x42, __x43, __c); cond_swap<_Compare>(__x44, __x45, __c); cond_swap<_Compare>(__x46, __x47, __c); cond_swap<_Compare>(__x48, __x49, __c); cond_swap<_Compare>(__x50, __x51, __c); cond_swap<_Compare>(__x52, __x53, __c); cond_swap<_Compare>(__x54, __x55, __c); cond_swap<_Compare>(__x56, __x57, __c); cond_swap<_Compare>(__x58, __x59, __c); cond_swap<_Compare>(__x60, __x61, __c);
}

