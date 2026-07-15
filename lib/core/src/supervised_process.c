#include <stdlib.h>
#include <uv.h>

#include "log.h"
#include "supervised_process_def.h"

SupervisedProcess* NewSupervisedProcess(uv_loop_t* loop) {
  SupervisedProcess* proc = malloc(sizeof(SupervisedProcess));
  if (proc) {
    CHECK_UV(UV_OK, "uv_spawn failed", "");
    return NULL;
  }
  return proc;
}
