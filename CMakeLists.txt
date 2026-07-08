cmake_minimum_required(VERSION 4.3)
project(vepsa
  VERSION 0.0.1
  HOMEPAGE_URL https://github.com/s0cks/vespa
  LANGUAGES C CXX)

if(CMAKE_BUILD_TYPE MATCHES "^[Dd]ebug")
  add_compile_definitions(_NDEBUG)
  add_compile_definitions(VESPA_DEBUG)
endif()

find_package(PkgConfig REQUIRED)
include(FetchContent)
include(Wayland)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
add_subdirectory(third_party/)
add_subdirectory(protocol/)
add_subdirectory(ipc/)
add_subdirectory(core/)
add_subdirectory(manifest/)
add_subdirectory(queen/)
add_subdirectory(hive/)
add_subdirectory(comb/)
add_subdirectory(worker/)
add_subdirectory(widgets/)
add_subdirectory(cli/)
