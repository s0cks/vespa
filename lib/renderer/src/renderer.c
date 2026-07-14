#include <stdio.h>
#include <stdlib.h>

#include "log.h"
#include "render_command.h"
#include "renderer_def.h"

Renderer* NewRenderer() {
  const uint64_t name_length = 1;
  char name[name_length];

  Renderer* result = malloc(sizeof(Renderer));
  if (!result) {
    LOG_ERROR("failed to alloc Renderer instance");
    goto finished;
  }

  SharedRenderCommandArray* sarr = NewSharedRenderCommandArray(name, name_length, kReadWrite);
  if (!sarr) {
    LOG_ERROR("failed to alloc SharedRenderCommandArray");
    goto failed0;
  }

  result->commands = sarr;
  goto finished;

failed0:
  if (result)
    free(result);
finished:
  return result;
}

void RendererFree(Renderer* rend) {
  if (!rend)
    return;

  SharedRenderCommandArrayFree(rend->commands);
}
