#ifndef VESPA_IPC_H
#define VESPA_IPC_H

#include <uv.h>

#define SOCKET_PATH "/tmp/ipc_test.sock"

typedef struct {
  uv_loop_t* loop;
  uv_pipe_t handle;
} IpcServer;

int IpcServerRun(IpcServer* server);
void IpcServerInit(IpcServer* server);
void IpcServerFree(IpcServer* server);

typedef struct {
  uv_loop_t* loop;
  uv_pipe_t handle;
  uv_connect_t connect_req;
} IpcClient;

int IpcClientRun(IpcClient* client);
void IpcClientInit(IpcClient* client);
void IpcClientFree(IpcClient* client);

#endif  // VESPA_IPC_H
