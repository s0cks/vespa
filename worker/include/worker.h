#ifndef VESPA_WORKER_H
#define VESPA_WORKER_H

#include <wasm_export.h>

typedef struct {
  wasm_module_t wasm_module;
  wasm_module_inst_t wasm_module_inst;
  wasm_exec_env_t wasm_exec_env;
} Worker;

bool WorkerInit(Worker* worker, uint8_t* bytes, const uint64_t num_bytes);
void WorkerFree(Worker* worker);

#endif  // VESPA_WORKER_H
