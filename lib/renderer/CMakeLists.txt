add_library(vespa-renderer SHARED)
target_link_libraries(vespa-renderer
  PUBLIC vespa::core)
add_library(vespa::renderer ALIAS vespa-renderer)

add_subdirectory(include/)
add_subdirectory(src/)
