add_library(vespa-sandbox SHARED)
add_library(vespa::sandbox ALIAS vespa-sandbox)
target_link_libraries(vespa-sandbox
  PUBLIC vespa::core
         vespa::wamr)

add_subdirectory(include/)
add_subdirectory(src/)
