find_path(FLATCC_INCLUDE_DIR NAMES flatcc/flatcc_prologue.h)
find_library(FLATCC_RUNTIME_LIB NAMES flatccrt)
find_library(FLATCC_BUILD_LIB NAMES flatcc) # Only needed if doing compiler-level work

# 3. Create a custom target to mirror the expected target look feel
if(FLATCC_INCLUDE_DIR AND FLATCC_RUNTIME_LIB)
    add_library(flatcc::flatccrt UNKNOWN IMPORTED)
    set_target_properties(flatcc::flatccrt PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${FLATCC_INCLUDE_DIR}"
        IMPORTED_LOCATION "${FLATCC_RUNTIME_LIB}"
    )
else()
    message(FATAL_ERROR "Could not find flatcc runtime library or headers inside vcpkg!")
endif()

if(NOT TARGET flatcc_cli)
  find_program(FLATCC_COMPILER_BIN
    NAMES flatcc
    PATHS 
      "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/tools/flatcc"
      "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/tools"
      "${VCPKG_INSTALLED_DIR}/x64-linux/tools/flatcc"
      "${VCPKG_INSTALLED_DIR}/x64-linux/tools"
  )

  if(FLATCC_COMPILER_BIN)
    message(STATUS "Found flatcc compiler at: ${FLATCC_COMPILER_BIN}")
    add_executable(flatcc_cli IMPORTED GLOBAL)
    set_target_properties(flatcc_cli PROPERTIES
        IMPORTED_LOCATION "${FLATCC_COMPILER_BIN}"
    )
  else()
    message(FATAL_ERROR 
        "flatcc compiler executable not found!\n"
        "Checked VCPKG_INSTALLED_DIR: ${VCPKG_INSTALLED_DIR}\n"
        "Please check where the 'flatcc' binary is located inside your build directory."
    )
  endif()
endif()

set(VESPA_GENERATED_HEADERS_DIR "${CMAKE_CURRENT_BINARY_DIR}/vespa-protocol-headers/")

add_custom_command(
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/protocol_builder.h"
         "${CMAKE_CURRENT_BINARY_DIR}/protocol_reader.h"
         "${CMAKE_CURRENT_BINARY_DIR}/protocol_verifier.h"
  COMMAND $<TARGET_FILE:flatcc_cli> -c -w -v -o "${CMAKE_CURRENT_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/protocol.fbs"
  DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/protocol.fbs"
  COMMENT "Compiling C FlatBuffers schemas via fetched flatcc tool"
)

add_custom_command(
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/error_builder.h"
         "${CMAKE_CURRENT_BINARY_DIR}/error_reader.h"
         "${CMAKE_CURRENT_BINARY_DIR}/error_verifier.h"
  COMMAND $<TARGET_FILE:flatcc_cli> -c -w -v -o "${CMAKE_CURRENT_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/error.fbs"
  DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/error.fbs"
  COMMENT "Compiling C FlatBuffers schemas via fetched flatcc tool"
)

add_custom_command(
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/event_builder.h"
         "${CMAKE_CURRENT_BINARY_DIR}/event_reader.h"
         "${CMAKE_CURRENT_BINARY_DIR}/event_verifier.h"
  COMMAND $<TARGET_FILE:flatcc_cli> -c -w -v -o "${CMAKE_CURRENT_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/event.fbs"
  DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/event.fbs"
  COMMENT "Compiling C FlatBuffers schemas via fetched flatcc tool"
)

add_custom_target(vespa-protocol-headers
  DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/protocol_builder.h"
          "${CMAKE_CURRENT_BINARY_DIR}/protocol_reader.h"
          "${CMAKE_CURRENT_BINARY_DIR}/protocol_verifier.h"
          "${CMAKE_CURRENT_BINARY_DIR}/error_builder.h"
          "${CMAKE_CURRENT_BINARY_DIR}/error_reader.h"
          "${CMAKE_CURRENT_BINARY_DIR}/error_verifier.h"
          "${CMAKE_CURRENT_BINARY_DIR}/event_builder.h"
          "${CMAKE_CURRENT_BINARY_DIR}/event_reader.h"
          "${CMAKE_CURRENT_BINARY_DIR}/event_verifier.h")

add_library(vespa-protocol INTERFACE)
add_dependencies(vespa-protocol vespa-protocol-headers)
target_include_directories(vespa-protocol
  INTERFACE "${CMAKE_CURRENT_BINARY_DIR}/")
target_link_libraries(vespa-protocol
  INTERFACE flatcc::flatccrt)
add_library(vespa::protocol ALIAS vespa-protocol)
