configure_file (
  ${CMAKE_CURRENT_LIST_DIR}/monitor-battery.h.in
  ${CMAKE_BINARY_DIR}/monitor-battery.h)

include_directories (${CMAKE_BINARY_DIR})

add_executable (monitor-battery "")
target_link_libraries (
  monitor-battery
    ${notify_LIBRARIES}
)

target_sources (
  monitor-battery PRIVATE
    main.c
)

target_include_directories (
  monitor-battery PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

set_target_properties (monitor-battery PROPERTIES C_STANDARD 11)
target_compile_options (
  monitor-battery PRIVATE
    -Wall
    -Wextra
    -Wno-unused-parameter
)
