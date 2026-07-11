#include "worker.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wasm_export.h"

static inline bool LoadWasmFile(Worker* worker, const char* filename, uint8_t** bytes, uint64_t* nbytes) {
  bool result = false;

  FILE* file = fopen(filename, "rb");
  if (!file) {
    fprintf(stderr, "error: failed to open %s\n", filename);
    goto finished;
  }
  fseek(file, 0, SEEK_END);
  uint32_t total_size = (uint32_t)ftell(file);
  fseek(file, 0, SEEK_SET);

  uint8_t* data = (uint8_t*)malloc(sizeof(uint8_t) * total_size);
  if (!data || fread(data, sizeof(uint8_t), total_size, file) != total_size) {
    fprintf(stderr, "error: failed to read WASM file contents.\n");
    goto close_and_finish;
  }

  (*bytes) = data;
  (*nbytes) = total_size;
  result = true;
close_and_finish:
  fclose(file);
finished:
  return result;
}

static inline void InitRuntimeInitArgs(RuntimeInitArgs* init_args) {
  memset(init_args, 0, sizeof(RuntimeInitArgs));
  init_args->mem_alloc_type = Alloc_With_Allocator;
  init_args->mem_alloc_option.allocator.malloc_func = malloc;
  init_args->mem_alloc_option.allocator.realloc_func = realloc;
  init_args->mem_alloc_option.allocator.free_func = free;
}

static inline void VespaPrintImpl(wasm_exec_env_t exec_env, const char* message, uint32_t message_len) {
  printf("WASM: %.*s\n", message_len, message);
}

static inline bool InitWorkerWASMRuntime(Worker* worker, const uint8_t* bytes, const uint64_t nbytes) {
  bool success = false;
  char error_buf[WORKER_WASM_ERRBUF_SIZE];

  RuntimeInitArgs init_args;
  InitRuntimeInitArgs(&init_args);

  if (!wasm_runtime_full_init(&init_args)) {
    fprintf(stderr, "failed to initialize WARM runtime\n");
    goto failed;
  }

  static NativeSymbol native_symbols[] = {
      {"VespaPrint", (void*)VespaPrintImpl, "(*~)", NULL},
  };
  wasm_runtime_register_natives("env", native_symbols, 1);

  worker->wasm_module = wasm_runtime_load((unsigned char*)bytes, nbytes, error_buf, sizeof(error_buf));
  if (!worker->wasm_module) {
    fprintf(stderr, "failed to load WASM module: %s\n", error_buf);
    goto failed;
  }

  worker->wasm_module_inst = wasm_runtime_instantiate(worker->wasm_module, WORKER_WASM_RUNTIME_SIZE,
                                                      WORKER_WASM_RUNTIME_SIZE, error_buf, sizeof(error_buf));
  if (!worker->wasm_module_inst) {
    fprintf(stderr, "failed to init WASM module: %s\n", error_buf);
    goto failed;
  }

  worker->wasm_exec_env = wasm_runtime_create_exec_env(worker->wasm_module_inst, WORKER_WASM_RUNTIME_SIZE);
  if (!worker->wasm_exec_env) {
    fprintf(stderr, "failed to create WASM execution environment\n");
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

bool WorkerLoadWasm(Worker* worker, const char* filename) {
  uint8_t* bytes = NULL;
  uint64_t nbytes = 0;
  if (!LoadWasmFile(worker, filename, &bytes, &nbytes))
    return false;
  if (!InitWorkerWASMRuntime(worker, bytes, nbytes))
    return false;
  return true;
}

bool WorkerRun(Worker* worker, const uv_run_mode mode) {
  const int status = uv_run(worker->loop, mode);
  if (status != 0) {
    fprintf(stderr, "failed to run worker loop: %s\n", uv_strerror(status));
    goto finished;
  }

finished:
  return status == 0;
}

#define VESPA_WIDGET_INIT_FUNC_NAME   "init"
#define VESPA_WIDGET_VIEW_FUNC_NAME   "view"
#define VESPA_WIDGET_UPDATE_FUNC_NAME "update"

static inline bool WorkerWidgetCallFunc(Worker* worker, const char* name) {
  static const int kNumberOfArgs = 1;
  static const int kNumberOfReturns = 1;

  wasm_val_t args[kNumberOfArgs];

  bool success = false;
  wasm_val_t rets[kNumberOfReturns];
  rets[0].kind = WASM_I32;
  rets[0].of.i32 = (int32_t)success;

  wasm_function_inst_t func = wasm_runtime_lookup_function(worker->wasm_module_inst, name);
  if (!func)
    goto cannot_find_func;

  if (!wasm_runtime_call_wasm_a(worker->wasm_exec_env, func, 1, rets, 0, NULL)) {
    fprintf(stderr, "WASM execution failed: %s\n", wasm_runtime_get_exception(worker->wasm_module_inst));
    goto errored;
  }

  goto success;
cannot_find_func:
  fprintf(stderr, "cannot find '%s' function in WASM module\n", name);
errored:
  goto finished;
success:
  success = rets[0].of.i32 != 0;
finished:
  return success;
}

bool WorkerWidgetInit(Worker* worker) {
  return WorkerWidgetCallFunc(worker, VESPA_WIDGET_INIT_FUNC_NAME);
}

bool WorkerWidgetView(Worker* worker) {
  return WorkerWidgetCallFunc(worker, VESPA_WIDGET_VIEW_FUNC_NAME);
}

bool WorkerWidgetUpdate(Worker* worker) {
  return WorkerWidgetCallFunc(worker, VESPA_WIDGET_UPDATE_FUNC_NAME);
}

static inline void OnIdle(uv_idle_t* handle) {
  Worker* worker = (Worker*)handle->data;
  if (worker->has_updates) {
    bool success = WorkerWidgetUpdate(worker);
  }
}

static inline void OnCheck(uv_check_t* handle) {
  Worker* worker = (Worker*)handle->data;
  if (worker->has_updates) {
    bool success = WorkerWidgetView(worker);
    worker->has_updates = false;
  }
}

bool WorkerInit(Worker* worker) {
  bool success = false;

  worker->has_updates = true;
  uv_loop_t* loop = uv_loop_new();
  if (!loop) {
    fprintf(stderr, "uv_loop_new() failed\n");
    goto finished;
  }
  worker->loop = loop;

  uv_idle_init(loop, &worker->idle);
  worker->idle.data = worker;
  uv_idle_start(&worker->idle, &OnIdle);

  uv_check_init(loop, &worker->check);
  worker->check.data = worker;
  uv_check_start(&worker->check, &OnCheck);

  success = true;
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
