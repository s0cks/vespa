add_library(vespa-c-wasm-sdk STATIC)

target_compile_options(vespa-c-wasm-sdk
  PRIVATE --target=wasm32
          -O3
          -nostdlib)

add_subdirectory(include/)
add_subdirectory(src/)
