cmake_minimum_required(VERSION 3.20)
project(tama VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# -----------------------------
# 1. Library
# -----------------------------
add_library(tama src/ema.cpp src/sma.cpp)
target_include_directories(tama PUBLIC include)
target_compile_options(tama PRIVATE -Wall -Wextra -Wpedantic)

# -----------------------------
# 2. GoogleTest via FetchContent
# -----------------------------
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

# -----------------------------
# 3. Test executable
# -----------------------------
add_executable(tests tests/main.cpp)
target_link_libraries(tests PRIVATE tama gtest_main)
target_include_directories(tests PRIVATE ${gtest_SOURCE_DIR}/include)


# Register tests with CTest
include(GoogleTest)
gtest_discover_tests(tests)
