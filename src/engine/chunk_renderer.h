#ifndef CHUNK_RENDERER_H
#define CHUNK_RENDERER_H

#include "load_system.h"
#include "update_system.h"
#include "renderer_base.h"

#include "include_sokol.h"
#include <libem/em_bmp.h>

#include <stdio.h>

#include "shaders/composite.glsl.h"
#ifdef DEBUG
    #include "shaders/chunk_debug.glsl.h"
#else
    #include "shaders/chunk.glsl.h"
#endif

#define MIP_LEVELS 5

#define COLOUR_PIXELFORMAT SG_PIXELFORMAT_RGBA8
#define NORMAL_PIXELFORMAT SG_PIXELFORMAT_RGBA8
#define DEPTH_PIXELFORMAT SG_PIXELFORMAT_R32F
#define POSITION_PIXELFORMAT SG_PIXELFORMAT_RGBA32F

typedef struct chunk_renderer {
    struct targets {
        sg_image colour;
        sg_image normal;
        sg_image depth;
        sg_image position;
        sg_image zbuf;
    } targets;
    renderer_base_t offscreen_base;
    renderer_base_t display_base;
    render_data_t data;
    render_coords_t coords;
} chunk_renderer_t;

typedef struct chunk_renderer_desc {
    const renderer_base_desc_t *base_desc;
} chunk_renderer_desc_t;

extern void chunk_renderer_init(chunk_renderer_t *cr, const chunk_renderer_desc_t *desc);
extern void chunk_renderer_load_textures(chunk_renderer_t *cr);
extern void chunk_renderer_cleanup(chunk_renderer_t *cr);
extern void chunk_renderer_render_all(chunk_renderer_t *cr);
extern void chunk_renderer_resize(chunk_renderer_t *cr, const vec2 dimensions);

#endif
