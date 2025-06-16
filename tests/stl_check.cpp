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

// Macro per formattare __LIBCPP_VERSION
#ifdef _LIBCPP_VERSION
    #define STRINGIFY(x) #x
    #define TOSTRING(x) STRINGIFY(x)
#endif

int main() {
    std::cout << "Hello\n";
      

      #if defined(__GLIBCXX__)
          std::cout << "Libreria standard: libstdc++ (GNU)\n";
          std::cout << "Versione (valore numerico): " << __GLIBCXX__ << "\n";
          // __GLIBCXX__ è una data codificata come AAAAMMGG
          int version = __GLIBCXX__;
          int year = version / 10000;
          int month = (version / 100) % 100;
          int day = version % 100;
          std::cout << "Versione (data): " << year << "-" 
                    << (month < 10 ? "0" : "") << month << "-"
                    << (day < 10 ? "0" : "") << day << '\n';
      #elif defined(_MSVC_STL_VERSION)
          std::cout << "Libreria standard: msvc stl\n";
      #elif defined(_LIBCPP_VERSION)
      std::cout << "Libreria standard: libc++\n";
      std::cout << "Versione (valore numerico): " << _LIBCPP_VERSION << "\n";
      std::cout << "Versione (formattata): ";
      std::cout << (_LIBCPP_VERSION / 1000) << '.'
                << ((_LIBCPP_VERSION / 10) % 100) << '.'
                << (_LIBCPP_VERSION % 10) << '\n';
      #else
          std::cout << "Libreria standard: sconosciuta o non rilevabile\n";
      #endif
          return 0;
}
