#include <stdio.h>
#include <string.h>

#include "log.h"
#include "supervisor.h"
#include "supervisor_def.h"

static inline void OnChildExit(uv_process_t* req, int64_t exit_status, int term_signal) {
  LOG_INFO("child exited with status %ld and signal %d", exit_status, term_signal);
  uv_close((uv_handle_t*)req, NULL);
}

bool SupervisorSpawnHive(Supervisor* super, SupervisorProcess* proc) {
  LOG_DEBUG("spawning hive process....");
  LOG_DEBUG("hive executable path: %s", super->config.hive_bin);

  char* args[2];
  args[0] = super->config.hive_bin;
  args[1] = NULL;

  memset(&proc->options, 0, sizeof(uv_process_options_t));
  proc->options.exit_cb = &OnChildExit;
  proc->options.file = super->config.hive_bin;
  proc->options.args = args;
  proc->options.flags = 0;
  LOG_INFO("superawning worker process %s....", super->config.hive_bin);
  const int result = uv_spawn(super->loop, &proc->process, &proc->options);
  if (result) {
    LOG_ERROR("failed to spawn worker process: %s", uv_strerror(result));
    return false;
  }

  return true;
}
