#include "supervisor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>

#include "log.h"

bool SupervisorRun(Supervisor* sp, const int mode) {
  LOG_DEBUG("running supervisor...");
  const int status = uv_run(sp->loop, mode);
  // TODO(@s0cks): check status
  return status == 0;
}

bool SupervisorInit(Supervisor* sp, SupervisorConfig* config) {
  uv_loop_t* loop = uv_loop_new();
  if (!loop)
    return false;
  sp->loop = loop;

  memset(&sp->config, 0, sizeof(SupervisorConfig));
  if (config) {
    memcpy(&sp->config, config, sizeof(SupervisorConfig));
    if (config->hive_bin)
      sp->config.hive_bin = strdup(config->hive_bin);
  }

  if (!sp->config.hive_bin) {
    LOG_ERROR("no hive executable specified");
    return true;
  }

  if (!SupervisorSpawnHive(sp, &sp->hive)) {
    LOG_ERROR("failed to spawn hive process from: %s", sp->config.hive_bin);
    return false;
  }

  SupervisorKillHive(sp, 15);
  return true;
}

void SupervisorFree(Supervisor* sp) {
  if (!sp)
    return;
  if (sp->loop)
    uv_loop_delete(sp->loop);
}
