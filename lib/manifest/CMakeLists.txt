find_package(RapidJSON REQUIRED)
find_package(ryml REQUIRED)

add_library(vespa-manifest STATIC)

add_subdirectory(include/)
add_subdirectory(src/)

target_link_libraries(vespa-manifest
  PUBLIC vespa::core
         ryml::ryml)
target_compile_definitions(vespa-manifest
  PUBLIC VESPA_HAS_JSON
         VESPA_HAS_YAML)

add_library(vespa::manifest ALIAS vespa-manifest)
