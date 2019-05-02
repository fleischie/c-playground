add_executable (gen-passwd "")
target_link_libraries (
  gen-passwd
    sodium
)

target_sources (
  gen-passwd PRIVATE
    main.c
)

target_include_directories (
  gen-passwd PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

set_target_properties (gen-passwd PROPERTIES C_STANDARD 11)
target_compile_options (
  gen-passwd PRIVATE
    -Wall
    -Wextra
    -Wno-unused-parameter
)
