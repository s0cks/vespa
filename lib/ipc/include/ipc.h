#ifndef VESPA_IPC_H
#define VESPA_IPC_H

#include <bson/bson.h>
#include <stdio.h>
#include <stdlib.h>
#include <uv.h>

#define SOCKET_PATH "/tmp/ipc_test.sock"

#ifndef UV_OK
#define UV_OK 0
#endif  // UV_OK

#ifndef IPC_ALLOC_FUNC
#define IPC_ALLOC_FUNC malloc
#endif  // IPC_ALLOC_FUNC

#ifndef IPC_REALLOC_FUNC
#define IPC_REALLOC_FUNC realloc
#endif  // IPC_REALLOC_FUNC

#ifndef IPC_FREE_FUNC
#define IPC_FREE_FUNC free
#endif  // IPC_FREE_FUNC

#define CHECK_UV(Status, Message)                                         \
  ({                                                                      \
    const int status = (Status);                                          \
    if (status != UV_OK) {                                                \
      fprintf(stderr, "error: %s: %s\n", (Message), uv_strerror(status)); \
      return;                                                             \
    }                                                                     \
  })

#define FOR_EACH_MESSAGE_TYPE(V) \
  V(Event)                       \
  V(Error)                       \
  V(Ping)                        \
  V(Pong)

#define DECLARE_MESSAGE(Name) typedef struct _##Name Name;
FOR_EACH_MESSAGE_TYPE(DECLARE_MESSAGE)
#undef DECLARE_MESSAGE

// clang-format off
typedef enum {
  kInvalidMessage = 0,
#define DEFINE_KIND(Name) k##Name##Kind,
  FOR_EACH_MESSAGE_TYPE(DEFINE_KIND)
#undef DEFINE_KIND
  kTotalNumberOfMessageKinds,
} MessageKind;
// clang-format on

#define DECLARE_ARRAY_FIELD(Name) \
  uint8_t* Name;                  \
  uint64_t Name##_len;

struct _Event {
  DECLARE_ARRAY_FIELD(topic);
};

struct _Error {
  uint64_t code;
  DECLARE_ARRAY_FIELD(message);
};

#define DECLARE_PINGPONG_FIELDS DECLARE_ARRAY_FIELD(digest);

struct _Ping {
  DECLARE_PINGPONG_FIELDS;
};

struct _Pong {
  DECLARE_PINGPONG_FIELDS;
};

#undef DECLARE_PINGPONG_FIELDS
#undef DECALRE_ARRAY_FIELD

typedef struct {
  MessageKind kind;
  uint64_t timestamp;
  union {
    Event event;
    Error error;
    Ping ping;
    Pong pong;
  };
} Message;

#define DECLARE_WRITE_PAYLOAD(Name) void Write##Name(Name* p, bson_t* B);
FOR_EACH_MESSAGE_TYPE(DECLARE_WRITE_PAYLOAD)
#undef DECLARE_WRITE_PAYLOAD

#define DECLARE_READ_PAYLOAD(Name) void Read##Name(bson_t* B, Name* p);
FOR_EACH_MESSAGE_TYPE(DECLARE_READ_PAYLOAD)
#undef DECLARE_READ_PAYLOAD

static inline void InitEventMessage(Message* m, uint8_t* topic, const uint64_t topic_len) {
  m->kind = kEventKind;
  m->timestamp = 1337;
  m->event.topic = topic;
  m->event.topic_len = topic_len;
}

static inline void InitPingMessage(Message* m, uint8_t* digest, const uint64_t digest_len) {
  m->kind = kPingKind;
  m->timestamp = 12937;
  m->ping.digest = digest;
  m->ping.digest_len = digest_len;
}

static inline void InitPongMessage(Message* m, uint8_t* digest, const uint64_t digest_len) {
  m->kind = kPongKind;
  m->timestamp = 12937;
  m->ping.digest = digest;
  m->ping.digest_len = digest_len;
}

void WriteMessage(Message* p, bson_t* B);
void ReadMessage(bson_t* B, Message* p);

#define DEFINE_IPC_HANDLE_FIELDS \
  uv_loop_t* loop;               \
  uv_pipe_t handle;

typedef struct {
  DEFINE_IPC_HANDLE_FIELDS;
} IpcServer;

bool IpcServerRun(IpcServer* server, const uv_run_mode mode);
void IpcServerInit(IpcServer* server, uv_loop_t* loop);
void IpcServerFree(IpcServer* server);

#define DEFINE_RUN_MODE(Name, Mode)                          \
  static inline bool IpcServerRun##Name(IpcServer* client) { \
    return IpcServerRun(client, (Mode));                     \
  }
DEFINE_RUN_MODE(Default, UV_RUN_DEFAULT);
DEFINE_RUN_MODE(NoWait, UV_RUN_NOWAIT);
DEFINE_RUN_MODE(Once, UV_RUN_ONCE);
#undef DEFINE_RUN_MODE

typedef struct {
  DEFINE_IPC_HANDLE_FIELDS;
  uv_connect_t connect_req;
} IpcClient;

bool IpcClientRun(IpcClient* client, const uv_run_mode mode);
void IpcClientInit(IpcClient* client, uv_loop_t* loop);
void IpcClientFree(IpcClient* client);

#define DEFINE_RUN_MODE(Name, Mode)                          \
  static inline bool IpcClientRun##Name(IpcClient* client) { \
    return IpcClientRun(client, (Mode));                     \
  }
DEFINE_RUN_MODE(Default, UV_RUN_DEFAULT);
DEFINE_RUN_MODE(NoWait, UV_RUN_NOWAIT);
DEFINE_RUN_MODE(Once, UV_RUN_ONCE);
#undef DEFINE_RUN_MODE

#endif  // VESPA_IPC_H
