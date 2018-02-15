include (CheckCCompilerFlag)
check_c_compiler_flag ("-Og" C_COMPILER_SUPPORTS_-Og)

if (C_COMPILER_SUPPORTS_-Og)
  set (opt_lvl "-Og")
else ()
  set (opt_lvl "-O0")
endif ()

set (NCURSES_PLAYGROUND_COMMON_COMPILE_FLAGS "")
list (
  APPEND NCURSES_PLAYGROUND_COMMON_COMPILE_FLAGS
    -Wall
    -Wextra
    -Wno-unused-parameter
    $<$<CONFIG:Debug>:${opt_lvl}>
)
