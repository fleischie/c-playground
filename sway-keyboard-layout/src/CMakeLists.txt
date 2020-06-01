add_executable (sway-keyboard-layout "")
target_link_libraries (
  sway-keyboard-layout
    ${JSON_LIBRARIES}
)

target_sources (
  sway-keyboard-layout PRIVATE
    main.c
    ipc.c
)

target_include_directories (
  sway-keyboard-layout PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

set_target_properties (sway-keyboard-layout PROPERTIES C_STANDARD 11)
target_compile_options (
  sway-keyboard-layout PRIVATE
    -Wall
    -Wextra
    -Wno-unused-parameter
)
