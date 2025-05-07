// ARM64 inline assembly translation of SortAlphaDev and VarSortAlphaDev functions
// Register mapping: w1=eax, w2=ecx, w3=edx, w4=r8d, w5=r9d, w6=r10d, w7=r11d, w8=r12d, w9=r13d, w10=r14d, w11=r15d

#include <algorithm>
#include <functional>
#include <utility>
#include <vector>

void Sort3AlphaDev(int* buffer) {
    asm volatile(
        "ldr w1, [%0, #4]             \n"
        "ldr w2, [%0, #8]             \n"
        "cmp w2, w1                   \n"
        "mov w3, w1                   \n"
        "csel w3, w2, w3, lt          \n"
        "ldr w4, [%0]                 \n"
        "csel w1, w2, w1, gt          \n"
        "cmp w1, w4                   \n"
        "mov w2, w4                   \n"
        "csel w2, w1, w2, lt          \n"
        "csel w1, w4, w1, le          \n"
        "str w1, [%0, #8]             \n"
        "cmp w3, w2                   \n"
        "csel w4, w3, w4, le          \n"
        "str w4, [%0]                 \n"
        "csel w2, w3, w2, gt          \n"
        "str w2, [%0, #4]             \n"
        : "+r"(buffer)
        :
        : "w1", "w2", "w3", "w4", "memory");
  }
  
  void Sort4AlphaDev(int* buffer) {
    asm volatile(
        /* mov 0x8(%0), %%eax      */ "ldr   w1, [%0, #8]            \n"
        /* mov (%0), %%ecx         */ "ldr   w2, [%0]                \n"
        /* mov 0x4(%0), %%edx      */ "ldr   w3, [%0, #4]            \n"
        /* cmp %%eax, %%ecx        */ "cmp   w1, w2                  \n"
        /* mov %%eax, %%r8d        */ "mov   w4, w1                  \n"
        /* cmovl %%ecx, %%r8d      */ "csel  w4, w4, w2, lt          \n"  // w4 = min(w1,w2)
        /* cmovl %%eax, %%ecx      */ "csel  w2, w2, w1, lt          \n"  // w2 = max(w1,w2)

        /* mov 0xc(%0), %%r9d      */ "ldr   w5, [%0, #12]           \n"
        /* cmp %%r9d, %%edx        */ "cmp   w5, w3                  \n"
        /* mov %%r9d, %%eax        */ "mov   w1, w5                  \n"
        /* cmovl %%edx, %%eax      */ "csel  w1, w1, w3, lt          \n"  // w1 = min(w5,w3)
        /* cmovl %%r9d, %%edx      */ "csel  w3, w3, w5, lt          \n"  // w3 = max(w5,w3)

        /* cmp %%eax, %%r8d        */ "cmp   w1, w4                  \n"
        /* mov %%eax, %%r9d        */ "mov   w5, w1                  \n"
        /* cmovl %%r8d, %%r9d      */ "csel  w5, w5, w4, lt          \n"  // w5 = min(w1,w4)
        /* cmovge %%r8d, %%eax     */ "csel  w1, w1, w4, ge          \n"  // w1 = max(w1,w4)

        /* mov %%r9d, (%0)         */ "str   w5, [%0]                \n"

        /* cmp %%edx, %%ecx        */ "cmp   w3, w2                  \n"
        /* mov %%edx, %%r8d        */ "mov   w4, w3                  \n"
        /* cmovl %%ecx, %%r8d      */ "csel  w4, w4, w2, lt          \n"  // w4 = min(w3,w2)
        /* cmovge %%ecx, %%edx     */ "csel  w3, w3, w2, ge          \n"  // w3 = max(w3,w2)
        /* mov %%edx, 0xc(%0)      */ "str   w3, [%0, #12]           \n"

        /* cmp %%r8d, %%eax        */ "cmp   w4, w1                  \n"
        /* mov %%r8d, %%ecx        */ "mov   w2, w4                  \n"
        /* cmovl %%eax, %%ecx      */ "csel  w2, w2, w1, lt          \n"  // w2 = min(w4,w1)
        /* cmovge %%eax, %%r8d     */ "csel  w4, w4, w1, ge          \n"  // w4 = max(w4,w1)
        /* mov %%r8d, 0x8(%0)      */ "str   w4, [%0, #8]            \n"
        /* mov %%ecx, 0x4(%0)      */ "str   w2, [%0, #4]            \n"
        : "+r"(buffer)
        :
        : "w1","w2","w3","w4","w5","cc","memory"
    );
}



void Sort5AlphaDev(int* buffer) {
  asm volatile(
      /* mov (%0), %%eax               */ "ldr   w1, [%0]                  \n"
      /* mov 0x4(%0), %%ecx            */ "ldr   w2, [%0, #4]              \n"
      /* cmp %%eax, %%ecx              */ "cmp   w1, w2                    \n"
      /* mov %%eax, %%edx              */ "mov   w3, w1                    \n"
      /* cmovl %%ecx, %%edx            */ "csel  w3, w3, w2, lt            \n"  // w3 = min(w1,w2)
      /* cmovg %%ecx, %%eax            */ "csel  w1, w1, w2, gt            \n"  // w1 = max(w1,w2)

      /* mov 0xc(%0), %%r8d            */ "ldr   w4, [%0, #12]             \n"
      /* mov 0x10(%0), %%ecx           */ "ldr   w2, [%0, #16]             \n"
      /* cmp %%r8d, %%ecx              */ "cmp   w4, w2                    \n"
      /* mov %%r8d, %%r9d              */ "mov   w5, w4                    \n"
      /* cmovl %%ecx, %%r9d            */ "csel  w5, w5, w2, lt            \n"  // w5 = min(w4,w2)
      /* cmovg %%ecx, %%r8d            */ "csel  w4, w4, w2, gt            \n"  // w4 = max(w4,w2)

      /* mov 0x8(%0), %%r10d           */ "ldr   w6, [%0, #8]              \n"
      /* cmp %%r10d, %%r8d             */ "cmp   w6, w4                    \n"
      /* mov %%r10d, %%ecx             */ "mov   w2, w6                    \n"
      /* cmovl %%r8d, %%ecx            */ "csel  w2, w2, w4, lt            \n"  // w2 = min(w6,w4)
      /* cmovle %%r10d, %%r8d          */ "csel  w4, w4, w6, le            \n"  // w4 = max(w6,w4)

      /* cmp %%ecx, %%r9d              */ "cmp   w2, w5                    \n"
      /* cmovle %%r9d, %%r10d          */ "csel  w6, w6, w5, le            \n"  // w6 = min(old w6, w5)
      /* cmovg %%r9d, %%ecx            */ "csel  w2, w2, w5, gt            \n"  // w2 = max(w2, w5)

      /* cmp %%eax, %%r8d              */ "cmp   w1, w4                    \n"
      /* mov %%eax, %%r9d              */ "mov   w5, w1                    \n"
      /* cmovl %%r8d, %%r9d            */ "csel  w5, w5, w4, lt            \n"  // w5 = min(w1,w4)
      /* cmovle %%eax, %%r8d           */ "csel  w4, w4, w1, le            \n"  // w4 = max(w1,w4)

      /* cmp %%edx, %%ecx              */ "cmp   w3, w2                    \n"
      /* mov %%edx, %%eax              */ "mov   w1, w3                    \n"
      /* cmovl %%ecx, %%eax            */ "csel  w1, w1, w2, lt            \n"  // w1 = min(w3,w2)
      /* cmovle %%edx, %%ecx           */ "csel  w2, w2, w3, le            \n"  // w2 = max(w3,w2)

      /* mov %%r8d, 0x10(%0)           */ "str   w4, [%0, #16]             \n"

      /* cmp %%eax, %%r10d             */ "cmp   w1, w6                    \n"
      /* cmovle %%r10d, %%edx          */ "csel  w3, w3, w6, le            \n"  // w3 = min(old w3, w6)
      /* mov %%edx, (%0)               */ "str   w3, [%0]                  \n"
      /* cmovg %%r10d, %%eax           */ "csel  w1, w1, w6, gt            \n"  // w1 = max(w1,w6)

      /* cmp %%r9d, %%ecx              */ "cmp   w5, w2                    \n"
      /* mov %%r9d, %%r8d              */ "mov   w4, w5                    \n"
      /* cmovl %%ecx, %%r8d            */ "csel  w4, w4, w2, lt            \n"  // w4 = min(w5,w2)
      /* cmovle %%r9d, %%ecx           */ "csel  w2, w2, w5, le            \n"  // w2 = max(w5,w2)
      /* mov %%ecx, 0xc(%0)            */ "str   w2, [%0, #12]             \n"

      /* cmp %%r8d, %%eax              */ "cmp   w4, w1                    \n"
      /* cmovle %%eax, %%r9d           */ "csel  w5, w5, w1, le            \n"  // w5 = min(old w5, w1)
      /* mov %%r9d, 0x4(%0)            */ "str   w5, [%0, #4]              \n"
      /* cmovg %%eax, %%r8d            */ "csel  w4, w4, w1, gt            \n"  // w4 = max(w4,w1)
      /* mov %%r8d, 0x8(%0)            */ "str   w4, [%0, #8]              \n"
      : "+r"(buffer)
      :
      : "w1","w2","w3","w4","w5","w6","cc","memory"
  );
}

void Sort6AlphaDev(int* buffer) {
  asm volatile(
      "; -- Initial loads from memory to ARM w registers --\n"
      "; x0 (implicit, from %%0) holds the 'buffer' pointer\n"
      // x86: mov 0x14(%%0), %%r9d           -> buffer[5] to r9d
      "ldr w1, [%0, #20]             \n"  // w1 = buffer[5] (mapping for r9d)
      // x86: mov 0xc(%%0), %%r10d           -> buffer[3] to r10d
      "ldr w2, [%0, #12]             \n"  // w2 = buffer[3] (mapping for r10d)
      // x86: mov 0x8(%%0), %%ecx            -> buffer[2] to ecx
      "ldr w3, [%0, #8]              \n"  // w3 = buffer[2] (mapping for ecx)
      // x86: mov 0x10(%%0), %%eax           -> buffer[4] to eax
      "ldr w4, [%0, #16]             \n"  // w4 = buffer[4] (mapping for eax)
      // x86: mov %%ecx, %%edx              -> ecx to edx
      "mov w5, w3                    \n"  // w5 = w3         (mapping for edx = ecx)
      // x86: mov (%%0), %%ebx               -> buffer[0] to ebx
      "ldr w6, [%0, #0]              \n"  // w6 = buffer[0] (mapping for ebx)
      // x86: mov 0x14(%%0), %%r8d           -> buffer[5] to r8d (already in w1 from first load)
      "mov w7, w1                    \n"  // w7 = w1         (mapping for r8d = buffer[5])
      // x86: mov (%%0), %%r11d              -> buffer[0] to r11d (already in w6 from load)
      "mov w8, w6                    \n"  // w8 = w6         (mapping for r11d = buffer[0])

      "; -- Start of sorting network comparisons and conditional moves --\n"
      // x86: cmp %%eax, %%r8d              -> cmp r8d, eax
      "cmp w7, w4                    \n"  // cmp w7(r8d), w4(eax)
      // x86: cmovg %%eax, %%r9d            -> if r8d > eax, r9d = eax
      "csel w1, w4, w1, gt           \n"  // if (w7 > w4) w1(r9d) = w4(eax) else w1=w1
      // x86: cmovl %%eax, %%r8d            -> if r8d < eax, r8d = eax
      "csel w7, w4, w7, lt           \n"  // if (w7 < w4) w7(r8d) = w4(eax) else w7=w7

      // x86: mov 0x4(%%0), %%eax            -> buffer[1] to eax
      "ldr w4, [%0, #4]              \n"  // w4 = buffer[1] (new eax val)
      // x86: cmp %%eax, %%edx              -> cmp edx, eax
      "cmp w5, w4                    \n"  // cmp w5(edx), w4(eax)
      // x86: cmovg %%eax, %%edx            -> if edx > eax, edx = eax
      "csel w5, w4, w5, gt           \n"  // if (w5 > w4) w5(edx) = w4(eax) else w5=w5
      // x86: cmovle %%eax, %%ecx           -> if edx <= eax, ecx = eax
      "csel w3, w4, w3, le           \n"  // if (w5 <= w4) w3(ecx) = w4(eax) else w3=w3

      // x86: cmp %%r11d, %%ecx             -> cmp ecx, r11d
      "cmp w3, w8                    \n"  // cmp w3(ecx), w8(r11d)
      // x86: cmovle %%ecx, %%r11d          -> if ecx <= r11d, r11d = ecx
      "csel w8, w3, w8, le           \n"  // if (w3 <= w8) w8(r11d) = w3(ecx) else w8=w8
      // x86: mov 0xc(%%0), %%eax            -> buffer[3] to eax (original r10d value, currently in w2)
      "mov w4, w2                    \n"  // w4(eax) = w2 (buffer[3])
      // x86: cmovle %%ebx, %%ecx           -> (uses prev cmp w3,w8) if ecx <= r11d, ecx = ebx
      "csel w3, w6, w3, le           \n"  // if (w3 <= w8) w3(ecx) = w6(ebx) else w3=w3

      // x86: cmp %%r10d, %%r8d             -> cmp r8d, r10d
      "cmp w7, w2                    \n"  // cmp w7(r8d), w2(r10d)
      // x86: cmovle %%r8d, %%r10d          -> if r8d <= r10d, r10d = r8d
      "csel w2, w7, w2, le           \n"  // if (w7 <= w2) w2(r10d) = w7(r8d) else w2=w2
      // x86: cmovl %%eax, %%r8d            -> (uses prev cmp w7,w2) if r8d < r10d, r8d = eax (eax is buffer[3])
      "csel w7, w4, w7, lt           \n"  // if (w7 < w2) w7(r8d) = w4(eax) else w7=w7

      // x86: cmp %%eax, %%r9d              -> cmp r9d, eax
      "cmp w1, w4                    \n"  // cmp w1(r9d), w4(eax)
      // x86: cmovge %%r9d, %%r10d          -> if r9d >= eax, r10d = r9d
      "csel w2, w1, w2, ge           \n"  // if (w1 >= w4) w2(r10d) = w1(r9d) else w2=w2
      // x86: cmovl %%r9d, %%eax            -> if r9d < eax, eax = r9d
      "csel w4, w1, w4, lt           \n"  // if (w1 < w4) w4(eax) = w1(r9d) else w4=w4

      // x86: cmp %%ebx, %%edx              -> cmp edx, ebx
      "cmp w5, w6                    \n"  // cmp w5(edx), w6(ebx)
      // x86: cmovle %%edx, %%ebx           -> if edx <= ebx, ebx = edx
      "csel w6, w5, w6, le           \n"  // if (w5 <= w6) w6(ebx) = w5(edx) else w6=w6
      // x86: cmovge %%edx, %%r11d          -> if edx >= ebx, r11d = edx
      "csel w8, w5, w8, ge           \n"  // if (w5 >= w6) w8(r11d) = w5(edx) else w8=w8

      // x86: cmp %%ecx, %%r8d              -> cmp r8d, ecx
      "cmp w7, w3                    \n"  // cmp w7(r8d), w3(ecx)
      // x86: mov %%ecx, %%edx              -> edx = ecx
      "mov w5, w3                    \n"  // w5(edx) = w3(ecx)
      // x86: cmovl %%r8d, %%edx            -> (uses prev cmp w7,w3) if r8d < ecx, edx = r8d
      "csel w5, w7, w5, lt           \n"  // if (w7 < w3) w5(edx) = w7(r8d) else w5=w5
      // x86: cmovle %%ecx, %%r8d           -> if r8d <= ecx, r8d = ecx
      "csel w7, w3, w7, le           \n"  // if (w7 <= w3) w7(r8d) = w3(ecx) else w7=w7

      // x86: mov %%ebx, %%ecx              -> ecx = ebx
      "mov w3, w6                    \n"  // w3(ecx) = w6(ebx)
      // x86: cmp %%ecx, %%eax              -> cmp eax, ecx
      "cmp w4, w3                    \n"  // cmp w4(eax), w3(ecx)
      // x86: cmovge %%eax, %%ebx           -> if eax >= ecx, ebx = eax
      "csel w6, w4, w6, ge           \n"  // if (w4 >= w3) w6(ebx) = w4(eax) else w6=w6
      // x86: cmovle %%eax, %%ecx           -> if eax <= ecx, ecx = eax
      "csel w3, w4, w3, le           \n"  // if (w4 <= w3) w3(ecx) = w4(eax) else w3=w3

      // x86: mov %%ebx, %%eax              -> eax = ebx
      "mov w4, w6                    \n"  // w4(eax) = w6(ebx)
      // x86: cmp %%r11d, %%r10d            -> cmp r10d, r11d
      "cmp w2, w8                    \n"  // cmp w2(r10d), w8(r11d)

      "; -- Stores back to memory --\n"
      // x86: mov %%r8d, 0x14(%%0)           -> r8d to buffer[5]
      "str w7, [%0, #20]             \n"  // buffer[5] = w7(r8d)
      // x86: mov %%r11d, %%r8d             -> r8d = r11d
      "mov w7, w8                    \n"  // w7(r8d) = w8(r11d)
      // x86: cmovle %%r10d, %%r8d          -> (uses prev cmp w2,w8) if r10d <= r11d, r8d = r10d
      "csel w7, w2, w7, le           \n"  // if (w2 <= w8) w7(r8d) = w2(r10d) else w7=w7
      // x86: cmovle %%r11d, %%r10d         -> if r10d <= r11d, r10d = r11d
      "csel w2, w8, w2, le           \n"  // if (w2 <= w8) w2(r10d) = w8(r11d) else w2=w2

      // x86: cmp %%r8d, %%ebx              -> cmp ebx, r8d
      "cmp w6, w7                    \n"  // cmp w6(ebx), w7(r8d)
      // x86: cmovl %%r8d, %%eax            -> if ebx < r8d, eax = r8d
      "csel w4, w7, w4, lt           \n"  // if (w6 < w7) w4(eax) = w7(r8d) else w4=w4
      // x86: mov %%ecx, (%%0)               -> ecx to buffer[0]
      "str w3, [%0, #0]              \n"  // buffer[0] = w3(ecx)
      // x86: cmovg %%r8d, %%ebx            -> (uses prev cmp w6,w7) if ebx > r8d, ebx = r8d
      "csel w6, w7, w6, gt           \n"  // if (w6 > w7) w6(ebx) = w7(r8d) else w6=w6

      // x86: cmp %%edx, %%eax              -> cmp eax, edx
      "cmp w4, w5                    \n"  // cmp w4(eax), w5(edx)
      // x86: mov %%ebx, 0x4(%%0)            -> ebx to buffer[1]
      "str w6, [%0, #4]              \n"  // buffer[1] = w6(ebx)
      // x86: mov %%edx, %%ecx              -> ecx = edx
      "mov w3, w5                    \n"  // w3(ecx) = w5(edx)
      // x86: cmovg %%eax, %%ecx            -> (uses prev cmp w4,w5) if eax > edx, ecx = eax
      "csel w3, w4, w3, gt           \n"  // if (w4 > w5) w3(ecx) = w4(eax) else w3=w3
      // x86: cmovge %%edx, %%eax           -> if eax >= edx, eax = edx
      "csel w4, w5, w4, ge           \n"  // if (w4 >= w5) w4(eax) = w5(edx) else w4=w4

      // x86: cmp %%edx, %%r10d             -> cmp r10d, edx
      "cmp w2, w5                    \n"  // cmp w2(r10d), w5(edx)
      // x86: cmovl %%r10d, %%ecx           -> if r10d < edx, ecx = r10d
      "csel w3, w2, w3, lt           \n"  // if (w2 < w5) w3(ecx) = w2(r10d) else w3=w3
      // x86: cmovl %%edx, %%r10d           -> if r10d < edx, r10d = edx
      "csel w2, w5, w2, lt           \n"  // if (w2 < w5) w2(r10d) = w5(edx) else w2=w2

      "; -- Final stores --\n"
      // x86: mov %%ecx, 0xc(%%0)            -> ecx to buffer[3]
      "str w3, [%0, #12]             \n"  // buffer[3] = w3(ecx)
      // x86: mov %%r10d, 0x10(%%0)          -> r10d to buffer[4]
      "str w2, [%0, #16]             \n"  // buffer[4] = w2(r10d)
      // x86: mov %%eax, 0x8(%%0)            -> eax to buffer[2]
      "str w4, [%0, #8]              \n"  // buffer[2] = w4(eax)
      : // No explicit output registers
      : [buffer_ptr] "r"(buffer) // Input: buffer pointer. Using %0 for [%0, ...]
      : "w1", "w2", "w3", "w4", "w5", "w6", "w7", "w8",
        "memory", "cc"
  );
}






  
void Sort7AlphaDev(int* buffer) {
  asm volatile(
      /* mov    (%0), %%r12d        */ "ldr   w9,  [x0]                \n"
      /* mov    0xc(%0), %%r9d      */ "ldr   w6,  [x0, #12]           \n"
      /* mov    0x10(%0), %%r8d     */ "ldr   w5,  [x0, #16]           \n"
      /* mov    0x14(%0), %%r10d    */ "ldr   w7,  [x0, #20]           \n"
      /* mov    0x18(%0), %%r11d    */ "ldr   w8,  [x0, #24]           \n"
      /* mov    0x8(%0), %%ecx      */ "ldr   w2,  [x0, #8]            \n"
      /* mov    %%r12d, %%ebx       */ "mov   w4,  w9                  \n"
      /* mov    0x4(%0), %%edx      */ "ldr   w3,  [x0, #4]            \n"
      /* mov    %%r9d, %%eax        */ "mov   w1,  w6                  \n"

      /* cmp    %%eax, %%r8d        */ "cmp   w1,  w5                  \n"
      /* cmovl  %%r8d, %%r9d        */ "csel  w6,  w6, w5, lt          \n" // w6 = min(old w6, w5)
      /* cmovl  %%eax, %%r8d        */ "csel  w5,  w5, w1, lt          \n" // w5 = min(w5, w1)

      /* mov    0x4(%0), %%eax      */ "ldr   w1,  [x0, #4]            \n"
      /* cmp    %%edx, %%ecx        */ "cmp   w3,  w2                  \n"
      /* cmovl  %%ecx, %%edx        */ "csel  w3,  w3, w2, lt          \n" // w3 = min(w3, w2)
      /* cmovle %%eax, %%ecx        */ "csel  w2,  w2, w1, le          \n" // w2 = min(w2, w1)

      /* cmp    %%r12d, %%ecx       */ "cmp   w9,  w2                  \n"
      /* cmovle %%ecx, %%r12d       */ "csel  w9,  w9, w2, le          \n" // w9 = min(w9, w2)

      /* mov    0x14(%0), %%eax     */ "ldr   w1,  [x0, #20]           \n"
      /* cmovle %%ebx, %%ecx        */ "csel  w2,  w2, w4, le          \n" // w2 = min(w2, w4)

      /* mov    %%ecx, %%r13d       */ "mov   w10, w2                  \n"

      /* cmp    %%eax, %%r11d       */ "cmp   w1,  w8                  \n"
      /* cmovl  %%r11d, %%eax       */ "csel  w1,  w1, w8, lt          \n" // w1 = min(w1, w8)

      /* mov    %%r9d, %%ebx        */ "mov   w4,  w6                  \n" 
      /* cmovle %%r10d, %%r11d      */ "csel  w8,  w8, w7, le          \n" // w8 = min(w8, w7)

      /* cmp    %%ebx, %%eax        */ "cmp   w4,  w1                  \n"
      /* cmovl  %%eax, %%ebx        */ "csel  w4,  w4, w1, lt          \n" // w4 = min(w4, w1)
      /* cmovle %%r9d, %%eax        */ "csel  w1,  w1, w6, le          \n" // w1 = min(w1, w6)

      /* cmp    %%r8d, %%r11d       */ "cmp   w5,  w8                  \n"
      /* mov    %%r8d, %%r10d       */ "mov   w7,  w5                  \n"
      /* cmovl  %%r11d, %%r10d      */ "csel  w7,  w7, w8, lt          \n" // w7 = min(w7, w8)
      /* cmovle %%r8d, %%r11d       */ "csel  w8,  w8, w5, le          \n" // w8 = min(w8, w5)

      /* cmp    %%r13d, %%r11d      */ "cmp   w10, w8                  \n"
      /* mov    %%r12d, %%r8d       */ "mov   w5,  w9                  \n"
      /* cmovle %%r11d, %%r13d      */ "csel  w10, w10, w8, le         \n" // w10 = min(w10, w8)
      /* cmovle %%ecx, %%r11d       */ "csel  w8,  w8, w2, le          \n" // w8 = min(w8, w2)

      /* mov    %%edx, %%r9d        */ "mov   w6,  w3                  \n"
      /* mov    %%r11d, 0x18(%0)    */ "str   w8,  [x0, #24]           \n"
      /* mov    %%r13d, %%ecx       */ "mov   w2,  w10                 \n"

      /* cmp    %%r9d, %%eax        */ "cmp   w6,  w1                  \n"
      /* cmovle %%eax, %%r9d        */ "csel  w6,  w6, w1, le          \n" // w6 = min(w6, w1)
      /* cmovl  %%edx, %%eax        */ "csel  w1,  w1, w3, lt          \n" // w1 = min(w1, w3)

      /* cmp    %%r8d, %%r10d       */ "cmp   w5,  w7                  \n"
      /* cmovl  %%r10d, %%r8d       */ "csel  w5,  w5, w7, lt          \n" // w5 = min(w5, w7)
      /* cmovl  %%r12d, %%r10d      */ "csel  w7,  w7, w9, lt          \n" // w7 = min(w7, w9)

      /* cmp    %%ecx, %%eax        */ "cmp   w2,  w1                  \n"
      /* cmovle %%eax, %%ecx        */ "csel  w2,  w2, w1, le          \n" // w2 = min(w2, w1)
      /* cmovle %%r13d, %%eax       */ "csel  w1,  w1, w10, le         \n" // w1 = min(w1, w10)

      /* cmp    %%r12d, %%ebx       */ "cmp   w9,  w4                  \n"
      /* mov    %%ecx, %%edx        */ "mov   w3,  w2                  \n"
      /* cmovg  %%ebx, %%r8d        */ "csel  w5,  w5, w4, gt          \n" // w5 = max(w5, w4)
      /* cmovle %%ebx, %%r12d       */ "csel  w9,  w9, w4, le          \n" // w9 = min(w9, w4)

      /* mov    %%r9d, %%r14d       */ "mov   w11, w6                  \n"
      /* cmp    %%r12d, %%r9d       */ "cmp   w9,  w6                  \n"
      /* cmovl  %%r12d, %%r14d      */ "csel  w11, w11, w9, lt         \n" // w11 = min(w11, w9)
      /* cmovl  %%r9d, %%r12d       */ "csel  w9,  w9, w6, lt          \n" // w9 = min(w9, w6)

      /* cmp    %%ecx, %%r10d       */ "cmp   w2,  w7                  \n"
      /* cmovl  %%r10d, %%edx      */ "csel  w3,  w3, w7, lt          \n" // w3 = min(w3, w7)
      /* cmovge %%r10d, %%ecx      */ "csel  w2,  w2, w7, ge          \n" // w2 = max(w2, w7)
      /* mov    %%edx, %%r11d       */ "mov   w8,  w3                  \n"

      /* cmp    %%r9d, %%r8d        */ "cmp   w6,  w5                  \n"
      /* cmovl  %%r8d, %%r14d      */ "csel  w11, w11, w5, lt         \n" // w11 = min(w11, w5)
      /* mov    %%r14d, 0x4(%0)    */ "str   w11, [x0, #4]            \n"
      /* mov    %%r12d, (%0)       */ "str   w9,  [x0]                \n"

      /* cmovl  %%r9d, %%r8d       */ "csel  w5,  w5, w6, lt          \n" // w5 = min(w5, w6)
      /* cmp    %%edx, %%r8d       */ "cmp   w3,  w5                  \n"
      /* cmovl  %%r8d, %%r11d      */ "csel  w8,  w8, w5, lt          \n" // w8 = min(w8, w5)
      /* cmovge %%r8d, %%edx      */ "csel  w3,  w3, w5, ge          \n" // w3 = max(w3, w5)
      /* mov    %%r11d, 0x8(%0)    */ "str   w8,  [x0, #8]            \n"

      /* cmp    %%ecx, %%eax       */ "cmp   w2,  w1                  \n"
      /* mov    %%edx, 0xc(%0)     */ "str   w3,  [x0, #12]           \n"
      /* mov    %%ecx, %%r11d      */ "mov   w8,  w2                  \n"
      /* cmovge %%eax, %%ecx      */ "csel  w2,  w2, w1, ge          \n" // w2 = max(w2, w1)
      /* mov    %%ecx, 0x14(%0)    */ "str   w2,  [x0, #20]           \n"
      /* cmovle %%eax, %%r11d     */ "csel  w8,  w8, w1, le          \n" // w8 = min(w8, w1)
      /* mov    %%r11d, 0x10(%0)   */ "str   w8,  [x0, #16]           \n"
      : "+r"(buffer)
      :
      : "w1","w2","w3","w4","w5","w6","w7","w8","w9","w10","w11","cc","memory"
  );
}

  
void Sort8AlphaDev(int* buffer) {
  // x86 Register Mappings to ARM w registers:
  // buffer -> %0 (passed as x0)
  // r8d  -> w1, ecx  -> w2, ebx  -> w3, r11d -> w4
  // r10d -> w5, r12d -> w6, r9d  -> w7, edx  -> w8
  // eax  -> w9, r13d -> w10,r14d -> w11
  asm volatile(
      "; -- Initial loads from memory --\n"
      // x86: mov 0x8(%0), %%r8d            ; buffer[2] -> r8d
      "ldr w1, [%0, #8]              \n"
      // x86: mov 0x4(%0), %%ecx            ; buffer[1] -> ecx
      "ldr w2, [%0, #4]              \n"
      // x86: mov 0x18(%0), %%ebx           ; buffer[6] -> ebx
      "ldr w3, [%0, #24]             \n"
      // x86: mov 0x10(%0), %%r11d          ; buffer[4] -> r11d
      "ldr w4, [%0, #16]             \n"
      // x86: mov 0x14(%0), %%r10d          ; buffer[5] -> r10d
      "ldr w5, [%0, #20]             \n"
      // x86: mov 0x18(%0), %%r12d          ; buffer[6] -> r12d (same as ebx initial)
      "ldr w6, [%0, #24]             \n"
      // x86: mov 0xc(%0), %%r9d            ; buffer[3] -> r9d
      "ldr w7, [%0, #12]             \n"
      // x86: mov (%0), %%edx               ; buffer[0] -> edx
      "ldr w8, [%0, #0]              \n"

      "; -- Stage 1 --\n"
      // x86: mov %%edx, %%eax              ; edx -> eax
      "mov w9, w8                    \n"  // w9(eax) = w8(edx)
      // x86: cmp %%eax, %%ecx              ; cmp ecx, eax
      "cmp w2, w9                    \n"  // cmp w2(ecx), w9(eax)
      // x86: cmovl %%ecx, %%edx            ; if ecx < eax, edx = ecx
      "csel w8, w2, w8, lt           \n"  // w8(edx) = (w2 < w9) ? w2 : w8
      // x86: cmovle %%eax, %%ecx           ; if ecx <= eax, ecx = eax
      "csel w2, w9, w2, le           \n"  // w2(ecx) = (w2 <= w9) ? w9 : w2

      // x86: mov %%r8d, %%eax              ; r8d -> eax
      "mov w9, w1                    \n"  // w9(eax) = w1(r8d)
      // x86: cmp %%r8d, %%r9d              ; cmp r9d, r8d
      "cmp w7, w1                    \n"  // cmp w7(r9d), w1(r8d)
      // x86: mov %%edx, %%r13d             ; edx -> r13d
      "mov w10, w8                   \n"  // w10(r13d) = w8(edx)
      // x86: cmovle %%r9d, %%eax           ; if r9d <= r8d, eax = r9d
      "csel w9, w7, w9, le           \n"  // w9(eax) = (w7 <= w1) ? w7 : w9
      // x86: cmovl %%r8d, %%r9d            ; if r9d < r8d, r9d = r8d
      "csel w7, w1, w7, lt           \n"  // w7(r9d) = (w7 < w1) ? w1 : w7

      // x86: cmp %%r11d, %%r10d            ; cmp r10d, r11d
      "cmp w5, w4                    \n"  // cmp w5(r10d), w4(r11d)
      // x86: mov 0x10(%0), %%r8d           ; buffer[4] -> r8d (original r11d value)
      "ldr w1, [%0, #16]             \n"  // w1(r8d) = buffer[4] (r8d overwritten)
      // x86: cmovl %%r10d, %%r11d          ; if r10d < r11d, r11d = r10d
      "csel w4, w5, w4, lt           \n"  // w4(r11d) = (w5 < w4) ? w5 : w4
      // x86: cmovl %%r8d, %%r10d           ; if r10d < r11d, r10d = r8d (buffer[4])
      "csel w5, w1, w5, lt           \n"  // w5(r10d) = (w5 < original w4) ? w1 : w5

      // x86: mov 0x1c(%0), %%r8d           ; buffer[7] -> r8d
      "ldr w1, [%0, #28]             \n"  // w1(r8d) = buffer[7] (r8d overwritten again)
      // x86: cmp %%r13d, %%eax             ; cmp eax, r13d
      "cmp w9, w10                   \n"  // cmp w9(eax), w10(r13d)
      // x86: cmovle %%eax, %%r13d          ; if eax <= r13d, r13d = eax
      "csel w10, w9, w10, le         \n"  // w10(r13d) = (w9 <= w10) ? w9 : w10
      // x86: mov %%r13d, %%r14d            ; r13d -> r14d
      "mov w11, w10                  \n"  // w11(r14d) = w10(r13d)
      // x86: cmovl %%edx, %%eax            ; if eax < r13d (original values), eax = edx (current value of edx in w8)
      "csel w9, w8, w9, lt           \n"  // w9(eax) = (w9_orig < w10_orig) ? w8 : w9

      // x86: cmp %%ebx, %%r8d              ; cmp r8d, ebx (r8d is buffer[7], ebx is buffer[6])
      "cmp w1, w3                    \n"  // cmp w1(r8d), w3(ebx)
      // x86: cmovle %%r8d, %%r12d          ; if r8d <= ebx, r12d = r8d
      "csel w6, w1, w6, le           \n"  // w6(r12d) = (w1 <= w3) ? w1 : w6
      // x86: mov %%r11d, %%edx             ; r11d -> edx
      "mov w8, w4                    \n"  // w8(edx) = w4(r11d) (edx overwritten)
      // x86: cmovle %%ebx, %%r8d           ; if r8d <= ebx, r8d = ebx
      "csel w1, w3, w1, le           \n"  // w1(r8d) = (w1 <= w3) ? w3 : w1

      "; -- Stage 2 --\n"
      // x86: cmp %%edx, %%r12d             ; cmp r12d, edx
      "cmp w6, w8                    \n"  // cmp w6(r12d), w8(edx)
      // x86: mov %%ecx, %%ebx              ; ecx -> ebx
      "mov w3, w2                    \n"  // w3(ebx) = w2(ecx) (ebx overwritten)
      // x86: cmovl %%r12d, %%edx           ; if r12d < edx, edx = r12d
      "csel w8, w6, w8, lt           \n"  // w8(edx) = (w6 < w8) ? w6 : w8
      // x86: cmovl %%r11d, %%r12d          ; if r12d < edx (original values), r12d = r11d (current value of r11d in w4)
      "csel w6, w4, w6, lt           \n"  // w6(r12d) = (w6_orig < w8_orig) ? w4 : w6

      // x86: cmp %%ecx, %%r9d              ; cmp r9d, ecx
      "cmp w7, w2                    \n"  // cmp w7(r9d), w2(ecx)
      // x86: cmovl %%r9d, %%ebx            ; if r9d < ecx, ebx = r9d
      "csel w3, w7, w3, lt           \n"  // w3(ebx) = (w7 < w2) ? w7 : w3
      // x86: cmovle %%ecx, %%r9d           ; if r9d <= ecx, r9d = ecx
      "csel w7, w2, w7, le           \n"  // w7(r9d) = (w7 <= w2) ? w2 : w7

      // x86: cmp %%ebx, %%eax              ; cmp eax, ebx
      "cmp w9, w3                    \n"  // cmp w9(eax), w3(ebx)
      // x86: mov %%ebx, %%r11d             ; ebx -> r11d
      "mov w4, w3                    \n"  // w4(r11d) = w3(ebx) (r11d overwritten)
      // x86: cmovge %%eax, %%ebx           ; if eax >= ebx, ebx = eax
      "csel w3, w9, w3, ge           \n"  // w3(ebx) = (w9 >= w3) ? w9 : w3
      // x86: cmovl %%eax, %%r11d           ; if eax < ebx (original values), r11d = eax
      "csel w4, w9, w4, lt           \n"  // w4(r11d) = (w9_orig < w3_orig) ? w9 : w4

      // x86: mov %%r11d, %%eax             ; r11d -> eax
      "mov w9, w4                    \n"  // w9(eax) = w4(r11d)
      // x86: cmp %%r13d, %%edx             ; cmp edx, r13d
      "cmp w8, w10                   \n"  // cmp w8(edx), w10(r13d)
      // x86: mov %%r10d, %%ecx             ; r10d -> ecx
      "mov w2, w5                    \n"  // w2(ecx) = w5(r10d) (ecx overwritten)
      // x86: cmovl %%edx, %%r14d           ; if edx < r13d, r14d = edx
      "csel w11, w8, w11, lt         \n"  // w11(r14d) = (w8 < w10) ? w8 : w11
      // x86: cmovl %%r13d, %%edx           ; if edx < r13d, edx = r13d
      "csel w8, w10, w8, lt          \n"  // w8(edx) = (w8 < w10) ? w10 : w8

      // x86: cmp %%r10d, %%r8d             ; cmp r8d, r10d (r8d is buffer[7] from last load)
      "cmp w1, w5                    \n"  // cmp w1(r8d), w5(r10d)
      // x86: cmovle %%r8d, %%ecx           ; if r8d <= r10d, ecx = r8d
      "csel w2, w1, w2, le           \n"  // w2(ecx) = (w1 <= w5) ? w1 : w2
      // x86: cmovle %%r10d, %%r8d          ; if r8d <= r10d, r8d = r10d
      "csel w1, w5, w1, le           \n"  // w1(r8d) = (w1 <= w5) ? w5 : w1
      // x86: cmovl %%ecx, %%r10d           ; if r8d < r10d (original values), r10d = ecx
      "csel w5, w2, w5, lt           \n"  // w5(r10d) = (w1_orig < w5_orig) ? w2 : w5

      "; -- Stage 3 & Stores --\n"
      // x86: cmp %%ecx, %%r12d             ; cmp r12d, ecx
      "cmp w6, w2                    \n"  // cmp w6(r12d), w2(ecx)
      // x86: cmovl %%r12d, %%r10d          ; if r12d < ecx, r10d = r12d
      "csel w5, w6, w5, lt           \n"  // w5(r10d) = (w6 < w2) ? w6 : w5
      // x86: mov %%r14d, (%0)              ; r14d -> buffer[0]
      "str w11, [%0, #0]             \n"  // buffer[0] = w11(r14d)
      // x86: cmovge %%r12d, %%ecx          ; if r12d >= ecx, ecx = r12d
      "csel w2, w6, w2, ge           \n"  // w2(ecx) = (w6 >= w2) ? w6 : w2

      // x86: cmp %%r11d, %%r10d            ; cmp r10d, r11d
      "cmp w5, w4                    \n"  // cmp w5(r10d), w4(r11d)
      // x86: cmovl %%r10d, %%eax           ; if r10d < r11d, eax = r10d
      "csel w9, w5, w9, lt           \n"  // w9(eax) = (w5 < w4) ? w5 : w9
      // x86: cmovle %%r11d, %%r10d         ; if r10d <= r11d, r10d = r11d
      "csel w5, w4, w5, le           \n"  // w5(r10d) = (w5 <= w4) ? w4 : w5

      // x86: mov %%r9d, %%r12d             ; r9d -> r12d
      "mov w6, w7                    \n"  // w6(r12d) = w7(r9d)
      // x86: cmp %%r9d, %%r8d              ; cmp r8d, r9d
      "cmp w1, w7                    \n"  // cmp w1(r8d), w7(r9d)
      // x86: cmovl %%r8d, %%r12d           ; if r8d < r9d, r12d = r8d
      "csel w6, w1, w6, lt           \n"  // w6(r12d) = (w1 < w7) ? w1 : w6
      // x86: cmovl %%r9d, %%r8d            ; if r8d < r9d, r8d = r9d
      "csel w1, w7, w1, lt           \n"  // w1(r8d) = (w1 < w7) ? w7 : w1

      // x86: mov %%ebx, %%r11d             ; ebx -> r11d
      "mov w4, w3                    \n"  // w4(r11d) = w3(ebx)
      // x86: mov %%r8d, 0x1c(%0)           ; r8d -> buffer[7]
      "str w1, [%0, #28]             \n"  // buffer[7] = w1(r8d)
      // x86: cmp %%r11d, %%ecx             ; cmp ecx, r11d
      "cmp w2, w4                    \n"  // cmp w2(ecx), w4(r11d)
      // x86: cmovl %%ecx, %%r11d           ; if ecx < r11d, r11d = ecx
      "csel w4, w2, w4, lt           \n"  // w4(r11d) = (w2 < w4) ? w2 : w4
      // x86: mov %%eax, %%r8d              ; eax -> r8d
      "mov w1, w9                    \n"  // w1(r8d) = w9(eax)
      // x86: cmovl %%ebx, %%ecx            ; if ecx < r11d (original values), ecx = ebx (current val of ebx in w3)
      "csel w2, w3, w2, lt           \n"  // w2(ecx) = (w2_orig < w4_orig) ? w3 : w2

      // x86: cmp %%r8d, %%edx              ; cmp edx, r8d
      "cmp w8, w1                    \n"  // cmp w8(edx), w1(r8d)
      // x86: cmovg %%edx, %%eax            ; if edx > r8d, eax = edx
      "csel w9, w8, w9, gt           \n"  // w9(eax) = (w8 > w1) ? w8 : w9
      // x86: mov %%r11d, %%ebx             ; r11d -> ebx
      "mov w3, w4                    \n"  // w3(ebx) = w4(r11d)
      // x86: cmovle %%edx, %%r8d           ; if edx <= r8d, r8d = edx
      "csel w1, w8, w1, le           \n"  // w1(r8d) = (w8 <= w1) ? w8 : w1

      // x86: cmp %%r12d, %%ecx             ; cmp ecx, r12d
      "cmp w2, w6                    \n"  // cmp w2(ecx), w6(r12d)
      // x86: mov %%r8d, 0x4(%0)            ; r8d -> buffer[1]
      "str w1, [%0, #4]              \n"  // buffer[1] = w1(r8d)
      // x86: mov %%r12d, %%r8d             ; r12d -> r8d
      "mov w1, w6                    \n"  // w1(r8d) = w6(r12d)
      // x86: cmovl %%ecx, %%r8d            ; if ecx < r12d, r8d = ecx
      "csel w1, w2, w1, lt           \n"  // w1(r8d) = (w2 < w6) ? w2 : w1
      // x86: cmovle %%r12d, %%ecx          ; if ecx <= r12d, ecx = r12d
      "csel w2, w6, w2, le           \n"  // w2(ecx) = (w2 <= w6) ? w6 : w2

      // x86: cmp %%r11d, %%eax             ; cmp eax, r11d
      "cmp w9, w4                    \n"  // cmp w9(eax), w4(r11d)
      // x86: cmovle %%eax, %%ebx           ; if eax <= r11d, ebx = eax
      "csel w3, w9, w3, le           \n"  // w3(ebx) = (w9 <= w4) ? w9 : w3
      // x86: cmovle %%r11d, %%eax          ; if eax <= r11d, eax = r11d
      "csel w9, w4, w9, le           \n"  // w9(eax) = (w9 <= w4) ? w4 : w9

      // x86: cmp %%r12d, %%r10d            ; cmp r10d, r12d
      "cmp w5, w6                    \n"  // cmp w5(r10d), w6(r12d)
      // x86: mov %%ebx, 0x8(%0)            ; ebx -> buffer[2]
      "str w3, [%0, #8]              \n"  // buffer[2] = w3(ebx)
      // x86: cmovge %%r10d, %%r8d          ; if r10d >= r12d, r8d = r10d
      "csel w1, w5, w1, ge           \n"  // w1(r8d) = (w5 >= w6) ? w5 : w1
      // x86: cmovle %%r10d, %%r12d         ; if r10d <= r12d, r12d = r10d
      "csel w6, w5, w6, le           \n"  // w6(r12d) = (w5 <= w6) ? w5 : w6

      // x86: mov %%r12d, %%ebx             ; r12d -> ebx
      "mov w3, w6                    \n"  // w3(ebx) = w6(r12d)
      // x86: cmp %%r12d, %%eax             ; cmp eax, r12d
      "cmp w9, w6                    \n"  // cmp w9(eax), w6(r12d)
      // x86: mov %%ecx, 0x18(%0)           ; ecx -> buffer[6]
      "str w2, [%0, #24]             \n"  // buffer[6] = w2(ecx)
      // x86: cmovg %%eax, %%ebx            ; if eax > r12d, ebx = eax
      "csel w3, w9, w3, gt           \n"  // w3(ebx) = (w9 > w6) ? w9 : w3
      // x86: mov %%ebx, 0x10(%0)           ; ebx -> buffer[4]
      "str w3, [%0, #16]             \n"  // buffer[4] = w3(ebx)
      // x86: mov %%r8d, 0x14(%0)           ; r8d -> buffer[5]
      "str w1, [%0, #20]             \n"  // buffer[5] = w1(r8d)
      // x86: cmovg %%r12d, %%eax           ; if eax > r12d (original values), eax = r12d
      "csel w9, w6, w9, gt           \n"  // w9(eax) = (w9_orig > w6_orig) ? w6 : w9
      // x86: mov %%eax, 0xc(%0)            ; eax -> buffer[3]
      "str w9, [%0, #12]             \n"  // buffer[3] = w9(eax)

      : "+r"(buffer) // Input/Output: buffer pointer
      : // No other inputs
      : "w1", "w2", "w3", "w4", "w5", "w6", "w7", "w8", "w9", "w10", "w11", // Clobbered ARM W registers
        "memory", "cc" // Clobber memory and condition codes
  );
}
  
  void VarSort3AlphaDev(int* buffer) {
    asm volatile(
        "ldr w1, [%0, #4]             \n"
        "mov w2, #1                   \n"
        "ldr w3, [%0]                 \n"
        "cmp w2, w3                   \n"
        "mov w4, w1                   \n"
        "mov w5, w1                   \n"
        "beq 0f                        \n"
        "ldr w6, [%0, #8]             \n"
        "cmp w4, w6                   \n"
        "csel w4, w6, w4, le          \n"
        "csel w6, w5, w6, lt          \n"
        "mov w7, #2                   \n"
        "cmp w7, w3                   \n"
        "csel w9, w4, wzr, ge          \n"
        "mov w10, w4                  \n"
        "str w10, [%0, #4]            \n"
        "str w9, [%0, #8]             \n"
        "beq 0f                        \n"
        "ldr w4, [%0, #12]            \n"
        "cmp w4, w6                   \n"
        "csel w9, w4, w9, le          \n"
        "csel w8, w4, w8, gt          \n"
        "str w9, [%0, #12]            \n"
        "cmp w8, w1                   \n"
        "csel w10, w5, w8, ge         \n"
        "csel w10, w4, w10, gt        \n"
        "str w10, [%0, #4]            \n"
        "str w8, [%0, #8]             \n"
        "0:                           \n"
        : "+r"(buffer)
        :
        : "w1","w2","w3","w4","w5","w6","w7","w8","w9","w10","memory");
  }
  
  void VarSort4AlphaDev(int* buffer) {
    asm volatile(
        "mov w2, #1                   \n"
        "ldr w1, [%0, #4]             \n"
        "mov w3, w1                   \n"
        "ldr w4, [%0]                 \n"
        "cmp w2, w4                   \n"
        "beq 0f                        \n"
        "mov w5, w1                   \n"
        "mov w6, w1                   \n"
        "ldr w7, [%0, #8]             \n"
        "cmp w3, w7                   \n"
        "csel w5, w7, w5, ge          \n"
        "csel w6, w7, w6, le          \n"
        "mov w2, #2                   \n"
        "cmp w2, w4                   \n"
        "str w6, [%0, #4]             \n"
        "b 1f                          \n"
        "csel w5, w1, w5, gt          \n"
        "str w5, [%0, #8]             \n"
        "csel w6, w3, w6, gt          \n"
        "str w6, [%0, #8]             \n"
        "1:                           \n"
        "ldr w5, [%0, #8]             \n"
        "mov w6, w5                   \n"
        "cmp w2, w4                   \n"
        "bne 0f                        \n"
        "ldr w7, [%0, #12]            \n"
        "cmp w5, w7                   \n"
        "csel w5, w7, w5, lt          \n"
        "csel w7, w7, w7, gt          \n"
        "cmp w6, w1                   \n"
        "csel w6, w7, w6, le          \n"
        "csel w1, w6, w1, gt          \n"
        "str w6, [%0, #8]             \n"
        "str w1, [%0, #4]             \n"
        "str w7, [%0, #12]            \n"
        "beq 0f                        \n"
        "ldr w7, [%0, #16]            \n"
        "mov w6, w5                   \n"
        "cmp w5, w7                   \n"
        "csel w6, w7, w6, le          \n"
        "csel w7, w7, w7, gt          \n"
        "ldr w8, [%0, #4]             \n"
        "cmp w8, w6                   \n"
        "csel w6, w8, w6, le          \n"
        "csel w8, w6, w8, ge          \n"
        "str w6, [%0, #4]             \n"
        "str w8, [%0, #8]             \n"
        "str w7, [%0, #16]            \n"
        "0:                           \n"
        : "+r"(buffer)
        :
        : "w1","w2","w3","w4","w5","w6","w7","w8","memory");
  }
  
  void VarSort5AlphaDev(int* buffer) {
    asm volatile(
        "ldr w1, [%0, #4]             \n"
        "mov w2, #1                   \n"
        "ldr w3, [%0]                 \n"
        "cmp w2, w3                   \n"
        "csel w4, w1, wzr, gt          \n"
        "beq 0f                        \n"
        "ldr w5, [%0, #8]             \n"
        "cmp w5, w4                   \n"
        "csel w1, w5, w1, gt          \n"
        "csel w4, w5, w4, lt          \n"
        "str w1, [%0, #4]             \n"
        "cmp w2, w3                   \n"
        "str w4, [%0, #8]             \n"
        "beq 0f                        \n"
        "ldr w6, [%0, #12]            \n"
        "mov w7, w4                   \n"
        "cmp w6, w7                   \n"
        "csel w4, w6, w4, gt          \n"
        "csel w7, w6, w7, lt          \n"
        "cmp w1, w6                   \n"
        "csel w1, w1, w4, le          \n"
        "csel w6, w1, w6, le          \n"
        "mov w8, #3                   \n"
        "cmp w8, w3                   \n"
        "mov w9, w7                   \n"
        "str w1, [%0, #4]             \n"
        "str w9, [%0, #12]            \n"
        "str w4, [%0, #8]             \n"
        "beq 0f                        \n"
        "ldr w6, [%0, #16]            \n"
        "cmp w6, w4                   \n"
        "csel w7, w6, w7, lt          \n"
        "csel w4, w6, w4, ge          \n"
        "csel w1, w1, w7, le          \n"
        "cmp w7, w9                   \n"
        "csel w7, w7, w9, gt          \n"
        "str w5, [%0, #8]             \n"
        "str w1, [%0, #4]             \n"
        "str w7, [%0, #16]            \n"
        "b 2f                          \n"
        "2:                           \n"
        "ldr w5, [%0, #12]            \n"
        "mov w10, w5                  \n"
        "cmp w10, wzr                 \n"
        "beq 0f                        \n"
        "ldr w6, [%0, #20]            \n"
        "mov w9, w4                   \n"
        "cmp w4, w6                   \n"
        "csel w9, w6, w9, le          \n"
        "str w9, [%0, #8]             \n"
        "ldr w1, [%0, #4]             \n"
        "csel w4, w6, w4, gt          \n"
        "cmp w4, w9                   \n"
        "csel w9, w4, w9, lt          \n"
        "csel w10, w10, w4, gt        \n"
        "str w10, [%0, #16]           \n"
        "str w9, [%0, #12]            \n"
        "ldr w1, [%0, #4]             \n"
        "mov w2, #1                   \n"
        "cmp w2, w3                   \n"
        "csel w4, w1, wzr, gt          \n"
        "bgt 0f                        \n"
        "ldr w5, [%0, #8]             \n"
        "cmp w1, w7                   \n"
        "csel w1, w7, w1, le          \n"
        "csel w4, w5, w4, gt          \n"
        "str w4, [%0, #8]             \n"
        "str w1, [%0, #4]             \n"
        "0:                           \n"
        : "+r"(buffer)               
        :                           
        : "w1","w2","w3","w4","w5","w6","w7","w8","w9","w10","memory");
  }
