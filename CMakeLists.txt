cmake_minimum_required(VERSION 3.16)
project(MySortsProject LANGUAGES CXX)

# Require C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Options
option(BUILD_BENCHMARK "Build benchmarks" ON)
option(BUILD_TESTS "Build tests" ON)

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

# Find Threads for benchmark linking
find_package(Threads REQUIRED)

# Include your project headers globally
include_directories(${PROJECT_SOURCE_DIR}/include)

# Source files for sorting implementations
set(SORT_SOURCES
    src/branchless_fixed_sorts.cpp
    src/branching_fixed_sorts.cpp
    src/alpha_dev_sorts_arm64.cpp
    src/sorts.cpp
)

# Benchmark executable (avoid name conflict)
if(BUILD_BENCHMARK)
  add_executable(benchmark_app
    benchmarks/benchmark_sorts.cpp
    ${SORT_SOURCES}
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
  )
  target_link_libraries(run_tests
    PRIVATE
      gtest_main
  )
  include(GoogleTest)
  gtest_discover_tests(run_tests)
endif()
