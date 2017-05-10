cmake_minimum_required(VERSION 3.1.0)

project(scurvy C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wextra")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-parameter")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror")
if (CMAKE_COMPILER_IS_GNUCC)
    if(NOT CMAKE_C_COMPILER_VERSION VERSION_LESS 7.0)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wimplicit-fallthrough=0")
    endif()
endif()

list(INSERT CMAKE_MODULE_PATH 0
	${CMAKE_CURRENT_SOURCE_DIR}/CMake
)

find_package(Libvterm REQUIRED)
find_package(Cairo REQUIRED)
find_package(Pango REQUIRED)

add_subdirectory(src/wayland)
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${PANGO_INCLUDE_DIRS}
    ${CAIRO_INCLUDE_DIRS}
)

add_executable(scurvy
    src/main.c
    src/ini.c
    src/colors.c
    src/config.c
    src/log.c
    src/pango.c
    src/child.c
    src/term.c
    src/utf8.c
)

target_link_libraries(scurvy
    scurvy-wayland
    ${LIBVTERM_LIBRARIES}
)
