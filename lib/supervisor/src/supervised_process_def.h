#ifndef VESPA_SUPERVISOR_PROCESS_DEF_H
#define VESPA_SUPERVISOR_PROCESS_DEF_H

#include "supervisor_def.h"

#define FOR_EACH_SUPERVISOR_PROCESS_STATE(V) \
  V(Starting)                                \
  V(Running)                                 \
  V(Stopping)                                \
  V(Stopped)

// clang-format off
typedef enum {
#define DEFINE_STATE(Name) kSupervisorProcess##Name##State,

  FOR_EACH_SUPERVISOR_PROCESS_STATE(DEFINE_STATE)
#undef DEFINE_STATE
  kTotalNumberOfSupervisorProcessStates,
} SupervisorProcessState;
// clang-format on

typedef struct {
  uv_process_t process;
  uv_process_options_t options;
  SupervisorProcessState state;

  char* name;
  uint64_t num_restarts;
} SupervisorProcess;

bool SupervisorProcessInit(SupervisorProcess* proc);

#define DEFINE_TYPE_CHECK(Name)                                           \
  static inline bool IsSupervisorProcess##Name(SupervisorProcess* proc) { \
    return proc && (proc->state == kSupervisorProcess##Name##State);      \
  }

FOR_EACH_SUPERVISOR_PROCESS_STATE(DEFINE_TYPE_CHECK)
#undef DEFINE_TYPE_CHECK

#endif  // VESPA_SUPERVISOR_PROCESS_DEF_H
