#include "util.h"

#include <stdlib.h>

bool LoadWasmFile(const char* filename, uint8_t** bytes, uint64_t* nbytes) {
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
