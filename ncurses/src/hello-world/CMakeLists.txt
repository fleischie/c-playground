add_executable (hello-world "")
target_link_libraries (hello-world ncurses)

target_sources (
  hello-world PRIVATE
    main.c
)

target_include_directories (
  hello-world PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

set_target_properties (hello-world PROPERTIES C_STANDARD 11)
target_compile_options (
  hello-world PRIVATE ${NCURSES_PLAYGROUND_COMMON_COMPILE_FLAGS}
)
