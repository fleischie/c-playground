add_executable (windowing "")
target_link_libraries (windowing ncurses)

target_sources (
  windowing PRIVATE
    main.c
)

set_target_properties (windowing PROPERTIES C_STANDARD 11)
target_compile_options (
  windowing PRIVATE ${NCURSES_PLAYGROUND_COMMON_COMPILE_FLAGS}
)
