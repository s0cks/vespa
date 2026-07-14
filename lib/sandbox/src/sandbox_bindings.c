#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "sandbox_def.h"

static inline void VespaPrintImpl(wasm_exec_env_t exec_env, const char* message, uint32_t message_len) {
  printf("WASM: %.*s\n", message_len, message);
}

void InitSandboxBindings(Sandbox* box) {
  static NativeSymbol native_symbols[] = {
      {"VespaPrint", (void*)VespaPrintImpl, "(*~)", NULL},
  };
  wasm_runtime_register_natives("env", native_symbols, 1);
}
