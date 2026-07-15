#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "sandbox_def.h"
#include "util.h"

static inline void InitRuntimeInitArgs(RuntimeInitArgs* init_args) {
  memset(init_args, 0, sizeof(RuntimeInitArgs));
  init_args->mem_alloc_type = Alloc_With_Allocator;
  init_args->mem_alloc_option.allocator.malloc_func = malloc;
  init_args->mem_alloc_option.allocator.realloc_func = realloc;
  init_args->mem_alloc_option.allocator.free_func = free;
}

static inline bool InitWorkerWASMRuntime(Sandbox* box) {
  bool success = false;

  RuntimeInitArgs init_args;
  InitRuntimeInitArgs(&init_args);

  if (!wasm_runtime_full_init(&init_args)) {
    LOG_ERROR("failed to initialize WASM runtime");
    goto finished;
  }

  InitSandboxBindings(box);
  success = true;
  goto finished;
  // if (box->wasm_module_inst)
  //   wasm_runtime_deinstantiate(box->wasm_module_inst);
  // if (box->wasm_module)
  //   wasm_runtime_unload(box->wasm_module);
finished:
  return success;
}

Sandbox* NewSandbox(uv_loop_t* loop, const char* filename) {
  Sandbox* result = NULL;
  if (!loop)
    goto finished;
  result = malloc(sizeof(Sandbox));
  if (!result)
    goto finished;

  if (!InitWorkerWASMRuntime(result))
    goto finished;

  memset(result, 0, sizeof(Sandbox));
  result->loop = loop;
  result->data = NULL;

  uint64_t nbytes = 0;
  uint8_t* bytes = NULL;
  char error_buf[WORKER_WASM_ERRBUF_SIZE];
  if (!LoadWasmFile(filename, &bytes, &nbytes)) {
    LOG_ERROR("failed to load wasm file %s", filename);
    goto failed;
  }

  result->wasm_module = wasm_runtime_load((unsigned char*)bytes, nbytes, error_buf, sizeof(error_buf));
  if (!result->wasm_module) {
    LOG_ERROR("failed to load WASM module: %s", error_buf);
    goto failed;
  }

  result->wasm_module_inst = wasm_runtime_instantiate(result->wasm_module, WORKER_WASM_RUNTIME_SIZE,
                                                      WORKER_WASM_RUNTIME_SIZE, error_buf, sizeof(error_buf));
  if (!result->wasm_module_inst) {
    LOG_ERROR("failed to init WASM module: %s", error_buf);
    goto failed;
  }

  result->wasm_exec_env = wasm_runtime_create_exec_env(result->wasm_module_inst, WORKER_WASM_RUNTIME_SIZE);
  if (!result->wasm_exec_env) {
    LOG_ERROR("failed to create WASM execution environment");
    goto failed;
  }

#define LOOKUP_WASM_FUNC(Name)                                                         \
  result->Name##_func = wasm_runtime_lookup_function(result->wasm_module_inst, #Name); \
  if (!result->Name##_func)                                                            \
    goto failed;

  LOOKUP_WASM_FUNC(init);
  LOOKUP_WASM_FUNC(view);
  LOOKUP_WASM_FUNC(update);
#undef LOOKUP_WASM_FUNC

  goto finished;
failed:
  if (bytes)
    free(bytes);
finished:
  return result;
}

bool SandboxCallInit(Sandbox* box) {
  static const int kNumberOfArgs = 1;
  static const int kNumberOfReturns = 1;

  wasm_val_t args[kNumberOfArgs];

  bool success = false;
  wasm_val_t rets[kNumberOfReturns];
  rets[0].kind = WASM_I32;
  rets[0].of.i32 = (int32_t)success;

  if (!wasm_runtime_call_wasm_a(box->wasm_exec_env, box->init_func, 1, rets, 0, NULL)) {
    fprintf(stderr, "WASM execution failed: %s\n", wasm_runtime_get_exception(box->wasm_module_inst));
    goto errored;
  }

  goto success;
cannot_find_func:
  fprintf(stderr, "cannot find '%s' function in WASM module\n", "init");
errored:
  goto finished;
success:
  success = rets[0].of.i32 != 0;
finished:
  return success;
}

bool SandboxCallView(Sandbox* box) {
  static const int kNumberOfArgs = 1;
  static const int kNumberOfReturns = 1;

  wasm_val_t args[kNumberOfArgs];

  bool success = false;
  wasm_val_t rets[kNumberOfReturns];
  rets[0].kind = WASM_I32;
  rets[0].of.i32 = (int32_t)success;

  if (!wasm_runtime_call_wasm_a(box->wasm_exec_env, box->init_func, 1, rets, 0, NULL)) {
    fprintf(stderr, "WASM execution failed: %s\n", wasm_runtime_get_exception(box->wasm_module_inst));
    goto errored;
  }

  goto success;
cannot_find_func:
  fprintf(stderr, "cannot find '%s' function in WASM module\n", "view");
errored:
  goto finished;
success:
  success = rets[0].of.i32 != 0;
finished:
  return success;
}

bool SandboxCallUpdate(Sandbox* box) {
  static const int kNumberOfArgs = 1;
  static const int kNumberOfReturns = 1;

  wasm_val_t args[kNumberOfArgs];

  bool success = false;
  wasm_val_t rets[kNumberOfReturns];
  rets[0].kind = WASM_I32;
  rets[0].of.i32 = (int32_t)success;

  if (!wasm_runtime_call_wasm_a(box->wasm_exec_env, box->init_func, 1, rets, 0, NULL)) {
    fprintf(stderr, "WASM execution failed: %s\n", wasm_runtime_get_exception(box->wasm_module_inst));
    goto errored;
  }

  goto success;
cannot_find_func:
  fprintf(stderr, "cannot find '%s' function in WASM module\n", "update");
errored:
  goto finished;
success:
  success = rets[0].of.i32 != 0;
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
