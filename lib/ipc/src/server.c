#include <bson/bson_t.h>
#include <stdlib.h>
#include <uv.h>

#include "common.h"
#include "ipc.h"

#define CLIENT_HANDLE &client->handle

typedef struct {
  uv_pipe_t handle;
  IpcServer* owner;
} Client;

static inline void ClientInit(Client* client, IpcServer* owner) {
  uv_pipe_init(owner->loop, &client->handle, 0);
  client->handle.data = client;
}

static inline void OnCloseClient(uv_handle_t* handle) {
  Client* client = (Client*)handle->data;
  IPC_FREE_FUNC(client);
}

static inline void CloseClient(Client* client) {
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

static inline void on_write(uv_write_t* req, int status) {
  if (status < 0) {
    LOG_ERROR("failed to write: %s", uv_strerror(status));
    return;
  }

  write_req_t* wr = (write_req_t*)req->data;
  if (wr)
    free(wr);
  LOG_DEBUG("message sent");
}

static inline void OnRead(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf) {
  if (nread > 0) {
    Message message;
    ReadMessageFromBytes(&message, (const uint8_t*)buf->base, nread);

    Client* c = (Client*)client->data;

    switch (message.kind) {
      case kPingKind: {
        LOG_DEBUG("received ping");
        char* digest = malloc(sizeof(char) * message.ping.digest_len + 1);
        memcpy(digest, message.ping.digest, message.ping.digest_len);
        digest[message.ping.digest_len] = '\0';
        LOG_INFO("ping: %s\n", (const char*)digest);

        Message pong;
        InitPongMessage(&pong, (uint8_t*)digest, message.ping.digest_len);
        WriteMessageToStream(c, client, &pong, &on_write);
        free(digest);
        break;
      }
      case kPongKind: {
        LOG_DEBUG("received pong");
        break;
      }
      default:
        LOG_ERROR("invalid message kind: %d", (int)message.kind);
    }
  } else if (nread < 0) {
    if (nread != UV_EOF)
      LOG_ERROR("read failure: %s", uv_err_name(nread));
    CloseClient((Client*)client->data);
  }

  FreeBuffer(buf);
}

static inline void IpcServerAccept(uv_stream_t* stream, Client* client) {
  if (uv_accept(stream, (uv_stream_t*)CLIENT_HANDLE) == 0) {
    uv_read_start((uv_stream_t*)CLIENT_HANDLE, &AllocBuffer, &OnRead);
  } else {
    uv_close((uv_handle_t*)CLIENT_HANDLE, NULL);
  }
}

static inline void OnNewConnection(uv_stream_t* handle, int status) {
  if (status < 0)
    CHECK_UV(status, "new connection failed");
  IpcServer* server = (IpcServer*)handle->data;
  Client* client = (Client*)malloc(sizeof(Client));
  ClientInit(client, server);
  IpcServerAccept(handle, client);
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
