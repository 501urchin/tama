include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
FetchContent_MakeAvailable(googletest)

file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS *.cpp)
add_executable(tests ${TEST_SOURCES})
target_link_libraries(tests PRIVATE tama gtest_main)

include(GoogleTest)
gtest_discover_tests(tests)
