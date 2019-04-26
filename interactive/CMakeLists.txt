cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_FLAGS "-O3 -Wall -Wextra")

PROJECT(pinpon-interactive C)

FIND_PACKAGE(PkgConfig REQUIRED)
PKG_CHECK_MODULES(GTK3 REQUIRED gtk+-3.0)

# Setup CMake to use GTK+, tell the compiler where to look for headers
# and to the linker where to look for libraries
INCLUDE_DIRECTORIES(${GTK3_INCLUDE_DIRS})
LINK_DIRECTORIES(${GTK3_LIBRARY_DIRS})

# Add other flags to the compiler
ADD_DEFINITIONS(${GTK3_CFLAGS_OTHER})

set(PATH_TO_GCR_CMAKE_PARENT_DIR .)
list(APPEND CMAKE_MODULE_PATH macros)
include(macros/GlibCompileResourcesSupport.cmake)
compile_gresources(RESOURCE_FILE
        tmpresources.xml
        TYPE EMBED_C
        PREFIX application/pinponi
        RESOURCES style.css)
add_custom_target(resource ALL DEPENDS ${RESOURCE_FILE})

# Add an executable compiled from hello.c
ADD_EXECUTABLE(pinponi main.cc ${RESOURCE_FILE})
ADD_DEPENDENCIES(pinponi resource)

# Link the target to the GTK+ libraries
TARGET_LINK_LIBRARIES(pinponi ${GTK3_LIBRARIES})
