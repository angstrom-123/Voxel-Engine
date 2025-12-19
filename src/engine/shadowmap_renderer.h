#ifndef SHADOWMAP_RENDERER_H
#define SHADOWMAP_RENDERER_H

#include "renderer_base.h"

#include "include_sokol.h"

typedef struct shadowmap_renderer {
    struct targets {
        sg_image depth;
        sg_image zbuf;
    } targets;
    renderer_base_t base;
} shadowmap_renderer_t;

typedef struct chunk_renderer_desc {
    const renderer_base_desc_t *main_desc;
} shadowmap_renderer_desc_t;

extern void shadowmap_renderer_init(shadowmap_renderer_t *sr, const shadowmap_renderer_desc_t *desc);
extern void shadowmap_renderer_cleanup(shadowmap_renderer_t *sr);
extern void shadowmap_renderer_render_all(shadowmap_renderer_t *sr);

#endif
