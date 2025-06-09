cmake_minimum_required(VERSION 3.16)
project(MySortsProject LANGUAGES CXX)

# Require C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Options
option(BUILD_BENCHMARK "Build benchmarks" ON)
option(BUILD_TESTS "Build tests" ON)
option(BUILD_STL_CHECK "Build stl check" ON)

# Fetch GoogleTest and GoogleBenchmark via FetchContent
include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.14.0
)
FetchContent_Declare(
  googlebenchmark
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG        v1.8.1
)
FetchContent_MakeAvailable(googletest googlebenchmark)

# --- BEGIN SKIP cxx03_test ---
# Se esiste il target legacy C++03, escludilo dalla build
if (TARGET cxx03_test)
  message(STATUS "Escludo il test legacy cxx03_test (richiede C++03)")
  set_target_properties(cxx03_test PROPERTIES
    EXCLUDE_FROM_ALL TRUE
    EXCLUDE_FROM_DEFAULT_BUILD TRUE
  )
endif()
# --- END SKIP cxx03_test ---

# Find Threads for benchmark linking
find_package(Threads REQUIRED)

# Include your project headers globally
include_directories(${PROJECT_SOURCE_DIR}/include)

# Source files for sorting implementations
set(SORT_SOURCES
    src/branchless_fixed_sorts.cpp
    src/branching_fixed_sorts.cpp
    src/sorts.cpp
)

set(ARCH_SOURCES)

if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|AARCH64" OR CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "arm64|ARM64|aarch64|AARCH64")
    message(STATUS "Building for AArch64, includo assembly ARM64")
    list(APPEND ARCH_SOURCES src/alpha_dev_sorts_arm64.cpp)

elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|X86_64|amd64|AMD64" OR
       CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "x86_64|X86_64|amd64|AMD64")
    message(STATUS "Building per x86_64/AMD64, includo assembly x86")
    list(APPEND ARCH_SOURCES src/alpha_dev_sorts_x86-64.cpp)

else()
    message(WARNING "Architettura non riconosciuta: ${CMAKE_SYSTEM_PROCESSOR} / ${CMAKE_HOST_SYSTEM_PROCESSOR}. Nessun file assembly incluso.")
endif()


# Benchmark executable (avoid name conflict)
if(BUILD_BENCHMARK)
  add_executable(benchmark_app
    benchmarks/benchmark_sorts.cpp
    ${SORT_SOURCES}
    ${ARCH_SOURCES}
  )
  target_link_libraries(benchmark_app
    PRIVATE
      benchmark::benchmark
      Threads::Threads
  )
endif()

# Test executable
if(BUILD_TESTS)
  enable_testing()
  add_executable(run_tests
    tests/test_sorts.cpp
    tests/test_fixed_sorts.cpp
    ${SORT_SOURCES}
    ${ARCH_SOURCES}
  )
  target_link_libraries(run_tests
    PRIVATE
      gtest_main
  )
  include(GoogleTest)
  gtest_discover_tests(run_tests)
endif()

# Test executable
if(BUILD_STL_CHECK)
  add_executable(run_stl_check
    tests/stl_check.cpp
  )
endif()
