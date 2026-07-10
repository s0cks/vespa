find_package(bson CONFIG REQUIRED)

add_library(vespa-ipc SHARED)
target_link_libraries(vespa-ipc
  PUBLIC vespa::core
         $<IF:$<TARGET_EXISTS:bson::static>,bson::static,bson::shared>)
add_library(vespa::ipc ALIAS vespa-ipc)

add_subdirectory(include/)
add_subdirectory(src/)
