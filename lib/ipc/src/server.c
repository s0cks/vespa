#include <bson/bson_t.h>
#include <stdlib.h>
#include <uv.h>

#include "common.h"
#include "ipc.h"
#include "log.h"

#define CLIENT_HANDLE &client->handle

static inline void ClientInit(IpcServerClient* client, IpcServer* owner) {
  client->owner = owner;
  uv_pipe_init(owner->loop, &client->handle, 0);
  client->handle.data = client;
}

static inline void OnCloseClient(uv_handle_t* handle) {
  IpcServerClient* client = (IpcServerClient*)handle->data;
  IPC_FREE_FUNC(client);
}

static inline void CloseClient(IpcServerClient* client) {
  uv_close((uv_handle_t*)&client->handle, &OnCloseClient);
}

bool IpcServerRun(IpcServer* server, const uv_run_mode mode) {
  bool success = false;
  const int status = uv_run(server->loop, mode);
  if (status != UV_OK) {
    LOG_ERROR("failed to run server: %s", uv_strerror(status));
    goto finished;
  }

  success = true;
finished:
  return success;
}

static inline void OnWrite(uv_write_t* req, int status) {
  if (status < 0) {
    LOG_ERROR("failed to write: %s", uv_strerror(status));
    return;
  }

  write_req_t* wr = (write_req_t*)req->data;
  if (wr) {
    bson_destroy(&wr->doc);
    free(wr);
  }

#ifdef VESPA_DEBUG
  LOG_INFO("message sent:");
  PrintBsonAsJson(stdout, &wr->doc);
  fprintf(stdout, "\n");
#endif  // VESPA_DEBUG
}

static inline void OnRead(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf) {
  if (nread > 0) {
    Message* message = malloc(sizeof(Message));
    if (!ReadMessageFromBytes(message, (const uint8_t*)buf->base, nread)) {
      LOG_ERROR("failed to read valid message from client");
      exit(1);
    }

    IpcServerClient* c = (IpcServerClient*)client->data;
    IpcServer* server = c->owner;
    switch (message->kind) {
      case kPingKind: {
        bool success = true;
        if (server->OnPing)
          success = server->OnPing(c, message);

        if (!success) {
          LOG_ERROR("failed to respond to ping");
          exit(1);
        }
        break;
      }
      case kPongKind: {
        LOG_DEBUG("received pong");
        break;
      }
      default:
        LOG_ERROR("invalid message kind: %d", (int)message->kind);
    }
  } else if (nread < 0) {
    if (nread != UV_EOF)
      LOG_ERROR("read failure: %s", uv_err_name(nread));
    CloseClient((IpcServerClient*)client->data);
  }

  FreeBuffer(buf);
}

bool IpcServerClientWrite(IpcServerClient* client, Message* msg) {
  WriteMessageToStream(client, (uv_stream_t*)&client->handle, msg, &OnWrite);
  return true;
}

static inline void Accept(uv_stream_t* stream, IpcServerClient* client) {
  if (uv_accept(stream, (uv_stream_t*)CLIENT_HANDLE) == 0) {
    uv_read_start((uv_stream_t*)CLIENT_HANDLE, &AllocBuffer, &OnRead);
  } else {
    CloseClient(client);
  }
}

static inline void OnNewConnection(uv_stream_t* handle, int status) {
  if (status < 0)
    CHECK_UV(status, "new connection failed");

  IpcServerClient* client = (IpcServerClient*)malloc(sizeof(IpcServerClient));
  ClientInit(client, (IpcServer*)handle->data);
  Accept(handle, client);
}

#define SERVER_HANDLE &server->handle

void IpcServerInit(IpcServer* server, uv_loop_t* loop) {
  INIT_IPC_HANDLE(server, loop);
  CHECK_UV(uv_pipe_bind(SERVER_HANDLE, SOCKET_PATH), "failed to bind server");
  CHECK_UV(uv_listen((uv_stream_t*)SERVER_HANDLE, 128, &OnNewConnection), "server failed to listen");
  LOG_DEBUG("server listening at: %s", SOCKET_PATH);
}

void IpcServerFree(IpcServer* server) {
  FREE_IPC_HANDLE(server);
}
