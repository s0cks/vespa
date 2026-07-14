FetchContent_Declare(
    wamr
    GIT_REPOSITORY https://github.com/bytecodealliance/wasm-micro-runtime
    GIT_TAG        main
)
FetchContent_MakeAvailable(wamr)

set(WAMR_BUILD_PLATFORM "linux")
set(WAMR_BUILD_INTERP 1)        # Enable Interpreter
set(WAMR_BUILD_AOT 0)           # Disable Ahead-of-Time compilation
set(WAMR_BUILD_LIBC_WASI 1)     # Enable WASI support

include(${wamr_SOURCE_DIR}/build-scripts/runtime_lib.cmake)

add_library(vespa-wamr STATIC ${WAMR_RUNTIME_LIB_SOURCE})
add_library(vespa::wamr ALIAS vespa-wamr)
target_link_libraries(vespa-wamr
  PUBLIC -lpthread -lm vmlib)
target_include_directories(vespa-wamr
  PUBLIC ${wamr_SOURCE_DIR}/core/iwasm/include
         ${wamr_SOURCE_DIR}/core/shared/utils)
