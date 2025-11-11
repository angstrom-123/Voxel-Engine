#ifndef CHUNK_RENDERER_H
#define CHUNK_RENDERER_H

#include "load_system.h"
#include "update_system.h"
#include "renderer_base.h"

#include "include_sokol.h"
#include <libem/em_bmp.h>

#include <stdio.h>

#ifdef DEBUG
    #include "shaders/chunk_debug.glsl.h"
#else
    #include "shaders/chunk.glsl.h"
#endif

#define MIP_LEVELS 5

typedef struct chunk_renderer {
    renderer_base_t base;
    render_data_t data;
    render_coords_t coords;
} chunk_renderer_t;

typedef struct chunk_renderer_desc {
    const camera_t *cam;
    vec2 dimensions;
} chunk_renderer_desc_t;

extern void chunk_renderer_init(chunk_renderer_t *cr, const chunk_renderer_desc_t *desc);
extern void chunk_renderer_load_textures(chunk_renderer_t *cr);
extern void chunk_renderer_cleanup(chunk_renderer_t *cr);
extern void chunk_renderer_render_all(chunk_renderer_t *cr);

#endif
