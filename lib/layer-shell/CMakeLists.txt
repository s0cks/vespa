pkg_get_variable(WAYLAND_SCANNER        wayland-scanner     wayland_scanner)
pkg_get_variable(WLR_PROTOCOLS_DIR wlr-protocols pkgdatadir)

set(WLR_LAYER_SHELL_VERSION 1)
set(WLR_LAYER_SHELL_BRANCH "unstable")
set(WLR_LAYER_SHELL_NAME "wlr-layer-shell-${WLR_LAYER_SHELL_BRANCH}-v${WLR_LAYER_SHELL_VERSION}")
set(WLR_LAYER_SHELL_XML "${WLR_PROTOCOLS_DIR}/${WLR_LAYER_SHELL_BRANCH}/${WLR_LAYER_SHELL_NAME}.xml")

macro(exec_wayland_scanner target out)
  execute_process(
    COMMAND ${WAYLAND_SCANNER}
            ${target}
            ${WLR_LAYER_SHELL_XML}
            ${CMAKE_CURRENT_BINARY_DIR}/${out}
  )
endmacro()

exec_wayland_scanner(server-header "${WLR_LAYER_SHELL_NAME}-server.h")
exec_wayland_scanner(client-header "${WLR_LAYER_SHELL_NAME}-client.h")
exec_wayland_scanner(private-code  "${WLR_LAYER_SHELL_NAME}.c")

add_library(vespa-layer-shell SHARED)
add_library(vespa::layer-shell ALIAS vespa-layer-shell)
target_include_directories(vespa-layer-shell
  PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(vespa-layer-shell
  PUBLIC vespa::xdg-shell)
target_sources(vespa-layer-shell
  PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/${WLR_LAYER_SHELL_NAME}.c)
