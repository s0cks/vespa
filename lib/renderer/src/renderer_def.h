#ifndef VESPA_RENDERER_DEF_H
#define VESPA_RENDERER_DEF_H

#include "render_command.h"
#include "renderer.h"

struct _Renderer {
  SharedRenderCommandArray* commands;
};

#endif  // VESPA_RENDERER_DEF_H
