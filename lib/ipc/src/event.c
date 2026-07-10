#include <bson/bson.h>

#include "common.h"
#include "ipc.h"
#include "log.h"

#define DEFINE_WRITE(Name) void Write##Name(Name* p, bson_t* B)

#define DEFINE_READ(Name)  void Read##Name(bson_t* B, Name* p)

DEFINE_WRITE(Event) {
  BSON_APPEND_BINARY(B, "topic", BSON_SUBTYPE_BINARY, p->topic, p->topic_len);
}

DEFINE_WRITE(Ping) {
  BSON_APPEND_BINARY(B, "digest", BSON_SUBTYPE_BINARY, p->digest, p->digest_len);
}

DEFINE_WRITE(Pong) {
  BSON_APPEND_BINARY(B, "digest", BSON_SUBTYPE_BINARY, p->digest, p->digest_len);
}

DEFINE_WRITE(Error) {
  BSON_APPEND_INT32(B, "code", p->code);
  BSON_APPEND_BINARY(B, "message", BSON_SUBTYPE_BINARY, p->message, p->message_len);
}

DEFINE_WRITE(Message) {
  BSON_APPEND_INT32(B, "kind", p->kind);
  BSON_APPEND_INT64(B, "timestamp", p->timestamp);
  switch (p->kind) {
#define DEFINE_WRITE_CASE(Name, Field)      \
  case k##Name##Kind: {                     \
    bson_t data = BSON_INITIALIZER;         \
    Write##Name(&p->Field, &data);          \
    BSON_APPEND_DOCUMENT(B, "data", &data); \
    bson_destroy(&data);                    \
    break;                                  \
  }

    DEFINE_WRITE_CASE(Ping, ping);
    DEFINE_WRITE_CASE(Pong, pong);
    DEFINE_WRITE_CASE(Event, event);
    DEFINE_WRITE_CASE(Error, error);

#undef DEFINE_WRITE_CASE
    default:
      fprintf(stderr, "error: invalid Message kind: %d\n", (int)p->kind);
      exit(1);
  }
}

#define _BEGIN_FIELD_ITERATOR(B, Name, KeyName, TypeName) \
  bson_iter_t Name;                                       \
  if (bson_iter_init(&Name, B)) {                         \
    while (bson_iter_next(&Name)) {                       \
      const char* key = bson_iter_key(&iter);             \
      bson_type_t type = bson_iter_type(&iter);

#define BEGIN_FIELD_ITERATOR(B, KeyName, TypeName) _BEGIN_FIELD_ITERATOR(B, iter, KeyName, TypeName)

#define END_FIELD_ITERATOR \
  }                        \
  }

DEFINE_READ(Ping) {
  BEGIN_FIELD_ITERATOR(B, key, type)

  if (strcmp(key, "digest") == 0) {
    uint32_t len = 0;
    const uint8_t* data = NULL;
    bson_subtype_t subtype;
    bson_iter_binary(&iter, &subtype, &len, &data);

    p->digest_len = len;
    p->digest = malloc(sizeof(uint8_t) * len);
    memcpy(p->digest, data, len);
  }

  END_FIELD_ITERATOR;
}

DEFINE_READ(Pong) {
  //   bson_iter_t iter;
  //   if (bson_iter_init(&iter, &b)) {
  //     while (bson_iter_next(&iter)) {
  //       const char* key = bson_iter_key(&iter);
  //       bson_type_t type = bson_iter_type(&iter);
  //
  //       printf(" - Found key: '%s' ", key);
  //
  //       // Check types and extract values
  //       if (type == BSON_TYPE_INT32) {
  //         printf("(INT32) = %d\n", bson_iter_int32(&iter));
  //       } else if (type == BSON_TYPE_UTF8) {
  //         printf("(STRING) = %s\n", bson_iter_utf8(&iter, NULL));
  //       } else if (type == BSON_TYPE_BOOL) {
  //         printf("(BOOL) = %s\n", bson_iter_bool(&iter) ? "true" : "false");
  //       } else {
  //         printf("(Other Type)\n");
  //       }
  //     }
  //   }
}

DEFINE_READ(Event) {
  //   bson_iter_t iter;
  //   if (bson_iter_init(&iter, &b)) {
  //     while (bson_iter_next(&iter)) {
  //       const char* key = bson_iter_key(&iter);
  //       bson_type_t type = bson_iter_type(&iter);
  //
  //       printf(" - Found key: '%s' ", key);
  //
  //       // Check types and extract values
  //       if (type == BSON_TYPE_INT32) {
  //         printf("(INT32) = %d\n", bson_iter_int32(&iter));
  //       } else if (type == BSON_TYPE_UTF8) {
  //         printf("(STRING) = %s\n", bson_iter_utf8(&iter, NULL));
  //       } else if (type == BSON_TYPE_BOOL) {
  //         printf("(BOOL) = %s\n", bson_iter_bool(&iter) ? "true" : "false");
  //       } else {
  //         printf("(Other Type)\n");
  //       }
  //     }
  //   }
}

DEFINE_READ(Error) {
  //   bson_iter_t iter;
  //   if (bson_iter_init(&iter, &b)) {
  //     while (bson_iter_next(&iter)) {
  //       const char* key = bson_iter_key(&iter);
  //       bson_type_t type = bson_iter_type(&iter);
  //
  //       printf(" - Found key: '%s' ", key);
  //
  //       // Check types and extract values
  //       if (type == BSON_TYPE_INT32) {
  //         printf("(INT32) = %d\n", bson_iter_int32(&iter));
  //       } else if (type == BSON_TYPE_UTF8) {
  //         printf("(STRING) = %s\n", bson_iter_utf8(&iter, NULL));
  //       } else if (type == BSON_TYPE_BOOL) {
  //         printf("(BOOL) = %s\n", bson_iter_bool(&iter) ? "true" : "false");
  //       } else {
  //         printf("(Other Type)\n");
  //       }
  //     }
  //   }
}

DEFINE_READ(Message) {
  BEGIN_FIELD_ITERATOR(B, key, type)
  if (strcmp(key, "kind") == 0) {
    p->kind = (MessageKind)bson_iter_int32(&iter);
  } else if (strcmp(key, "timestamp") == 0) {
    p->timestamp = bson_iter_int64(&iter);
  } else if (strcmp(key, "data") == 0) {
    bson_t doc;
    uint32_t doc_len = 0;
    const uint8_t* doc_data = NULL;
    bson_iter_document(&iter, &doc_len, &doc_data);
    if (bson_init_static(&doc, doc_data, doc_len)) {
      switch (p->kind) {
        case kPingKind: {
          ReadPing(&doc, &p->ping);
          continue;
        }
        case kPongKind: {
          ReadPong(&doc, &p->pong);
          continue;
        }
        case kEventKind: {
          ReadEvent(&doc, &p->event);
          continue;
        }
        case kErrorKind: {
          ReadError(&doc, &p->error);
          continue;
        }
        default:
          LOG_ERROR("invalid message kind: %d", (int)p->kind);
          return;
      }
      bson_destroy(&doc);
    }
  } else {
    LOG_ERROR("invalid message key %s", key);
  }

  END_FIELD_ITERATOR;
}
