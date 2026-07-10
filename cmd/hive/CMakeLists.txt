find_package(Vulkan REQUIRED)

add_executable(vespa-hive main.c)
target_link_libraries(vespa-hive
  PUBLIC vespa::core
         vespa::manifest
         vespa::ipc
         Vulkan::Vulkan)

add_subdirectory(include/)
add_subdirectory(src/)

add_executable(vespa-hive-client client.c)
target_link_libraries(vespa-hive-client
  PUBLIC vespa::core
         vespa::ipc)
