#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>

#include "ipc.h"

typedef struct {
  uv_write_t req;
  uv_buf_t buf;
} write_req_t;

int IpcClientRun(IpcClient* client) {
  return uv_run(client->loop, UV_RUN_DEFAULT);
}

void on_write(uv_write_t* req, int status) {
  if (status < 0) {
    fprintf(stderr, "Write error: %s\n", uv_strerror(status));
    return;
  }

  write_req_t* wr = (write_req_t*)req;
  free(wr->buf.base);  // Free the flatbuffers serialized buffer
  free(wr);
  printf("Message sent successfully.\n");
}

void on_connect(uv_connect_t* req, int status) {
  if (status < 0) {
    fprintf(stderr, "Connect error: %s\n", uv_strerror(status));
    return;
  }

  IpcClient* client = (IpcClient*)req->data;

  // flatcc_builder_t builder;
  // flatcc_builder_init(&builder);
  //
  // MyIPC_Message_start_as_root(&builder);
  // MyIPC_Message_id_add(&builder, 42);
  // MyIPC_Message_text_create_str(&builder, "Hello from libuv Client!");
  // MyIPC_Message_end_as_root(&builder);
  //
  // size_t size;
  // void* buffer = flatcc_builder_finalize_buffer(&builder, &size);
  //
  // write_req_t* wr = malloc(sizeof(write_req_t));
  // wr->buf = uv_buf_init((char*)buffer, size);
  //
  // uv_write((uv_write_t*)wr, stream, &wr->buf, 1, on_write);
  //
  // flatcc_builder_clear(&builder);
}

void IpcClientInit(IpcClient* client) {
  client->loop = uv_loop_new();
  uv_pipe_init(client->loop, &client->handle, 0);
  client->handle.data = client;
  uv_pipe_connect(&client->connect_req, &client->handle, SOCKET_PATH, on_connect);
}

void IpcClientFree(IpcClient* client) {
  // TODO(@s0cks): implement
  if (client->loop)
    uv_loop_delete(client->loop);
}
