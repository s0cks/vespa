file(GLOB VESPA_WASM_C_SDK
  "${CMAKE_CURRENT_SOURCE_DIR}/*.c")

target_sources(vespa-c-wasm-sdk
  PUBLIC ${VESPA_WASM_C_SDK})
