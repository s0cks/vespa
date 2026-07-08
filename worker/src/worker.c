#include "worker.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef WORKER_WASM_ERRBUF_SIZE
#define WORKER_WASM_ERRBUF_SIZE 128
#endif  // WORKER_WASM_ERRBUF_SIZE

#ifndef WORKER_WASM_RUNTIME_SIZE
#define WORKER_WASM_RUNTIME_SIZE 8192
#endif  // WORKER_WASM_RUNTIME_SIZE

bool WorkerInit(Worker* worker, uint8_t* bytes, const uint64_t num_bytes) {
  bool success = false;
  char error_buf[WORKER_WASM_ERRBUF_SIZE];

  RuntimeInitArgs init_args;
  memset(&init_args, 0, sizeof(RuntimeInitArgs));
  init_args.mem_alloc_type = Alloc_With_Allocator;
  init_args.mem_alloc_option.allocator.malloc_func = malloc;
  init_args.mem_alloc_option.allocator.realloc_func = realloc;
  init_args.mem_alloc_option.allocator.free_func = free;

  if (!wasm_runtime_full_init(&init_args)) {
    printf("error: failed to initialize WAMR runtime.\n");
    goto failed;
  }

  worker->wasm_module = wasm_runtime_load(bytes, num_bytes, error_buf, sizeof(error_buf));
  if (!worker->wasm_module) {
    fprintf(stderr, "error: WASM module failed: %s\n", error_buf);
    goto failed;
  }

  worker->wasm_module_inst = wasm_runtime_instantiate(worker->wasm_module, WORKER_WASM_RUNTIME_SIZE,
                                                      WORKER_WASM_RUNTIME_SIZE, error_buf, sizeof(error_buf));
  if (!worker->wasm_module_inst) {
    fprintf(stderr, "error: init WASM module failed: %s\n", error_buf);
    goto failed;
  }

  worker->wasm_exec_env = wasm_runtime_create_exec_env(worker->wasm_module_inst, WORKER_WASM_RUNTIME_SIZE);
  if (!worker->wasm_exec_env) {
    fprintf(stderr, "error: create execution environment failed.\n");
    goto failed;
  }

  success = true;
  goto finished;
failed:
  success = false;
  if (worker->wasm_module_inst)
    wasm_runtime_deinstantiate(worker->wasm_module_inst);
  if (worker->wasm_module)
    wasm_runtime_unload(worker->wasm_module);
finished:
  return success;
}

void WorkerFree(Worker* worker) {
  if (worker->wasm_exec_env)
    wasm_runtime_destroy_exec_env(worker->wasm_exec_env);
  if (worker->wasm_module_inst)
    wasm_runtime_deinstantiate(worker->wasm_module_inst);
  if (worker->wasm_module)
    wasm_runtime_unload(worker->wasm_module);
  wasm_runtime_destroy();
}
