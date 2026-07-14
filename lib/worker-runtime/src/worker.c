#include "worker.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool WorkerLoadWasm(Worker* worker, const char* filename) {
  if (!worker || !worker->sandbox)
    return false;
  return SandboxLoadWasm(worker->sandbox, filename);
}

bool WorkerRun(Worker* worker, const uv_run_mode mode) {
  const int status = uv_run(worker->loop, mode);
  if (status != 0) {
    fprintf(stderr, "failed to run worker loop: %s\n", uv_strerror(status));
    goto finished;
  }

finished:
  return status == 0;
}

#define VESPA_WIDGET_INIT_FUNC_NAME   "init"
#define VESPA_WIDGET_VIEW_FUNC_NAME   "view"
#define VESPA_WIDGET_UPDATE_FUNC_NAME "update"

bool WorkerWidgetInit(Worker* worker) {
  if (!worker || !worker->sandbox)
    return false;
  return SandboxCallFunc(worker->sandbox, VESPA_WIDGET_INIT_FUNC_NAME);
}

bool WorkerWidgetView(Worker* worker) {
  if (!worker || !worker->sandbox)
    return false;
  return SandboxCallFunc(worker->sandbox, VESPA_WIDGET_VIEW_FUNC_NAME);
}

bool WorkerWidgetUpdate(Worker* worker) {
  if (!worker || !worker->sandbox)
    return false;
  return SandboxCallFunc(worker->sandbox, VESPA_WIDGET_UPDATE_FUNC_NAME);
}

bool WorkerInit(Worker* worker, uv_loop_t* loop) {
  bool success = false;
  if (!loop) {
    fprintf(stderr, "uv_loop_new() failed\n");
    goto finished;
  }

  worker->has_updates = true;
  worker->loop = loop;
  worker->sandbox = NewSandbox(loop);
  if (!worker->sandbox) {
    fprintf(stderr, "failed to allocate sandbox");
    goto finished;
  }

  success = true;
finished:
  return success;
}

void WorkerFree(Worker* worker) {
  if (!worker)
    return;

  if (worker->sandbox)
    SandboxFree(worker->sandbox);
}
