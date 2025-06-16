# usalo solo su Windows
set(CMAKE_SYSTEM_NAME Windows)
# modifica il percorso se LLVM è altrove
#set(CMAKE_C_COMPILER   "C:/Program Files/LLVM/bin/clang.exe")
#set(CMAKE_CXX_COMPILER "C:/Program Files/LLVM/bin/clang++.exe")

#set(CMAKE_C_COMPILER   "C:/dev-tools/msys64/mingw64/bin/clang")
#set(CMAKE_CXX_COMPILER   "C:/dev-tools/msys64/mingw64/bin/clang++")

set(CMAKE_CXX_FLAGS        "${CMAKE_CXX_FLAGS} -stdlib=libc++")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")
