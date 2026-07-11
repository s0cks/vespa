#ifndef VESPA_WORKER_H
#define VESPA_WORKER_H

#include <uv.h>
#include <wasm_export.h>

#ifndef WORKER_WASM_ERRBUF_SIZE
#define WORKER_WASM_ERRBUF_SIZE 128
#endif  // WORKER_WASM_ERRBUF_SIZE

#ifndef WORKER_WASM_RUNTIME_SIZE
#define WORKER_WASM_RUNTIME_SIZE 8192
#endif  // WORKER_WASM_RUNTIME_SIZE

typedef struct {
  uv_loop_t* loop;
  uv_idle_t idle;
  uv_check_t check;

  wasm_module_t wasm_module;
  wasm_module_inst_t wasm_module_inst;
  wasm_exec_env_t wasm_exec_env;

  bool has_updates;
} Worker;

bool WorkerInit(Worker* worker);
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
