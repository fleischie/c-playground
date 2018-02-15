add_executable (xcb-xkb-layout "")
target_link_libraries (
  xcb-xkb-layout
    xcb
    xcb-xkb
    xkbcommon
    xkbcommon-x11
)

target_sources (
  xcb-xkb-layout PRIVATE
    main.c
)

target_include_directories (
  xcb-xkb-layout PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

set_target_properties (xcb-xkb-layout PROPERTIES C_STANDARD 11)
target_compile_options (
  xcb-xkb-layout PRIVATE
    -Wall
    -Wextra
    -Wno-unused-parameter
)
