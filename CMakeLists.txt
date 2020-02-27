cmake_minimum_required(VERSION 3.10)
project(PinponProject)

set(CMAKE_CXX_STANDARD 11)
if(WIN32)
        set(CMAKE_CXX_FLAGS "/O2 /EHa /MTd /utf-8")
        set(CMAKE_CXX_FLAGS_DEBUG "/EHa /MTd /utf-8")
else()
        set(CMAKE_CXX_FLAGS "-Wall -Wextra")
        set(CMAKE_CXX_FLAGS_DEBUG "-Wall -Wextra")
endif()

add_subdirectory(./googletest)
add_subdirectory(./fakeit)

include_directories("src/")
include_directories("websocketpp/")
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
target_link_libraries(pinpon dl)

add_library(pinpon_lib ${pinpon_SRC})
target_link_libraries(pinpon_lib dl)

add_library(pinpon_dynamic SHARED ${dynamic_SRC} ${pinpon_SRC})

set(CTEST_OUTPUT_ON_FAILURE ON)
enable_testing()
add_executable(pinpon_test ${pinpon_SRC} ${pinpon_test_SRC})
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        # using GCC
        set_target_properties ( pinpon_test PROPERTIES COMPILE_FLAGS " -O1" )
endif()
target_link_libraries(pinpon_test gtest dl)
add_test(NAME pinpon_test COMMAND pinpon_test)

add_subdirectory(pikatanuki)

