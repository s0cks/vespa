#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "sandbox_def.h"

static inline bool LoadWasmFile(Sandbox* box, const char* filename, uint8_t** bytes, uint64_t* nbytes) {
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

static inline bool InitWorkerWASMRuntime(Sandbox* box, const uint8_t* bytes, const uint64_t nbytes) {
  bool success = false;
  char error_buf[WORKER_WASM_ERRBUF_SIZE];

  RuntimeInitArgs init_args;
  InitRuntimeInitArgs(&init_args);

  if (!wasm_runtime_full_init(&init_args)) {
    LOG_ERROR("failed to initialize WASM runtime");
    goto failed;
  }

  InitSandboxBindings(box);

  box->wasm_module = wasm_runtime_load((unsigned char*)bytes, nbytes, error_buf, sizeof(error_buf));
  if (!box->wasm_module) {
    LOG_ERROR("failed to load WASM module: %s", error_buf);
    goto failed;
  }

  box->wasm_module_inst = wasm_runtime_instantiate(box->wasm_module, WORKER_WASM_RUNTIME_SIZE, WORKER_WASM_RUNTIME_SIZE,
                                                   error_buf, sizeof(error_buf));
  if (!box->wasm_module_inst) {
    LOG_ERROR("failed to init WASM module: %s", error_buf);
    goto failed;
  }

  box->wasm_exec_env = wasm_runtime_create_exec_env(box->wasm_module_inst, WORKER_WASM_RUNTIME_SIZE);
  if (!box->wasm_exec_env) {
    LOG_ERROR("failed to create WASM execution environment");
    goto failed;
  }

  success = true;
  goto finished;
failed:
  success = false;
  if (box->wasm_module_inst)
    wasm_runtime_deinstantiate(box->wasm_module_inst);
  if (box->wasm_module)
    wasm_runtime_unload(box->wasm_module);
finished:
  return success;
}

bool SandboxCallFunc(Sandbox* box, const char* name) {
  static const int kNumberOfArgs = 1;
  static const int kNumberOfReturns = 1;

  wasm_val_t args[kNumberOfArgs];

  bool success = false;
  wasm_val_t rets[kNumberOfReturns];
  rets[0].kind = WASM_I32;
  rets[0].of.i32 = (int32_t)success;

  wasm_function_inst_t func = wasm_runtime_lookup_function(box->wasm_module_inst, name);
  if (!func)
    goto cannot_find_func;

  if (!wasm_runtime_call_wasm_a(box->wasm_exec_env, func, 1, rets, 0, NULL)) {
    fprintf(stderr, "WASM execution failed: %s\n", wasm_runtime_get_exception(box->wasm_module_inst));
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

Sandbox* NewSandbox(uv_loop_t* loop) {
  Sandbox* result = NULL;
  if (!loop)
    goto finished;
  result = malloc(sizeof(Sandbox));
  if (!result)
    goto finished;

  memset(result, 0, sizeof(Sandbox));
  result->loop = loop;
  result->data = NULL;

finished:
  return result;
}

bool SandboxLoadWasm(Sandbox* box, const char* filename) {
  bool success = false;
  uint8_t* bytes = NULL;
  uint64_t nbytes = 0;
  if (!box) {
    goto failed;
  }

  if (!LoadWasmFile(box, filename, &bytes, &nbytes)) {
    LOG_ERROR("failed to load wasm file %s", filename);
    goto failed;
  }

  if (!InitWorkerWASMRuntime(box, bytes, nbytes)) {
    LOG_ERROR("failed to initialize wasm runtime");
    goto failed;
  }

  success = true;
  goto finished;
failed:
  if (bytes)
    free(bytes);
finished:
  return success;
}

void SandboxFree(Sandbox* box) {
  if (!box)
    return;
  if (box->wasm_exec_env)
    wasm_runtime_destroy_exec_env(box->wasm_exec_env);
  if (box->wasm_module_inst)
    wasm_runtime_deinstantiate(box->wasm_module_inst);
  if (box->wasm_module)
    wasm_runtime_unload(box->wasm_module);
  wasm_runtime_destroy();
  free(box);
}
