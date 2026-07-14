file(GLOB VESPA_SUPERVISOR_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/*.c")

target_include_directories(vespa-supervisor
  PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/")

target_sources(vespa-supervisor
  PUBLIC ${VESPA_SUPERVISOR_SOURCES})
