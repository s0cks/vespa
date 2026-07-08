#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wasm_export.h"

int main(int argc, char** argv) {
  const char* filename = NULL;
  if (argc < 2 || argv[1] == NULL) {
    fprintf(stderr, "error: expected filename argument\n");
    return EXIT_FAILURE;
  }
  filename = argv[1];

  char error_buf[128];
  uint8_t* wasm_file_buf = NULL;
  uint32_t wasm_file_size = 0;

  // 1. Initialize the WAMR runtime
  RuntimeInitArgs init_args;
  memset(&init_args, 0, sizeof(RuntimeInitArgs));
  init_args.mem_alloc_type = Alloc_With_Allocator;
  init_args.mem_alloc_option.allocator.malloc_func = malloc;
  init_args.mem_alloc_option.allocator.realloc_func = realloc;
  init_args.mem_alloc_option.allocator.free_func = free;

  if (!wasm_runtime_full_init(&init_args)) {
    printf("error: failed to initialize WAMR runtime.\n");
    return -1;
  }

  // 2. Read math.wasm into a memory buffer
  FILE* file = fopen(filename, "rb");
  if (!file) {
    fprintf(stderr, "error: failed to open %s\n", filename);
    goto fail1;
  }
  fseek(file, 0, SEEK_END);
  wasm_file_size = (uint32_t)ftell(file);
  fseek(file, 0, SEEK_SET);

  wasm_file_buf = malloc(wasm_file_size);
  if (!wasm_file_buf || fread(wasm_file_buf, 1, wasm_file_size, file) != wasm_file_size) {
    fprintf(stderr, "error: failed to read WASM file contents.\n");
    fclose(file);
    goto fail2;
  }
  fclose(file);

  // 3. Load the WASM module
  wasm_module_t wasm_module = wasm_runtime_load(wasm_file_buf, wasm_file_size, error_buf, sizeof(error_buf));
  if (!wasm_module) {
    fprintf(stderr, "error: WASM module failed: %s\n", error_buf);
    goto fail2;
  }

  // 4. Instantiate the module (no extra heap/stack needed for basic pure math)
  wasm_module_inst_t wasm_module_inst = wasm_runtime_instantiate(wasm_module, 8192, 8192, error_buf, sizeof(error_buf));
  if (!wasm_module_inst) {
    fprintf(stderr, "error: init WASM module failed: %s\n", error_buf);
    goto fail3;
  }

  // 5. Create the execution environment
  wasm_exec_env_t exec_env = wasm_runtime_create_exec_env(wasm_module_inst, 8192);
  if (!exec_env) {
    fprintf(stderr, "error: create execution environment failed.\n");
    goto fail4;
  }

  // 6. Look up your exported function from Clang ("add")
  wasm_function_inst_t func = wasm_runtime_lookup_function(wasm_module_inst, "add");
  if (!func) {
    fprintf(stderr, "error: cannot find 'add' function in WASM module.\n");
    goto fail5;
  }

  // 7. Set up arguments and invoke the function
  // WAMR packs parameters and return values into a single uint32 array.
  // Ensure the array size matches whichever is larger: total args or total returns.
  uint32_t args[2];
  args[0] = 15;  // First parameter (a)
  args[1] = 27;  // Second parameter (b)

  printf("Calling 'add' inside WebAssembly with arguments: %d, %d\n", args[0], args[1]);

  // Pass 2 arguments to the target function
  if (!wasm_runtime_call_wasm(exec_env, func, 2, args)) {
    fprintf(stderr, "errror: WASM execution failed: %s\n", wasm_runtime_get_exception(wasm_module_inst));
  } else {
    // WAMR overwrites the start of the 'args' array with the return value(s)
    int result = (int)args[0];
    printf("Result returned from WASM execution: %d\n", result);
  }

  // 8. Cleanup resources
fail5:
  wasm_runtime_destroy_exec_env(exec_env);
fail4:
  wasm_runtime_deinstantiate(wasm_module_inst);
fail3:
  wasm_runtime_unload(wasm_module);
fail2:
  free(wasm_file_buf);
fail1:
  wasm_runtime_destroy();
  return 0;
}
