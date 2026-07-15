#ifndef SUPERVISED_PROCESS_H
#define SUPERVISED_PROCESS_H

#include <uv.h>

#ifndef VESPA_MAX_NUMBER_OF_SUPERVISED_PROCESSES
#define VESPA_MAX_NUMBER_OF_SUPERVISED_PROCESSES 128
#endif  // VESPA_MAX_NUMBER_OF_SUPERVISED_PROCESSES

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

typedef struct _SupervisedProcess SupervisedProcess;

SupervisedProcess* NewSupervisorProcess(uv_loop_t* loop);
bool IsSupervisedProcessInState(SupervisedProcess* proc, const SupervisorProcessState state);

#define DEFINE_TYPE_CHECK(Name)                                               \
  static inline bool IsSupervisorProcess##Name(SupervisedProcess* proc) {     \
    return IsSupervisedProcessInState(proc, kSupervisorProcess##Name##State); \
  }

FOR_EACH_SUPERVISOR_PROCESS_STATE(DEFINE_TYPE_CHECK)
#undef DEFINE_TYPE_CHECK

#endif  // SUPERVISED_PROCESS_H
