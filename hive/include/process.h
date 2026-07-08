#ifndef VESPA_PROCESS_H
#define VESPA_PROCESS_H

#include <uv.h>

typedef struct {
  uv_process_t handle;
  uv_process_options_t options;

  int argc;
  char** argv;
} Process;

#endif  // VESPA_PROCESS_H
