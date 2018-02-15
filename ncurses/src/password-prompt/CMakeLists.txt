add_executable (password-prompt "")
target_link_libraries (password-prompt ncurses)

target_sources (
  password-prompt PRIVATE
    main.c
)

target_include_directories (
  password-prompt PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

set_target_properties (password-prompt PROPERTIES C_STANDARD 11)
target_compile_options (
  password-prompt PRIVATE ${NCURSES_PLAYGROUND_COMMON_COMPILE_FLAGS}
)
