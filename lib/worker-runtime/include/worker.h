#ifndef VESPA_WORKER_H
#define VESPA_WORKER_H

#include <uv.h>

#include "sandbox.h"

typedef struct {
  uv_loop_t* loop;

  uv_idle_t idle;
  uv_check_t check;
  bool has_updates;

  Sandbox* sandbox;
} Worker;

Worker* NewWorker(uv_loop_t* loop, const char* filename);
bool WorkerRun(Worker* worker, const uv_run_mode mode);
void WorkerFree(Worker* worker);

static inline bool WorkerRunDefault(Worker* worker) {
  return WorkerRun(worker, UV_RUN_DEFAULT);
}

static inline bool WorkerRunOnce(Worker* worker) {
  return WorkerRun(worker, UV_RUN_ONCE);
}

static inline bool WorkerRunNoWait(Worker* worker) {
  return WorkerRun(worker, UV_RUN_NOWAIT);
}

#endif  // VESPA_WORKER_H
