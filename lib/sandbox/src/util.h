#ifndef VESPA_SANDBOX_UTIL_H
#define VESPA_SANDBOX_UTIL_H

#include "sandbox_def.h"

bool LoadWasmFile(const char* filename, uint8_t** bytes, uint64_t* nbytes);

#endif  // VESPA_SANDBOX_UTIL_H
