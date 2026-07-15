#include "worker.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "sandbox.h"

bool WorkerRun(Worker* worker, const uv_run_mode mode) {
  const int status = uv_run(worker->loop, mode);
  if (status != 0)
    LOG_ERROR("failed to run worker loop: %s", uv_strerror(status));

finished:
  return status == 0;
}

Worker* NewWorker(uv_loop_t* loop, const char* filename) {
  Worker* result = malloc(sizeof(Worker));
  if (!result) {
    LOG_ERROR("failed to allocate new Worker");
    goto finished;
  }

  result->has_updates = true;
  result->loop = loop;
  result->sandbox = NewSandbox(loop, filename);
  if (!result->sandbox) {
    LOG_ERROR("failed to allocate Worker Sandbox");
    goto finished;
  }

finished:
  return result;
}

void WorkerFree(Worker* worker) {
  if (!worker)
    return;

  if (worker->sandbox)
    SandboxFree(worker->sandbox);
}
