#include "orchestrator.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>
#include <wayland-client-protocol.h>
#include <wlr-layer-shell-unstable-v1-client.h>
#include <xdg-shell-client.h>

static inline void registry_global(void* data, struct wl_registry* registry, uint32_t name, const char* interface,
                                   uint32_t version) {
  Orchestrator* orc = (Orchestrator*)data;
  if (orc == nullptr)
    return;

#define DEFINE_CHECK(Field, Interface, Version)                    \
  else if (strcmp(interface, Interface.name) == 0) {               \
    Field = wl_registry_bind(registry, name, &Interface, Version); \
  }

  DEFINE_CHECK(orc->compositor, wl_compositor_interface, 6)
  DEFINE_CHECK(orc->output, wl_output_interface, 4)
  DEFINE_CHECK(orc->layer_shell, zwlr_layer_shell_v1_interface, 4)
}

static inline void registry_remove(void* data, struct wl_registry* registry, uint32_t name) {
  // not implemented
}

static const struct wl_registry_listener registry_listener = {
    .global = registry_global,
    .global_remove = registry_remove,
};

static inline void layer_configure(void* data, struct zwlr_layer_surface_v1* surface, uint32_t serial, uint32_t width,
                                   uint32_t height) {
  Orchestrator* orc = (Orchestrator*)data;
  zwlr_layer_surface_v1_ack_configure(surface, serial);
}

static inline void layer_closed(void* data, struct zwlr_layer_surface_v1* surface) {
  exit(0);
}

static const struct zwlr_layer_surface_v1_listener layer_listener = {
    .configure = layer_configure,
    .closed = layer_closed,
};

void OrchestratorInit(Orchestrator* orc) {
  orc->loop = uv_loop_new();
  orc->display = wl_display_connect(NULL);
  orc->registry = wl_display_get_registry(orc->display);
  wl_registry_add_listener(orc->registry, &registry_listener, orc);
  wl_display_roundtrip(orc->display);
  orc->surface = wl_compositor_create_surface(orc->compositor);
  orc->layer_surface = zwlr_layer_shell_v1_get_layer_surface(orc->layer_shell, orc->surface, orc->output,
                                                             ZWLR_LAYER_SHELL_V1_LAYER_TOP, "vespa");
  zwlr_layer_surface_v1_add_listener(orc->layer_surface, &layer_listener, orc);
  zwlr_layer_surface_v1_set_anchor(orc->layer_surface, ZWLR_LAYER_SURFACE_V1_ANCHOR_TOP |
                                                           ZWLR_LAYER_SURFACE_V1_ANCHOR_LEFT |
                                                           ZWLR_LAYER_SURFACE_V1_ANCHOR_RIGHT);
  zwlr_layer_surface_v1_set_size(orc->layer_surface, 0, 32);
  zwlr_layer_surface_v1_set_exclusive_zone(orc->layer_surface, 32);
}

void OrchestratorFree(Orchestrator* orc) {
  if (orc->loop)
    uv_loop_delete(orc->loop);
}

static inline void CreateVulkanInstance(Orchestrator* orc) {
  const char* extensions[] = {
      VK_KHR_SURFACE_EXTENSION_NAME,
      VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME,
  };

  VkApplicationInfo app_info = {
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pApplicationName = "vespa",
      .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
      .pEngineName = "vespa",
      .engineVersion = VK_MAKE_VERSION(1, 0, 0),
      .apiVersion = VK_API_VERSION_1_4,
  };
  VkInstanceCreateInfo create = {
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pApplicationInfo = &app_info,
      .enabledExtensionCount = 2,
      .ppEnabledExtensionNames = extensions,
  };

  VkResult result = vkCreateInstance(&create, NULL, &orc->instance);
  if (result != VK_SUCCESS) {
    fprintf(stderr, "failed to create vk instance\n");
    exit(1);
  }

  fprintf(stdout, "created vk instance\n");
}

static void CreateVulkanSurface(Orchestrator* orc) {
  VkWaylandSurfaceCreateInfoKHR info = {
      .sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR,
      .display = orc->display,
      .surface = orc->surface,
  };

  VkResult result = vkCreateWaylandSurfaceKHR(orc->instance, &info, NULL, &orc->vk_surface);
  if (result != VK_SUCCESS) {
    fprintf(stderr, "failed to create vk wayland surface\n");
    exit(1);
  }

  fprintf(stdout, "vk wayland surface created\n");
}

int OrchestratorRun(Orchestrator* orc, const int mode) {
  wl_surface_commit(orc->surface);
  wl_display_roundtrip(orc->display);

  CreateVulkanInstance(orc);
  CreateVulkanSurface(orc);

  if (orc->loop)
    return uv_run(orc->loop, mode);
  return 0;
}
