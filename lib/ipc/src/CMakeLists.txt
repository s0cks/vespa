file(GLOB VESPA_IPC_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/*.c")

target_sources(vespa-ipc
  PUBLIC ${VESPA_IPC_SOURCES})
