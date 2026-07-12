#include <stdio.h>
#include <string.h>

#include "log.h"
#include "supervisor.h"

static inline void OnChildExit(uv_process_t* req, int64_t exit_status, int term_signal) {
  LOG_INFO("child exited with status %ld and signal %d", exit_status, term_signal);
  uv_close((uv_handle_t*)req, NULL);
}

bool SupervisorSpawnHive(Supervisor* sp, SupervisorProcess* proc) {
  LOG_DEBUG("spawning hive process....");
  LOG_DEBUG("hive executable path: %s", sp->config.hive_bin);

  char* args[2];
  args[0] = sp->config.hive_bin;
  args[1] = NULL;

  memset(&proc->options, 0, sizeof(uv_process_options_t));
  proc->options.exit_cb = &OnChildExit;
  proc->options.file = sp->config.hive_bin;
  proc->options.args = args;
  proc->options.flags = 0;
  fprintf(stdout, "spawning worker process %s....\n", sp->config.hive_bin);
  const int result = uv_spawn(sp->loop, &proc->process, &proc->options);
  if (result) {
    fprintf(stderr, "error: failed to spawn child: %s\n", uv_strerror(result));
    return false;
  }

  return true;
}
