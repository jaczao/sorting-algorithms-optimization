# si usa sempre clang per questo progetto
# usare solo su Macos
set(CMAKE_SYSTEM_NAME    Darwin)
set(CMAKE_C_COMPILER     "/usr/bin/clang")
set(CMAKE_CXX_COMPILER   "/usr/bin/clang++")
set(CMAKE_CXX_FLAGS      "${CMAKE_CXX_FLAGS} -stdlib=libc++")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")
