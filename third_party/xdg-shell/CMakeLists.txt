pkg_get_variable(WAYLAND_SCANNER        wayland-scanner     wayland_scanner)
pkg_get_variable(WAYLAND_PROTOCOLS_DIR  wayland-protocols   pkgdatadir)

macro(exec_wayland_scanner mode out)
  execute_process(
    COMMAND ${WAYLAND_SCANNER}
            ${mode}
            "${WAYLAND_PROTOCOLS_DIR}/stable/xdg-shell/xdg-shell.xml"
            ${CMAKE_CURRENT_BINARY_DIR}/${out}
  )
endmacro()

exec_wayland_scanner(client-header  xdg-shell-client.h)
exec_wayland_scanner(private-code   xdg-shell.c)

add_library(vespa-xdg-shell   SHARED)
add_library(vespa::xdg-shell  ALIAS vespa-xdg-shell)
target_include_directories(vespa-xdg-shell
  PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_sources(vespa-xdg-shell
  PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/xdg-shell.c)
