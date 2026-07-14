#include "render_command_def.h"

#define DEFINE_GET_ADDRESS(Name, Value)                                                 \
  uint64_t SharedRenderCommandArrayGet##Name##Address(SharedRenderCommandArray* sarr) { \
    if (!sarr)                                                                          \
      return 0;                                                                         \
    return (Value);                                                                     \
  }

DEFINE_GET_ADDRESS(Starting, sarr->start);
DEFINE_GET_ADDRESS(Current, sarr->current);
DEFINE_GET_ADDRESS(Ending, sarr->start + sarr->size);
#undef DEFINE_GET_ADDRESS
