add_executable (print-bold "")
target_link_libraries (print-bold ncurses)

target_sources (
  print-bold PRIVATE
    main.c
)

set_target_properties (print-bold PROPERTIES C_STANDARD 11)
target_compile_options (
  print-bold PRIVATE ${NCURSES_PLAYGROUND_COMMON_COMPILE_FLAGS}
)
