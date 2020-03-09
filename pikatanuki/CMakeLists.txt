project(pikatanuki)

find_package(nlohmann_json)

file(GLOB pikatanuki_SRC
        "./*.h"
        "./*.cc"
        )

add_executable(pikatanuki
        ${pikatanuki_SRC}
        )

target_link_libraries(pikatanuki pinpon_lib nlohmann_json::nlohmann_json)
