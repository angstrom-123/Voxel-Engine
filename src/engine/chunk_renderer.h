#ifndef CHUNK_RENDERER_H
#define CHUNK_RENDERER_H

#include "load_system.h"
#include "update_system.h"
#include "renderer_base.h"

#include "include_sokol.h"
#include "texture_handler.h"
#include <libem/em_bmp.h>

#include <stdio.h>

#include "shaders/composite.glsl.h"
#include "shaders/shadowmap.glsl.h"

#include "shaders/chunk.glsl.h"

#define MIP_LEVELS 5

#define COLOUR_PIXELFORMAT SG_PIXELFORMAT_RGBA8
#define NORMAL_PIXELFORMAT SG_PIXELFORMAT_RGBA8
#define DEPTH_PIXELFORMAT SG_PIXELFORMAT_R32F

typedef struct chunk_renderer {
    struct targets {
        sg_image colour;
        sg_image normal;
        sg_image depth;
        sg_image shadowmap;
        sg_image zbuf;
        sg_image zbuf_shadow;
    } targets;
    renderer_base_t offscreen_base;
    renderer_base_t display_base;
    renderer_base_t shadowmap_base;
    vec3 inv_sun_dir;
    render_data_t data;
    render_coords_t coords;
} chunk_renderer_t;

typedef struct chunk_renderer_desc {
    const renderer_base_desc_t *base_desc;
    const renderer_base_desc_t *shadowmap_base_desc;
    vec3 inv_sun_dir;
} chunk_renderer_desc_t;

extern void chunk_renderer_init(chunk_renderer_t *cr, const chunk_renderer_desc_t *desc);
extern void chunk_renderer_load_textures(chunk_renderer_t *cr);
extern void chunk_renderer_cleanup(chunk_renderer_t *cr);
extern void chunk_renderer_render_all(chunk_renderer_t *cr);
extern void chunk_renderer_resize(chunk_renderer_t *cr, const vec2 dimensions);

#endif
