#include <iostream>

// macros per trasformare un token in string literal
#define STR_HELPER(x) #x
#define STR(x)        STR_HELPER(x)

#ifdef _LIBCPP_VERSION
  #pragma message("USING libc++ version " STR(_LIBCPP_VERSION))
#elif defined(__GLIBCXX__)
  #pragma message("USING libstdc++ version " STR(__GLIBCXX__))
#elif defined(_MSVC_STL_VERSION)
  #pragma message("USING MSVC STL version " STR(_MSVC_STL_VERSION))
#else
  #pragma message("STL UNKNOWN")
#endif

int main() {
    std::cout << "OK\n";
    return 0;
}
