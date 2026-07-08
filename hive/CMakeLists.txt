find_package(Vulkan REQUIRED)

add_executable(vespa-hive main.c)
target_link_libraries(vespa-hive
  PUBLIC vespa::core
         vespa::protocol
         vespa::manifest
         Vulkan::Vulkan)

add_subdirectory(include/)
add_subdirectory(src/)
