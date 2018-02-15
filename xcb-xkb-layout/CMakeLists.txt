cmake_minimum_required (VERSION 3.5 FATAL_ERROR)
project (xcb-xkb-keyboard LANGUAGES C VERSION 1.0)

set (xcb-xkb-keyboard_VERSION_MAJOR 1)
set (xcb-xkb-keyboard_VERSION_MINOR 1)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory (src)
