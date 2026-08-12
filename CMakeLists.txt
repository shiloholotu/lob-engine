cmake_minimum_required(VERSION 3.20)
project(LOBEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# App
add_executable(lob_engine src/main.cpp)
target_include_directories(lob_engine PRIVATE include)

# Google Test via FetchContent (downloads on first configure)
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# Avoid overriding parent compiler/linker flags on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_executable(lob_tests tests/test_smoke.cpp)
target_link_libraries(lob_tests PRIVATE GTest::gtest GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(lob_tests)