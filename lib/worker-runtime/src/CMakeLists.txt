file(GLOB VESPA_WORKER_RUNTIME_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cc")

target_sources(vespa-worker-runtime
  PUBLIC ${VESPA_WORKER_RUNTIME_SOURCES})
target_include_directories(vespa-worker-runtime
  PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/")
