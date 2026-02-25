cmake_minimum_required(VERSION 3.20)
project(tama VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE TAMA_SOURCES CONFIGURE_DEPENDS src/*.cpp)

add_library(tama ${TAMA_SOURCES})
target_include_directories(tama PUBLIC include)
target_compile_options(tama PRIVATE -Wall -Wextra -Wpedantic)

target_compile_options(tama PRIVATE
    $<$<CONFIG:Release>:-O3 -march=native -flto>
    $<$<CONFIG:Debug>:-O0 -march=native -g>
)


option(TAMA_RUN_BUILD "Build program" OFF)
if (TAMA_RUN_BUILD)
    add_executable(tama_run examples/main.cpp)
    target_link_libraries(tama_run PRIVATE tama)
    target_compile_options(tama_run PRIVATE
        $<$<CONFIG:Release>:-O3 -ffast-math -march=native -flto>
        $<$<CONFIG:Debug>:-O0 -march=native -g>
        -Wall
    )
endif()

option(TAMA_BUILD_TESTS "Build tests" OFF)
if (TAMA_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
