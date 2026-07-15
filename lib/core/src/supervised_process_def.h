#ifndef VESPA_SUPERVISED_PROCESS_DEF_H
#define VESPA_SUPERVISED_PROCESS_DEF_H

#include "supervised_process.h"

struct _SupervisedProcess {
  uv_process_t process;
  uv_process_options_t options;
  SupervisorProcessState state;

  char* name;
  uint64_t num_restarts;
};

#endif  // VESPA_SUPERVISED_PROCESS_DEF_H
