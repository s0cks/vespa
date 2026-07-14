#ifndef VESPA_RENDERER_H
#define VESPA_RENDERER_H

typedef struct _Renderer Renderer;

Renderer* NewRenderer();
void RendererFree(Renderer* ren);

#endif  // VESPA_RENDERER_H
