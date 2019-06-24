cmake_minimum_required (VERSION 3.14 FATAL_ERROR)
project (monitor-temp LANGUAGES C VERSION 1.0)

set (monitor-temp_VERSION_MAJOR 1)
set (monitor-temp_VERSION_MINOR 0)

list (APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake/modules)

# gather libnotify dependencies
find_package (PkgConfig REQUIRED)
pkg_search_module (notify REQUIRED libnotify)
include_directories (${notify_INCLUDE_DIRS})

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory (src)
