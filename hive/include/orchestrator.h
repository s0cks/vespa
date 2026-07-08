#ifndef VESPA_ORCHESTRATOR_H
#define VESPA_ORCHESTRATOR_H

#include <uv.h>
#include <wlr-layer-shell-unstable-v1-client.h>
#include <xdg-shell-client.h>

#define VK_USE_PLATFORM_WAYLAND_KHR
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include "process.h"

#ifndef VESPA_MAX_PROCESSES
#define VESPA_MAX_PROCESSES 64
#endif  // VESPA_MAX_PROCESSES

typedef struct {
  uv_loop_t* loop;

  struct wl_display* display;
  struct wl_registry* registry;
  struct wl_compositor* compositor;
  struct wl_output* output;
  struct zwlr_layer_shell_v1* layer_shell;
  struct wl_surface* surface;
  struct zwlr_layer_surface_v1* layer_surface;

  VkInstance instance;
  VkSurfaceKHR vk_surface;
} Orchestrator;

int OrchestratorRun(Orchestrator* orc, const int mode);
void OrchestratorInit(Orchestrator* orc);
void OrchestratorFree(Orchestrator* orc);
void OrchestratorKillAllProcesses(Orchestrator* orc);

int OrchestratorSpawn(Orchestrator* orc, int argc, char** argv);

static inline int OrchestratorRunDefault(Orchestrator* orc) {
  return OrchestratorRun(orc, UV_RUN_DEFAULT);
}

static inline int OrchestratorRunOnce(Orchestrator* orc) {
  return OrchestratorRun(orc, UV_RUN_ONCE);
}

static inline int OrchestratorRunNoWait(Orchestrator* orc) {
  return OrchestratorRun(orc, UV_RUN_NOWAIT);
}

#endif  // VESPA_ORCHESTRATOR_H
