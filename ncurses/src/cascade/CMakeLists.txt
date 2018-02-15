add_executable (cascade "")
target_link_libraries (cascade ncurses)

target_sources (
  cascade PRIVATE
    main.c
)

set_target_properties (cascade PROPERTIES C_STANDARD 11)
target_compile_options (
  cascade PRIVATE ${NCURSES_PLAYGROUND_COMMON_COMPILE_FLAGS}
)
