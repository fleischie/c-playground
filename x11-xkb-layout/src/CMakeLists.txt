add_executable (x11-xkb-layout "")
target_link_libraries (
  x11-xkb-layout
    X11
    xkbfile
)

target_sources (
  x11-xkb-layout PRIVATE
    main.c
)

target_include_directories (
  x11-xkb-layout PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

set_target_properties (x11-xkb-layout PROPERTIES C_STANDARD 11)
target_compile_options (
  x11-xkb-layout PRIVATE
    -Wall
    -Wextra
    -Wno-unused-parameter
)
