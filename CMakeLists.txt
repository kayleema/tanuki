cmake_minimum_required(VERSION 3.10)
project(TanukiProject)

set(CMAKE_CXX_STANDARD 11)
if (WIN32)
    set(CMAKE_CXX_FLAGS "/O2 /EHa /MTd /utf-8")
    set(CMAKE_CXX_FLAGS_DEBUG "/EHa /MTd /utf-8")
else ()
    set(CMAKE_CXX_FLAGS "-Wall -Wextra")
    set(CMAKE_CXX_FLAGS_DEBUG "-Wall -Wextra -Og")
endif ()

add_subdirectory(./googletest EXCLUDE_FROM_ALL)
add_subdirectory(./fakeit EXCLUDE_FROM_ALL)

include_directories("src/")
include_directories("websocketpp/")
include_directories("fakeit/single_header/gtest")
include_directories("/usr/local/include")
include_directories("/usr/include")

file(GLOB tanuki_SRC
        "src/*.h"
        "src/*.cc"
        "src/TextInput/*.h"
        "src/TextInput/*.cc"
        "src/Lexer/*.h"
        "src/Lexer/*.cc"
        "src/Lexer/Matchers/*.h"
        "src/Lexer/Matchers/*.cc"
        "src/Parser/*.h"
        "src/Parser/*.cc"
        "src/Runtime/*.h"
        "src/Runtime/*.cc"
        )
file(GLOB tanuki_test_SRC
        "test/*.h"
        "test/*.cc"
        )
file(GLOB dynamic_SRC
        "dynamic/*.h"
        "dynamic/*.cc"
        )

add_library(tanuki_lib ${tanuki_SRC})
target_link_libraries(tanuki_lib dl)

add_executable(tanuki main.cc)
target_link_libraries(tanuki tanuki_lib dl)

add_library(tanuki_dynamic SHARED ${dynamic_SRC})
target_link_libraries(tanuki_dynamic tanuki_lib)

set(CTEST_OUTPUT_ON_FAILURE ON)
enable_testing()
add_executable(tanuki_test ${tanuki_test_SRC})
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # using GCC
    set_target_properties(tanuki_test PROPERTIES COMPILE_FLAGS " -O1")
endif ()
target_link_libraries(tanuki_test tanuki_lib gtest dl)
add_test(NAME tanuki_test COMMAND tanuki_test)

#add_subdirectory(pikatanuki)

include(GNUInstallDirs)

install(TARGETS tanuki DESTINATION bin)
install(TARGETS tanuki_dynamic LIBRARY DESTINATION lib)
install(PROGRAMS tanuki-lib.tnk DESTINATION lib/tanuki RENAME 狸図書館.tnk)
install(PROGRAMS tanuki-alias.sh DESTINATION bin RENAME 狸)
