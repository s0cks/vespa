add_library(vespa-render-ipc SHARED)
target_link_libraries(vespa-render-ipc
  PUBLIC vespa::core)
add_library(vespa::render-ipc ALIAS vespa-render-ipc)

add_subdirectory(include/)
add_subdirectory(src/)
