add_executable(vespa-queen main.c)

add_subdirectory(include/)
add_subdirectory(src/)

target_link_libraries(vespa-queen
  PRIVATE vespa::core)
