#ifndef VESPA_SUPERVISOR_DEF_H
#define VESPA_SUPERVISOR_DEF_H

#include "supervisor.h"

struct _Supervisor {
  uv_loop_t* loop;
  SupervisorConfig config;

  SupervisedProcess** processes_;
};

#endif  // VESPA_SUPERVISOR_DEF_H
