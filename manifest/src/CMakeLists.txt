file(GLOB VESPA_MANIFEST_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cc")

target_sources(vespa-manifest
  PUBLIC ${VESPA_MANIFEST_SOURCES})
target_include_directories(vespa-manifest
  PRIVATE "${CMKAE_CURRENT_SOURCE_DIR}/")
