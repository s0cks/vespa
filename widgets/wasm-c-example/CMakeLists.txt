add_executable(vespa-c-example main.c)

target_compile_options(vespa-c-example
  PRIVATE --target=wasm32
          -O3
          -nostdlib)

target_link_options(vespa-c-example
  PRIVATE --target=wasm32
          -nostdlib
          -Wl,--no-entry
          -Wl,--export-all
          -Wl,--allow-undefined)

target_link_libraries(vespa-c-example
  PUBLIC vespa-c-wasm-sdk)

set_target_properties(vespa-c-example PROPERTIES
    SUFFIX ".wasm"
    LINK_DEPENDS_NO_SHARED TRUE)
