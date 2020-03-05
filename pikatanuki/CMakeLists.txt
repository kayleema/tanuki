project(pikatanuki)

file(GLOB pikatanuki_SRC
        "./*.h"
        "./*.cc"
        )

add_executable(pikatanuki
        ${pikatanuki_SRC}
        )

target_link_libraries(pikatanuki pinpon_lib)
