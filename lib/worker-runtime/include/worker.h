#ifndef VESPA_WORKER_H
#define VESPA_WORKER_H

#include <uv.h>

#include "sandbox.h"

typedef struct {
  uv_loop_t* loop;
  uv_idle_t idle;
  uv_check_t check;
  Sandbox* sandbox;
  bool has_updates;
} Worker;

bool WorkerInit(Worker* worker, uv_loop_t*);
bool WorkerLoadWasm(Worker* worker, const char* filename);
bool WorkerWidgetInit(Worker* worker);
bool WorkerWidgetView(Worker* worker);
bool WorkerWidgetUpdate(Worker* worker);
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
