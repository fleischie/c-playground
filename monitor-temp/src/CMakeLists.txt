add_executable (monitor-temp "")
target_link_libraries (
  monitor-temp
    sensors
    ${notify_LIBRARIES}
)

target_sources (
  monitor-temp PRIVATE
    main.c
)

target_include_directories (
  monitor-temp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

set_target_properties (monitor-temp PROPERTIES C_STANDARD 11)
target_compile_options (
  monitor-temp PRIVATE
    -Wall
    -Wextra
    -Wno-unused-parameter
)
