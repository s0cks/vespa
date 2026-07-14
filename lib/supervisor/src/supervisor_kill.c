#include "log.h"
#include "supervisor.h"
#include "supervisor_def.h"

bool SupervisorKillHive(Supervisor* super, const int signal) {
  // TODO(@s0cks): check state
  LOG_DEBUG("killing hive process....");
  uv_process_kill(&super->hive.process, signal);
  SupervisorRunNoWait(super);
  return true;
}
