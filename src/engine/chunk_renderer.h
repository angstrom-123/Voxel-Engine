#ifndef CHUNK_RENDERER_H
#define CHUNK_RENDERER_H

#include "load_system.h"
#include "update_system.h"
#include "renderer_base.h"

#include "include_sokol.h"
#include "texture_handler.h"
#include <libem/em_bmp.h>
#include <libem/em_random.h>

#include <stdio.h>

#include "shaders/chunk.glsl.h"

#define MIP_LEVELS 5

#define COLOUR_PIXELFORMAT SG_PIXELFORMAT_RGBA8
#define NORMAL_PIXELFORMAT SG_PIXELFORMAT_RGBA8
#define DATA_PIXELFORMAT SG_PIXELFORMAT_RG32F
#define DEPTH_PIXELFORMAT SG_PIXELFORMAT_R32F
#define SHADOW_PIXELFORMAT SG_PIXELFORMAT_R16F

#define SSAO_SAMPLES 64
#define SSAO_NOISE_SCALE 4

typedef struct skybox_vertex {
    vec3 pos;
    vec3 nrm;
    vec2 uv;
} skybox_vertex_t;

typedef struct chunk_renderer {
    struct targets {
        // offscreen
        sg_image albedo;
        sg_image normal;
        sg_image depth;
        sg_image shadow;
        sg_image zbuf;
        // shadowmap
        sg_image shadowmap;
        sg_image zbuf_shadow;
        // Skybox
        sg_image skybox;
        sg_image zbuf_skybox;
        // composite 
        sg_image colour;
        sg_image zbuf_composite;
    } targets;
    renderer_base_t offscreen_base;
    renderer_base_t shadowmap_base;
    renderer_base_t composite_base;
    renderer_base_t skybox_base;
    renderer_base_t effects_base;
    struct {
        vec3 sun_dir;
        render_data_t chunk_data;
        render_coords_t chunk_coords;
        vec4 ssao_kernel[SSAO_SAMPLES];
        vec2 ssao_noise_data[em_sqr(SSAO_NOISE_SCALE)];
        sg_image ssao_noise_image;
        float view_distance;
    } info;
} chunk_renderer_t;

typedef struct chunk_renderer_desc {
    const renderer_base_desc_t *base_desc;
    const renderer_base_desc_t *shadowmap_base_desc;
    vec3 sun_dir;
    float view_distance;
} chunk_renderer_desc_t;

extern void chunk_renderer_init(chunk_renderer_t *cr, const chunk_renderer_desc_t *desc);
extern void chunk_renderer_load_textures(chunk_renderer_t *cr);
extern void chunk_renderer_cleanup(chunk_renderer_t *cr);
extern void chunk_renderer_render_all(chunk_renderer_t *cr);
extern void chunk_renderer_resize(chunk_renderer_t *cr, const vec2 dimensions);

#endif
