add_executable (loadavg "")

target_sources (
  loadavg PRIVATE
    main.c
)

target_include_directories (
  loadavg PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

set_target_properties (loadavg PROPERTIES C_STANDARD 11)
target_compile_options (
  loadavg PRIVATE
    -Wall
    -Wextra
    -Wno-unused-parameter
)
