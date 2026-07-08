add_executable(vespa-cli main.c)
target_link_libraries(vespa-cli
  PUBLIC vespa-core
         vespa-protocol)

add_subdirectory(include/)
add_subdirectory(src/)
