#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#if !defined(SOKOL_GFX_INCLUDED) 
#include <sokol/sokol_gfx.h>
#endif

#if !defined(SOKOL_GLUE_INCLUDED) 
#include <sokol/sokol_glue.h>
#endif

#include <libem/em_bmp.h>

#include "camera.h"
#include "logger.h"
#include "chunk.glsl.h"
#include "update_system.h"
#include "load_system.h"

typedef struct render_system {
    sg_pipeline pip;
    sg_bindings bind;
    sg_pass_action pass_act;
    camera_t cam;
} render_system_t;

typedef struct render_system_desc {
    vec2 window_size;
    float view_distance;
    sg_buffer vbo;
    sg_buffer ibo;
} render_system_desc_t;

extern void render_sys_init(render_system_t *rs, const render_system_desc_t *desc);
extern void render_sys_cleanup(render_system_t *rs);
extern void render_sys_render(render_system_t *rs, render_data_t data, render_coords_t crd_data);

#endif
