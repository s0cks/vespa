#include "supervisor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>

static inline void OnChildExit(uv_process_t* req, int64_t exit_status, int term_signal) {
  fprintf(stdout, "child exited with status %lld signal %d\n", exit_status, term_signal);
  uv_close((uv_handle_t*)req, NULL);
}

static inline bool SupervisorKillHive(Supervisor* sp, const int signal) {
  // TODO(@s0cks): check state
  fprintf(stdout, "terminating hive process...\n");
  uv_process_kill(&sp->hive.process, signal);
  uv_run(sp->loop, UV_RUN_NOWAIT);
}

static inline bool SupervisorSpawnHive(Supervisor* sp, SupervisorProcess* proc) {
  char* args[2];
  args[0] = sp->hive_bin;
  args[1] = NULL;

  memset(&proc->options, 0, sizeof(uv_process_options_t));
  proc->options.exit_cb = &OnChildExit;
  proc->options.file = sp->hive_bin;
  proc->options.args = args;
  proc->options.flags = 0;
  fprintf(stdout, "spawning worker process %s....\n", sp->hive_bin);
  const int result = uv_spawn(sp->loop, &proc->process, &proc->options);
  if (result) {
    fprintf(stderr, "error: failed to spawn child: %s\n", uv_strerror(result));
    return false;
  }

  return true;
}

int SupervisorRun(Supervisor* sp, const int mode) {
#ifdef VESPA_DEBUG
  fprintf(stdout, "running supervisor...\n");
#endif  // VESPA_DEBUG
  return uv_run(sp->loop, mode);
}

bool SupervisorInit(Supervisor* sp) {
  uv_loop_t* loop = uv_loop_new();
  if (!loop)
    return false;
  sp->loop = loop;

  const char* hive_bin_env = getenv("VESPA_SUPERVISOR_WORKER_PATH");
  if (!hive_bin_env) {
    fprintf(stderr, "please define VESPA_SUPERVISOR_WORKER_PATH env variable\n");
    return false;
  }

  sp->hive_bin = strdup(hive_bin_env);
#ifdef VESPA_DEBUG
  fprintf(stdout, "spawning hive process %s....\n", sp->hive_bin);
#endif  // VESPA_DEBUG
  if (!SupervisorSpawnHive(sp, &sp->hive)) {
    fprintf(stderr, "failed to spawn hive process from %s\n", sp->hive_bin);
    return false;
  }

  SupervisorKillHive(sp, 15);
  return true;
}

void SupervisorFree(Supervisor* sp) {
  if (!sp)
    return;
  uv_loop_delete(sp->loop);
}
