file(GLOB VESPA_QUEEN_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/*.c")

target_sources(vespa-queen
  PUBLIC ${VESPA_QUEEN_SOURCES})
target_include_directories(vespa-queen
  PRIVATE "${CMKAE_CURRENT_SOURCE_DIR}/")
