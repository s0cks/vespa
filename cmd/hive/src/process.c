#include <stdlib.h>
#include <string.h>

#include "orchestrator.h"

typedef struct {
  int num_processes;
  Process processes[VESPA_MAX_PROCESSES];
} ProcessPool;

static ProcessPool pool = {
    .num_processes = 0,
};

static inline void ProcessFree(Process* proc) {
  if (proc->argv) {
    for (int i = 0; i < proc->argc; i++) {
      if (proc->argv[i])
        free(proc->argv[i]);
    }

    free(proc->argv);
  }

  memset(proc, 0, sizeof(Process));
}

static inline void OnProcessClosed(uv_handle_t* handle) {
  Process* proc = (Process*)handle->data;
  ProcessFree(proc);
}

static inline void OnProcessExit(uv_process_t* handle, int64_t status, int term_signal) {
  fprintf(stdout, "process %d exited. status: %ld\n", handle->pid, status);
  pool.num_processes--;
  uv_close((uv_handle_t*)handle, &OnProcessClosed);
}

int OrchestratorSpawn(Orchestrator* orc, int argc, char** argv) {
  int slot = -1;

  if (pool.num_processes >= VESPA_MAX_PROCESSES) {
    fprintf(stderr, "error: process pool is full\n");
    goto finished;
  }

  for (int i = 0; i < VESPA_MAX_PROCESSES; i++) {
    if (pool.processes[i].handle.pid == 0) {
      slot = i;
      break;
    }
  }

  if (slot == -1) {
    fprintf(stderr, "error: no free slot\n");
    goto finished;
  }

  Process* proc = &pool.processes[slot];
  proc->argc = argc;
  proc->argv = (char**)malloc(sizeof(const char*) * argc);
  for (int i = 0; i < argc; i++)
    proc->argv[i] = strdup(argv[i]);

  proc->options.args = proc->argv;
  proc->options.file = proc->argv[0];
  proc->options.exit_cb = &OnProcessExit;
  proc->handle.data = proc;

  int result = uv_spawn(orc->loop, &proc->handle, &proc->options);
  if (result) {
    fprintf(stderr, "error: failed to spawn process: %s\n", uv_strerror(result));
    return result;
  }

  pool.num_processes++;
  fprintf(stdout, "spanwed process %d: %s\n", proc->handle.pid, proc->argv[0]);
finished:
  return slot;
}

void OrchestratorKillAllProcesses(Orchestrator* orc) {
#ifdef VESPA_DEBUG
  fprintf(stdout, "killing all processes...\n");
#endif  // VESPA_DEBUG

  for (int i = 0; i < VESPA_MAX_PROCESSES; i++) {
    Process* proc = &pool.processes[i];
    if (proc->handle.pid > 0) {
#ifdef VESPA_DEBUG
      fprintf(stdout, "killing process %d...\n", proc->handle.pid);
#endif  // VESPA_DEBUG
      int status = uv_process_kill(&proc->handle, SIGKILL);
      if (status)
        fprintf(stderr, "failed to kill process %d: %s\n", proc->handle.pid, uv_strerror(status));
    }
  }
}
