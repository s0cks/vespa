add_library(vespa-wl INTERFACE)
add_library(vespa::wl ALIAS vespa-wl)
pkg_check_modules(WAYLAND_CLIENT    REQUIRED IMPORTED_TARGET wayland-client)
pkg_check_modules(WAYLAND_PROTOCOLS REQUIRED IMPORTED_TARGET wayland-protocols)
target_link_libraries(vespa-wl
  INTERFACE PkgConfig::WAYLAND_CLIENT
            PkgConfig::WAYLAND_PROTOCOLS)

pkg_check_modules(WAYLAND_CURSOR IMPORTED_TARGET wayland-cursor)
if(WAYLAND_CURSOR_FOUND)
  target_link_libraries(vespa-wl
    INTERFACE PkgConfig::WAYLAND_CURSOR)
endif()

pkg_check_modules(WAYLAND_EGL    IMPORTED_TARGET wayland-egl)
if(WAYLAND_EGL_FOUND)
  pkg_check_modules(EGL   REQUIRED IMPORTED_TARGET egl)
  pkg_check_modules(GLES2 REQUIRED IMPORTED_TARGET glesv2)
  target_link_libraries(vespa-wl
    INTERFACE PkgConfig::GLES2
              PkgConfig::EGL
              PkgConfig::WAYLAND_EGL)
endif()
