add_executable(vespa-hive main.c)
target_link_libraries(vespa-hive
  PUBLIC vespa::core
         vespa::manifest
         vespa::ipc)

# add_subdirectory(include/)
# add_subdirectory(src/)
