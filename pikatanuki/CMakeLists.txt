project(pikatanuki)

add_executable(pikatanuki
        main.cc
        )

target_link_libraries(pikatanuki pinpon_lib)
