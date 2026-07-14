file(GLOB VESPA_SANDBOX_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cc")

target_sources(vespa-sandbox
  PUBLIC ${VESPA_SANDBOX_SOURCES})
target_include_directories(vespa-sandbox
  PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/")
