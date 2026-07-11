#include "supervisor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>

static inline void OnChildExit(uv_process_t* req, int64_t exit_status, int term_signal) {
  fprintf(stdout, "child exited with status %lld signal %d\n", exit_status, term_signal);
  uv_close((uv_handle_t*)req, NULL);
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

// #define WORKER_BIN "./worker"
//
// uv_loop_t *loop;
// uv_fs_event_t fs_watcher;
// uv_process_t child_req;
// uv_process_options_t options;
// int child_running = 0;
//
// // Forward declarations
// void start_worker();
// void kill_worker();
//
// // Callback when the child process exits
//
// // Callback when the worker binary is modified or replaced
// void on_file_change(uv_fs_event_t *handle, const char *filename, int events, int status) {
//     if (status < 0) return;
//
//     // Filter for the specific worker binary target
//     if (filename && strcmp(filename, "worker") == 0) {
//         printf("[Supervisor] %s changed! Hot reloading...\n", filename);
//
//         kill_worker();
//         start_worker();
//     }
// }
//
// void start_worker() {
// }
//
// void kill_worker() {
//     if (!child_running) return;
//
//     printf("[Supervisor] Terminating old worker...\n");
//     // Send SIGTERM for graceful exit, or SIGKILL (9) for instant termination
//     uv_process_kill(&child_req, 15);
//
//     // Force loop to process the exit callback immediately before spawning next
//     uv_run(loop, UV_RUN_NOWAIT);
// }

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

  return true;
}

void SupervisorFree(Supervisor* sp) {
  if (!sp)
    return;
  uv_loop_delete(sp->loop);
}
