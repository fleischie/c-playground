cmake_minimum_required (VERSION 3.14 FATAL_ERROR)
project (gen-passwd LANGUAGES C VERSION 1.0)

set (gen-passwd_VERSION_MAJOR 1)
set (gen-passwd_VERSION_MINOR 1)

list (APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake/modules)

find_package (sodium REQUIRED)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory (src)
