#include "log.h"
#include "supervisor.h"

bool SupervisorKillHive(Supervisor* sp, const int signal) {
  // TODO(@s0cks): check state
  LOG_DEBUG("killing hive process....");
  uv_process_kill(&sp->hive.process, signal);
  SupervisorRunNoWait(sp);
  return true;
}
