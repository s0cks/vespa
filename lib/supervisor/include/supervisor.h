#ifndef VESPA_SUPERVISOR_H
#define VESPA_SUPERVISOR_H

#include <stdbool.h>
#include <uv.h>

typedef struct {
  char* hive_bin;
} SupervisorConfig;

typedef struct _SupervisedProcess SupervisedProcess;
typedef struct _Supervisor Supervisor;

Supervisor* NewSupervisor(SupervisorConfig*);
bool SupervisorRun(Supervisor* supervisor, const int mode);
void SupervisorFree(Supervisor*);

SupervisedProcess* GetHiveProcess(Supervisor* super);
bool SupervisorKillHive(Supervisor* sp, const int signal);
bool SupervisorSpawnHive(Supervisor* sp);

static inline bool SupervisorRunDefault(Supervisor* sp) {
  return SupervisorRun(sp, UV_RUN_DEFAULT);
}

static inline bool SupervisorRunNoWait(Supervisor* sp) {
  return SupervisorRun(sp, UV_RUN_NOWAIT);
}

static inline bool SupervisorRunOnce(Supervisor* sp) {
  return SupervisorRun(sp, UV_RUN_ONCE);
}

#endif  // VESPA_SUPERVISOR_H
