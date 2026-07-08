#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "worker.h"

static inline bool LoadWasmFile(const char* filename, uint8_t** data, uint32_t* size) {
  bool result = false;

  FILE* file = fopen(filename, "rb");
  if (!file) {
    fprintf(stderr, "error: failed to open %s\n", filename);
    goto finished;
  }
  fseek(file, 0, SEEK_END);
  uint32_t total_size = (uint32_t)ftell(file);
  fseek(file, 0, SEEK_SET);

  uint8_t* bytes = (uint8_t*)malloc(sizeof(uint8_t) * total_size);
  if (!bytes || fread(bytes, sizeof(uint8_t), total_size, file) != total_size) {
    fprintf(stderr, "error: failed to read WASM file contents.\n");
    goto close_and_finish;
  }

  (*data) = bytes;
  (*size) = total_size;
  result = true;
close_and_finish:
  fclose(file);
finished:
  return result;
}

static inline bool add(wasm_exec_env_t* exec_env, wasm_module_inst_t* wasm_module_inst, const uint32_t a,
                       const uint32_t b, uint32_t* result) {
  bool success = false;
  uint32_t args[2];
  args[0] = a;
  args[1] = b;

  wasm_function_inst_t func = wasm_runtime_lookup_function(*wasm_module_inst, "add");
  if (func) {
    if (!wasm_runtime_call_wasm(*exec_env, func, 2, args)) {
      fprintf(stderr, "errror: WASM execution failed: %s\n", wasm_runtime_get_exception(*wasm_module_inst));
      goto errored;
    }

    goto success;
  }

cannot_find_func:
  fprintf(stderr, "error: cannot find 'add' function in WASM module.\n");
errored:
  (*result) = 0;
  goto finished;
success:
  (*result) = args[0];
  success = true;
finished:
  return success;
}

int main(int argc, char** argv) {
  int result = EXIT_FAILURE;
  uint8_t* wasm_file_buf = NULL;
  uint32_t wasm_file_size = 0;
  const char* filename = NULL;
  if (argc < 2 || argv[1] == NULL) {
    fprintf(stderr, "error: expected filename argument\n");
    return EXIT_FAILURE;
  }
  filename = argv[1];
  if (!LoadWasmFile(filename, &wasm_file_buf, &wasm_file_size))
    goto failed;

  Worker worker;
  if (!WorkerInit(&worker, wasm_file_buf, wasm_file_size)) {
    fprintf(stderr, "error: failed to initialize worker\n");
    result = EXIT_FAILURE;
    goto failed;
  }

  uint32_t value = 0;
  if (!add(&worker.wasm_exec_env, &worker.wasm_module_inst, 17, 15, &value)) {
    fprintf(stderr, "error: failed to add numbers\n");
    goto failed;
  }

success:
  result = EXIT_SUCCESS;
  fprintf(stdout, "result: %d\n", (int)value);
  goto finished;
failed:
  result = EXIT_FAILURE;
finished:
  if (wasm_file_buf)
    free(wasm_file_buf);
  WorkerFree(&worker);
  return result;
}
