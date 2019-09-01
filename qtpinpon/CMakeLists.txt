project(qtpinpon)


set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "/Users/pivotal/Qt/5.13.0/clang_64/")

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt5 COMPONENTS Widgets)

add_executable(qtpinpon
        mainwindow.ui
        mainwindow.cpp
        main.cpp
        resources.qrc
        )

target_link_libraries(qtpinpon Qt5::Widgets)
target_link_libraries(qtpinpon pinpon_lib)

