#ifndef VESPA_IPC_COMMON_H
#define VESPA_IPC_COMMON_H

#include <stdio.h>

#include "ipc.h"
#include "log.h"

static inline void PrintBsonAsJson(FILE* stream, bson_t* B) {
  uint64_t len = 0;
  char* json_string = bson_as_canonical_extended_json(B, &len);
  if (json_string) {
    fprintf(stream, "%s", json_string);
    bson_free(json_string);
  }
}

#define INIT_IPC_HANDLE(Handle, Loop)             \
  Handle->loop = (Loop);                          \
  uv_pipe_init(Handle->loop, &Handle->handle, 0); \
  Handle->handle.data = (Handle);

#define FREE_IPC_HANDLE(Handle) \
  if (Handle->loop)             \
    uv_loop_delete(Handle->loop);

typedef struct {
  uv_write_t req;
  uv_buf_t buf;
  bson_t doc;
} write_req_t;

static inline void AllocBuffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
  buf->base = IPC_ALLOC_FUNC(suggested_size);
  buf->len = suggested_size;
}

static inline void FreeBuffer(const uv_buf_t* buf) {
  if (buf && buf->base)
    IPC_FREE_FUNC(buf->base);
}

static inline bool ReadMessageFromBytes(Message* message, const uint8_t* bytes, const uint64_t nbytes) {
  bson_t message_doc;
  if (!bson_init_static(&message_doc, bytes, nbytes)) {
    LOG_ERROR("failed to initialize bson doc");
    return false;
  }

  if (!bson_validate(&message_doc, BSON_VALIDATE_NONE, NULL)) {
    LOG_ERROR("message is not valid");
    return false;
  }

  ReadMessage(&message_doc, message);
  return true;
}

static inline void WriteMessageToStream(void* data, uv_stream_t* stream, Message* m, uv_write_cb on_write) {
  write_req_t* wr = malloc(sizeof(write_req_t));
  bson_init(&wr->doc);
  WriteMessage(m, &wr->doc);
  wr->req.data = wr;
  wr->buf = uv_buf_init((char*)bson_get_data(&wr->doc), wr->doc.len);
  uv_write((uv_write_t*)&wr->req, stream, &wr->buf, 1, on_write);
}

#endif  // VESPA_IPC_COMMON_H
