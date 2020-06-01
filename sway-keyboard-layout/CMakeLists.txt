cmake_minimum_required (VERSION 3.5 FATAL_ERROR)
project (sway-keyboard-layout LANGUAGES C VERSION 1.0)

set (sway-keyboard-layout_VERSION_MAJOR 1)
set (sway-keyboard-layout_VERSION_MINOR 1)

# gather dependencies
find_package (PkgConfig REQUIRED)
pkg_search_module (JSON REQUIRED json-c)
include_directories(${JSON_INCLUDE_DIRS})

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory (src)
