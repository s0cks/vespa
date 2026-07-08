add_executable(vespa-test-wasm lib.c)

target_compile_options(vespa-test-wasm
  PRIVATE --target=wasm32
          -O3
          -nostdlib)

target_link_options(vespa-test-wasm
  PRIVATE --target=wasm32
          -nostdlib
          -Wl,--no-entry
          -Wl,--export-all)

# target_link_libraries(vespa-test-wasm
#   PRIVATE vespa::protocol)

set_target_properties(vespa-test-wasm PROPERTIES
    SUFFIX ".wasm"
    LINK_DEPENDS_NO_SHARED TRUE)
