add_library(vespa-render-ipc SHARED)
target_link_libraries(vespa-render-ipc
  PUBLIC vespa::core)

add_subdirectory(include/)
add_subdirectory(src/)
