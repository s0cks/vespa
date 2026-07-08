pkg_check_modules(UV REQUIRED IMPORTED_TARGET libuv)

add_library(vespa-core STATIC "${VESPA_CORE_SOURCES}")
add_library(vespa::core ALIAS vespa-core)

add_subdirectory(include/)
add_subdirectory(src/)

target_link_libraries(vespa-core
  PUBLIC vespa::layer-shell
         vespa::wl
         PkgConfig::UV)
