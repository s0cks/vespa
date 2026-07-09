#include "supervisor.h"

int SupervisorRun(Supervisor* sp, const int mode) {
#ifdef VESPA_DEBUG
  fprintf(stdout, "running supervisor...\n");
#endif  // VESPA_DEBUG
  return uv_run(sp->loop, mode);
}

bool SupervisorInit(Supervisor* sp) {
  uv_loop_t* loop = uv_loop_new();
  if (!loop)
    return false;
  sp->loop = loop;
  return true;
}

void SupervisorFree(Supervisor* sp) {
  if (!sp)
    return;
  uv_loop_delete(sp->loop);
}
