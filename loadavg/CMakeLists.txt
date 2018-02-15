cmake_minimum_required (VERSION 3.5 FATAL_ERROR)
project (loadavg LANGUAGES C VERSION 1.0)

set (loadavg_VERSION_MAJOR 1)
set (loadavg_VERSION_MINOR 0)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory (src)
