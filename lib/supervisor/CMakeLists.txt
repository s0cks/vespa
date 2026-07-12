add_library(vespa-supervisor SHARED)
target_link_libraries(vespa-supervisor
  PUBLIC vespa::core)

add_subdirectory(include/)
add_subdirectory(src/)
