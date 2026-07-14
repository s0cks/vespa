#include "render_command.h"
#ifdef __linux__

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "log.h"
#include "render_command_def.h"

static inline uint64_t CalcTotalShmSize() {
  return sizeof(RenderCommand) * MAX_NUMBER_OF_RENDER_COMMANDS;
}

SharedRenderCommandArray* NewSharedRenderCommandArray(const char* name, const uint64_t name_length,
                                                      const SharedRenderCommandArrayAccess access) {
  int shm = 0;
  void* ptr = NULL;
  const uint64_t total_size = CalcTotalShmSize();
  SharedRenderCommandArray* result = NULL;
  if (!name || name_length == 0)
    goto finished;

  if (total_size <= 0)
    goto finished;

  if ((shm = shm_open(name, O_CREAT | O_RDWR, 0666)) == -1) {
    LOG_ERROR("shm_open failed");
    goto failed1;
  }

  if (ftruncate(shm, (uint32_t)total_size) == -1) {
    LOG_ERROR("ftruncate failed");
    goto failed1;
  }

  if ((ptr = mmap(NULL, total_size, (int)access, MAP_SHARED, shm, 0)) == MAP_FAILED) {
    LOG_ERROR("mmap failed");
    goto failed1;
  }

  if (!(result = malloc(sizeof(SharedRenderCommandArray)))) {
    LOG_ERROR("failed to alloc SharedRenderCommandArray");
    goto failed0;
  }

  result->name = strndup(name, name_length);
  result->shm = shm;
  result->size = total_size;
  result->start = result->current = (uint64_t)ptr;
  goto finished;

failed0:
  munmap(ptr, total_size);
failed1:
  close(shm);
failed2:
  shm_unlink(name);
finished:
  return result;
}

bool SharedRenderCommandArrayProtect(SharedRenderCommandArray* sarr, const SharedRenderCommandArrayAccess access) {
  if (!sarr)
    return false;
  if (sarr->start == 0 || sarr->size == 0)
    return false;
  mprotect((void*)sarr->start, sarr->size, (int)access);
  return true;
}

void SharedRenderCommandArrayFree(SharedRenderCommandArray* sarr) {
  if (!sarr)
    return;
  if (sarr->start != 0)
    munmap((void*)sarr->start, sarr->size);
  if (sarr->shm != 0) {
    close(sarr->shm);
    shm_unlink(sarr->name);
  }
  if (sarr->name)
    free(sarr->name);
}

#endif  // __linux__
