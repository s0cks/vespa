find_package(Vulkan REQUIRED)

add_executable(vespa-comb main.c)

add_subdirectory(include/)
add_subdirectory(src/)

target_link_libraries(vespa-comb
  PUBLIC vespa::core
         vespa::ipc
         Vulkan::Vulkan)
