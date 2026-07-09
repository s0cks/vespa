find_program(GO_EXECUTABLE go REQUIRED)

file(GLOB_RECURSE GO_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/**/*.go")

set(EXE_NAME "vespa")
add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}/bin/${EXE_NAME}"
  COMMAND ${GO_EXECUTABLE} build -o "${CMAKE_BINARY_DIR}/bin/" ./cmd/...
  DEPENDS ${GO_SOURCES}
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  COMMENT "Building Go executable: ${EXE_NAME}"
  VERBATIM
)

add_custom_target(vespa-cli ALL DEPENDS "${CMAKE_BINARY_DIR}/bin/${EXE_NAME}")
