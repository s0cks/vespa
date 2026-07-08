file(GLOB VESPA_COMB_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cc")

target_sources(vespa-comb
  PUBLIC "${VESPA_COMB_SOURCES}")
target_include_directories(vespa-comb
  PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/")
