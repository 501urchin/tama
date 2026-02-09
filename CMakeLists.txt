cmake_minimum_required(VERSION 3.20)
project(tama VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB TAMA_SOURCES CONFIGURE_DEPENDS
    src/*.cpp
)

add_library(tama ${TAMA_SOURCES})
target_include_directories(tama PUBLIC include)
target_compile_options(tama PRIVATE -Wall -Wextra -Wpedantic)



option(TAMA_RUN_BUILD "Build run program" ON)
if (TAMA_RUN_BUILD)
    add_executable(tama_run examples/main.cpp ${TAMA_SOURCES})
    target_link_libraries(tama_run PRIVATE tama)
    target_compile_options(tama_run PRIVATE -O3 -ffast-math  -Wall)
endif()


option(TAMA_BUILD_TESTS "Build tests" ON)

if (TAMA_BUILD_TESTS)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/heads/main.zip
    )
    FetchContent_MakeAvailable(googletest)

    enable_testing()

    file(GLOB TEST_SOURCES CONFIGURE_DEPENDS
        tests/*.cpp
    )

    add_executable(tests ${TEST_SOURCES})
    target_link_libraries(tests PRIVATE tama gtest_main)
    target_include_directories(tests PRIVATE ${gtest_SOURCE_DIR}/include)

    include(GoogleTest)
    gtest_discover_tests(tests)
endif()


