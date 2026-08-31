cmake_minimum_required(VERSION 3.20)
project(LOBEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(lob_engine
    src/main.cpp
    src/OrderBook.cpp
    src/MapOrderBook.cpp
    src/MatchingEngine.cpp
    src/OrderPool.cpp
)
target_include_directories(lob_engine PRIVATE include)

include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_executable(lob_tests
    tests/test_smoke.cpp
    tests/test_orderbook.cpp
    tests/test_matching.cpp
    tests/test_orderpool.cpp
    src/OrderBook.cpp
    src/MatchingEngine.cpp
    src/OrderPool.cpp
)
target_include_directories(lob_tests PRIVATE include)
target_link_libraries(lob_tests PRIVATE GTest::gtest GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(lob_tests)

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
  benchmark
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG v1.8.3
)
FetchContent_MakeAvailable(benchmark)

add_executable(lob_bench
    bench/bench_array.cpp
    bench/bench_map.cpp
    src/OrderBook.cpp
    src/MapOrderBook.cpp
    src/OrderPool.cpp
    src/MatchingEngine.cpp
    src/MapMatchingEngine.cpp
)
target_include_directories(lob_bench PRIVATE include)
target_link_libraries(lob_bench PRIVATE benchmark::benchmark)
