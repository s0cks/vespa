#ifndef VESPA_RENDER_COMMAND_H
#define VESPA_RENDER_COMMAND_H

#include <stdint.h>

#ifdef __linux__

#include <sys/mman.h>

#else
#error "unsupported operating system"
#endif  // __linux__

typedef enum {
  kNone = 0,
#ifdef __linux__

  kReadOnly = PROT_READ,
  kWriteOnly = PROT_WRITE,
  kReadWrite = PROT_WRITE | PROT_READ,
  kExecute = PROT_EXEC,

#else
#error "unsupported operating system"
#endif  // __linux__

  kTotalNumberOfAccessModes,
} SharedRenderCommandArrayAccess;

#define MAX_NUMBER_OF_RENDER_COMMANDS 256

typedef struct {
  int id;
  double score;
  char name[32];
} RenderCommand;

typedef struct _SharedRenderCommandArray SharedRenderCommandArray;

SharedRenderCommandArray* NewSharedRenderCommandArray(const char* name, const uint64_t name_length,
                                                      const SharedRenderCommandArrayAccess access);
bool SharedRenderCommandArrayProtect(SharedRenderCommandArray* sarr, const SharedRenderCommandArrayAccess access);

static inline bool SharedRenderCommandArraySetReadOnly(SharedRenderCommandArray* sarr) {
  return SharedRenderCommandArrayProtect(sarr, kReadOnly);
}

static inline bool SharedRenderCommandArraySetWriteOnly(SharedRenderCommandArray* sarr) {
  return SharedRenderCommandArrayProtect(sarr, kWriteOnly);
}

static inline bool SharedRenderCommandArraySetReadWrite(SharedRenderCommandArray* sarr) {
  return SharedRenderCommandArrayProtect(sarr, kReadWrite);
}

static inline bool SharedRenderCommandArraySetExecutable(SharedRenderCommandArray* sarr) {
  return SharedRenderCommandArrayProtect(sarr, kExecute);
}

#define DEFINE_GET_ADDRESS_AND_POINTER(Name)                                                       \
  uint64_t SharedRenderCommandArrayGet##Name##Address(SharedRenderCommandArray* sarr);             \
  static inline void* SharedRenderCommandArrayGet##Name##Pointer(SharedRenderCommandArray* sarr) { \
    return (void*)SharedRenderCommandArrayGet##Name##Address(sarr);                                \
  }

DEFINE_GET_ADDRESS_AND_POINTER(Starting);
DEFINE_GET_ADDRESS_AND_POINTER(Current);
DEFINE_GET_ADDRESS_AND_POINTER(Ending);
#undef DEFINE_GET_ADDRESS_AND_POINTER

void SharedRenderCommandArrayFree(SharedRenderCommandArray* sarr);

#endif  // VESPA_RENDER_COMMAND_H
