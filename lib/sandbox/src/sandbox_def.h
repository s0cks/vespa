#ifndef VESPA_SANDBOX_DEF_H
#define VESPA_SANDBOX_DEF_H

#include <wasm_export.h>

#include "sandbox.h"

#ifndef WORKER_WASM_ERRBUF_SIZE
#define WORKER_WASM_ERRBUF_SIZE 128
#endif  // WORKER_WASM_ERRBUF_SIZE

#ifndef WORKER_WASM_RUNTIME_SIZE
#define WORKER_WASM_RUNTIME_SIZE 8192
#endif  // WORKER_WASM_RUNTIME_SIZE

struct _Sandbox {
  uv_loop_t* loop;
  void* data;

  wasm_module_t wasm_module;
  wasm_module_inst_t wasm_module_inst;
  wasm_exec_env_t wasm_exec_env;

  wasm_function_inst_t init_func;
  wasm_function_inst_t update_func;
  wasm_function_inst_t view_func;
};

void InitSandboxBindings(Sandbox* box);

#endif  // VESPA_SANDBOX_DEF_H
