add_library(vespa-worker-runtime SHARED)
add_library(vespa::worker-runtime ALIAS vespa-worker-runtime)
target_link_libraries(vespa-worker-runtime
  PUBLIC vespa::core
         vespa::wamr
         vespa::sandbox)

add_subdirectory(include/)
add_subdirectory(src/)
