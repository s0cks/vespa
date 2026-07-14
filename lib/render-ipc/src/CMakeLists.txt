file(GLOB VESPA_RENDER_IPC_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cc")

target_sources(vespa-render-ipc
  PUBLIC ${VESPA_RENDER_IPC_SOURCES})

target_include_directories(vespa-render-ipc
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
