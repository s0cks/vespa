#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>

#include "common.h"
#include "ipc.h"
#include "log.h"

bool IpcClientRun(IpcClient* client, const uv_run_mode mode) {
  bool success = false;

  LOG_DEBUG("running client loop");

  const int status = uv_run(client->loop, mode);
  if (status != UV_OK) {
    LOG_ERROR("failed to run client loop: %s", uv_strerror(status));
    goto finished;
  }

  success = true;
  LOG_DEBUG("client loop finished in %d", 0);
finished:
  return success;
}

void on_write(uv_write_t* req, int status) {
  if (status < 0) {
    LOG_ERROR("failed to write: %s", uv_strerror(status));
    return;
  }

  write_req_t* wr = (write_req_t*)req->data;
  if (wr)
    free(wr);
  LOG_DEBUG("message sent");
}

static inline void OnRead(uv_stream_t* handle, const ssize_t nread, const uv_buf_t* buf) {
  LOG_DEBUG("read %ld bytes", nread);
}

static inline void on_connect(uv_connect_t* req, int status) {
  if (status < 0) {
    LOG_ERROR("connect error: %s", uv_strerror(status));
    return;
  }

  IpcClient* client = (IpcClient*)req->data;
  CHECK_UV(uv_read_start(req->handle, &AllocBuffer, &OnRead), "failed to read");

  const char* topic = strdup("Hello World");

  Message m;
  InitEventMessage(&m, (uint8_t*)topic, strlen(topic));
  WriteMessageToStream(client, req->handle, &m, &on_write);
}

void IpcClientInit(IpcClient* client, uv_loop_t* loop) {
  INIT_IPC_HANDLE(client, loop);
  uv_pipe_connect(&client->connect_req, &client->handle, SOCKET_PATH, on_connect);
  client->connect_req.data = client;
}

void IpcClientFree(IpcClient* client) {
  FREE_IPC_HANDLE(client);
}
