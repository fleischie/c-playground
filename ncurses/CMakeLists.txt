cmake_minimum_required (VERSION 3.5 FATAL_ERROR)
project (ncurses-playground LANGUAGES C VERSION 1.0)

set (ncurses-playground_VERSION_MAJOR 1)
set (ncurses-playground_VERSION_MINOR 0)

list (APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake/modules)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include (nCursesPlaygroundCompileFlags)

add_subdirectory (src)
