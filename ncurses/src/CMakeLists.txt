configure_file (
  ${CMAKE_CURRENT_LIST_DIR}/ncurses-playground.h.in
  ${CMAKE_BINARY_DIR}/ncurses-playground.h)

include_directories (${CMAKE_BINARY_DIR})

add_subdirectory (hello-world)
add_subdirectory (print-bold)
add_subdirectory (cascade)
add_subdirectory (password-prompt)
add_subdirectory (attrib-demo)
add_subdirectory (windowing)
