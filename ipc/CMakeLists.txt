add_library(vespa-ipc STATIC)
target_link_libraries(vespa-ipc
  PUBLIC vespa::protocol
         vespa::core)

add_subdirectory(include/)
add_subdirectory(src/)

add_library(vespa::ipc ALIAS vespa-ipc)
