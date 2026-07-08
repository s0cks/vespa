configure_file("vespa.h.in" "${CMAKE_CURRENT_BINARY_DIR}/vespa.h")

target_include_directories(vespa-core
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
         "${CMAKE_CURRENT_BINARY_DIR}")
