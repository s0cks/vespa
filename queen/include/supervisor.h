#ifndef VESPA_SUPERVISOR_H
#define VESPA_SUPERVISOR_H

#include <uv.h>

typedef struct {
  uv_loop_t* loop;
} Supervisor;

bool SupervisorInit(Supervisor*);
void SupervisorFree(Supervisor*);
int SupervisorRun(Supervisor* supervisor, const int mode);

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
