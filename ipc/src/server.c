#include <stdlib.h>

#include "flatcc/flatcc.h"
#include "ipc.h"
#include "protocol_builder.h"

int IpcServerRun(IpcServer* server) {
  return uv_run(server->loop, UV_RUN_DEFAULT);
}

static inline void AllocBuffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
  buf->base = malloc(suggested_size);
  buf->len = suggested_size;
}

static inline void OnRead(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf) {
  if (nread > 0) {
    // TODO(@s0cks): implement
  } else if (nread < 0) {
    if (nread != UV_EOF) {
      fprintf(stderr, "error: read failed: %s\n", uv_err_name(nread));
    }

    uv_close((uv_handle_t*)client, NULL);
  }

  if (buf->base)
    free(buf->base);
}

static inline void OnNewConnection(uv_stream_t* handle, int status) {
  if (status < 0) {
    fprintf(stderr, "error: new connection failed: %s\n", uv_strerror(status));
    return;
  }

  IpcServer* server = (IpcServer*)handle->data;

  uv_pipe_t* client = (uv_pipe_t*)malloc(sizeof(uv_pipe_t));
  uv_pipe_init(server->loop, client, 0);
  if (uv_accept(handle, (uv_stream_t*)client) == 0) {
    uv_read_start((uv_stream_t*)client, &AllocBuffer, &OnRead);
  } else {
    uv_close((uv_handle_t*)client, NULL);
  }
}

void IpcServerInit(IpcServer* server) {
  server->loop = uv_loop_new();
  {
    const int status = uv_pipe_bind(&server->handle, SOCKET_PATH);
    if (status) {
      fprintf(stderr, "error: failed to bind server: %s\n", uv_strerror(status));
      return;
    }
  }

  {
    const int status = uv_listen((uv_stream_t*)&server->handle, 128, &OnNewConnection);
    if (status) {
      fprintf(stderr, "error: failed to listen: %s\n", uv_strerror(status));
      return;
    }
  }
}

void IpcServerFree(IpcServer* server) {
  // TODO(@s0cks): implement
  uv_loop_delete(server->loop);
}
