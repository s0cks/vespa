#include "supervisor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>

#include "log.h"
#include "supervisor_def.h"

Supervisor* NewSupervisor(SupervisorConfig* config) {
  Supervisor* super = (Supervisor*)malloc(sizeof(Supervisor));
  uv_loop_t* loop = uv_loop_new();
  if (!loop)
    goto init_failed0;
  super->loop = loop;

  memset(&super->config, 0, sizeof(SupervisorConfig));
  if (config) {
    memcpy(&super->config, config, sizeof(SupervisorConfig));
    if (config->hive_bin)
      super->config.hive_bin = strdup(config->hive_bin);
  }

  if (!super->config.hive_bin) {
    LOG_ERROR("no hive executable superecified");
    goto init_failed1;
  }

  goto finished;
init_failed1:
  uv_loop_delete(loop);
init_failed0:
  free(super);
  super = NULL;
finished:
  return super;
}

bool SupervisorRun(Supervisor* super, const int mode) {
  LOG_DEBUG("running supervisor...");
  const int status = uv_run(super->loop, mode);
  // TODO(@s0cks): check status
  return status == 0;
}

void SupervisorFree(Supervisor* super) {
  if (!super)
    return;
  if (super->loop)
    uv_loop_delete(super->loop);
  if (super->config.hive_bin)
    free(super->config.hive_bin);
}
