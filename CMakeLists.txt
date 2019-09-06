cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-O3 -Wall -Wextra")

add_subdirectory(./googletest)
add_subdirectory(./fakeit)

include_directories("src/")
include_directories("fakeit/single_header/gtest")

file(GLOB pinpon_SRC
        "src/*.h"
        "src/*.cc"
        )
file(GLOB pinpon_test_SRC
        "test/*.h"
        "test/*.cc"
        )
file(GLOB dynamic_SRC
        "dynamic/*.h"
        "dynamic/*.cc"
        )

add_executable(pinpon main.cc ${pinpon_SRC})

add_library(pinpon_lib ${pinpon_SRC})

add_library(pinpon_dynamic SHARED ${dynamic_SRC})
target_link_libraries(pinpon_dynamic pinpon_lib)

set(CTEST_OUTPUT_ON_FAILURE ON)
enable_testing()
add_executable(pinpon_test ${pinpon_SRC} ${pinpon_test_SRC})
target_link_libraries(pinpon_test gtest_main)
add_test(NAME pinpon_test COMMAND pinpon_test)

add_subdirectory(qtpinpon)
