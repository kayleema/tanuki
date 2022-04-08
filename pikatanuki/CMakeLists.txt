project(pikatanuki)

find_package (Threads)

file(GLOB pikatanuki_SRC
        "./*.h"
        "./*.cc"
        )

add_executable(pikatanuki
        ${pikatanuki_SRC}
        )

target_link_libraries(pikatanuki tanuki_lib ${CMAKE_THREAD_LIBS_INIT})
