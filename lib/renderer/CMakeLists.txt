find_package(Vulkan REQUIRED)

add_library(vespa-renderer SHARED)
target_link_libraries(vespa-renderer
  PUBLIC vespa::core
         vespa::render-ipc
         Vulkan::Vulkan)
add_library(vespa::renderer ALIAS vespa-renderer)

target_compile_definitions(vespa-renderer
  PUBLIC VESPA_HAS_VULKAN)

add_subdirectory(include/)
add_subdirectory(src/)
