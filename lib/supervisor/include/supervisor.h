#ifndef VESPA_SUPERVISOR_H
#define VESPA_SUPERVISOR_H

#include <stdbool.h>
#include <uv.h>

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

typedef struct {
  char* hive_bin;
} SupervisorConfig;

typedef struct {
  uv_loop_t* loop;
  SupervisorConfig config;

  SupervisorProcess hive;
} Supervisor;

bool SupervisorRun(Supervisor* supervisor, const int mode);
bool SupervisorInit(Supervisor*, SupervisorConfig*);
bool SupervisorKillHive(Supervisor* sp, const int signal);
bool SupervisorSpawnHive(Supervisor* sp, SupervisorProcess* proc);
void SupervisorFree(Supervisor*);

static inline int SupervisorRunDefault(Supervisor* sp) {
  return SupervisorRun(sp, UV_RUN_DEFAULT);
}

static inline int SupervisorRunNoWait(Supervisor* sp) {
  return SupervisorRun(sp, UV_RUN_NOWAIT);
}

static inline int SupervisorRunOnce(Supervisor* sp) {
  return SupervisorRun(sp, UV_RUN_ONCE);
}

#endif  // VESPA_SUPERVISOR_H
