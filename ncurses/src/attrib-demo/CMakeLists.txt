add_executable (attrib-demo "")
target_link_libraries (attrib-demo ncurses)

target_sources (
  attrib-demo PRIVATE
    main.c
)

target_include_directories (
  attrib-demo PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

set_target_properties (attrib-demo PROPERTIES C_STANDARD 11)
target_compile_options (
  attrib-demo PRIVATE ${NCURSES_PLAYGROUND_COMMON_COMPILE_FLAGS}
)
