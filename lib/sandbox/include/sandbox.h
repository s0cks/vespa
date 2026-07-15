#ifndef VESPA_SANDBOX_H
#define VESPA_SANDBOX_H

#include <stdbool.h>
#include <uv.h>

typedef struct _SandboxModule SandboxModule;
typedef struct _Sandbox Sandbox;

Sandbox* NewSandbox(uv_loop_t* loop, const char* filename);
bool SandboxCallFunc(Sandbox* box, const char* name);
bool SandboxCallInit(Sandbox*);
bool SandboxCallView(Sandbox*);
bool SandboxCallUpdate(Sandbox*);
void SandboxFree(Sandbox* box);

#endif  // VESPA_SANDBOX_H
