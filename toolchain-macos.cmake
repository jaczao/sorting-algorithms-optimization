# si usa sempre clang per questo progetto
# usare solo su Macos
set(CMAKE_SYSTEM_NAME    Darwin)
set(CMAKE_C_COMPILER     "clang")
set(CMAKE_CXX_COMPILER   "clang++")
set(CMAKE_CXX_FLAGS      "${CMAKE_CXX_FLAGS} -stdlib=libc++") #-L/opt/homebrew/opt/llvm/lib/c++ -L/opt/homebrew/opt/llvm/lib/unwind -lunwind
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++") #same as above
