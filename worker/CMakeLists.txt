include(wasm3)

add_executable(vespa-worker main.c)

add_subdirectory(include/)
add_subdirectory(src/)

target_link_libraries(vespa-worker
  PUBLIC vespa-core
         vespa-wl
         vespa-wamr)
