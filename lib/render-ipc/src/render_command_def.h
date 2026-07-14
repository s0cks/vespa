#ifndef VESPA_RENDER_COMMAND_DEF_H
#define VESPA_RENDER_COMMAND_DEF_H

#include <stdint.h>

#include "render_command.h"

struct _SharedRenderCommandArray {
  int shm;
  char* name;
  uint64_t start;
  uint64_t size;
  uint64_t current;
};

#endif  // VESPA_RENDER_COMMAND_DEF_H
