#ifndef VESPA_SANDBOX_H
#define VESPA_SANDBOX_H

#include <stdbool.h>
#include <uv.h>

typedef struct _Sandbox Sandbox;

Sandbox* NewSandbox(uv_loop_t* loop);
bool SandboxLoadWasm(Sandbox* box, const char* filename);
bool SandboxCallFunc(Sandbox* box, const char* name);
void SandboxFree(Sandbox* box);

#endif  // VESPA_SANDBOX_H
