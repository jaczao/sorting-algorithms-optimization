// Riproduzione del codice di libc++ al tempo del commit 194d196 fatto da Deepmind

#include <iterator>
#include <optional>
#include <vector>
#include <utility>
#include <iostream>
using namespace std;

template <typename _Compare, typename _RandomAccessIterator>
inline void cond_swap(_RandomAccessIterator __x, _RandomAccessIterator __y, _Compare __c) {
  using value_type = typename std::iterator_traits<_RandomAccessIterator>::value_type;
  bool __r = __c(*__x, *__y);
  value_type __tmp = __r ? *__x : *__y;
  *__y = __r ? *__y : *__x;
  *__x = __tmp;
}

template <class _Compare, class _RandomAccessIterator>
inline void partially_sorted_swap(_RandomAccessIterator __x, _RandomAccessIterator __y,
                                                           _RandomAccessIterator __z, _Compare __c) {
   using value_type = typename std::iterator_traits<_RandomAccessIterator>::value_type;
   bool __r = __c(*__z, *__x);
   value_type __tmp = __r ? *__z : *__x;
   *__z = __r ? *__x : *__z;
   __r = __c(__tmp, *__y);
   *__x = __r ? *__x : *__y;
   *__y = __r ? *__y : __tmp;
}

template <class _Compare, class _RandomAccessIterator>
 inline void
 sort3_maybe_branchless(_RandomAccessIterator __x1, _RandomAccessIterator __x2, _RandomAccessIterator __x3,
                          _Compare __c) {
   cond_swap<_Compare>(__x2, __x3, __c);
   partially_sorted_swap<_Compare>(__x1, __x2, __x3, __c);
   //sort3<_Compare>(__x1, __x2, __x3, __c);
}

using T = int;
void do_sort(T* a, T* b, T* c) {
    std::less<T> comp;
    sort3_maybe_branchless<std::less<T>&>(a, b, c, comp);
}

/*
template <class _Compare, class _RandomAccessIterator>
void sort_phase_2(_RandomAccessIterator __first, _RandomAccessIterator __last, _Compare __comp)
{
    typedef typename iterator_traits<_RandomAccessIterator>::difference_type difference_type;
    sort3_maybe_branchless<_Compare>(__first, __first + difference_type(1), --__last, __comp);
}


void sort_phase_1(T& a, T& b, T& c) { //
    //vector<T> v = {a, b, c};
    std::less<T> comp;
    //sort_phase_2<std::less<T>&>(v.begin(), v.end(), comp);
    sort_phase_2<std::less<T>&>(&a, &c, comp);
}



OLD VERSION
vector<int> do_sort(T& a, T& b, T& c) {
    vector<T> v = {a,b,c};
    cond_swap(++v.begin(), --v.end(), std::less<T>());
    partially_sorted_swap(v.begin(), ++v.begin(), --v.end(), std::less<T>());
    //sort3(&a, &b, &c, std::less<T>());
    return v;
}
*/

template <class _Compare, class _ForwardIterator>
  unsigned
 sort3(_ForwardIterator __x, _ForwardIterator __y, _ForwardIterator __z, _Compare __c)
 {
     unsigned __r = 0;
     if (!__c(*__y, *__x))          // if x <= y
     {
         if (!__c(*__z, *__y))      // if y <= z
             return __r;            // x <= y && y <= z
                                    // x <= y && y > z
         std::swap(*__y, *__z);          // x <= z && y < z
         __r = 1;
         if (__c(*__y, *__x))       // if x > y
         {
             std::swap(*__x, *__y);      // x < y && y <= z
             __r = 2;
         }
         return __r;                // x <= y && y < z
     }
     if (__c(*__z, *__y))           // x > y, if y > z
     {
         std::swap(*__x, *__z);          // x < y && y < z
         __r = 1;
         return __r;
     }
     std::swap(*__x, *__y);              // x > y && y <= z
     __r = 1;                       // x < y && x <= z
     if (__c(*__z, *__y))           // if y > z
     {
         std::swap(*__y, *__z);          // x <= y && y < z
         __r = 2;
     }
     return __r;
 }                                  // x <= y && y <= z


