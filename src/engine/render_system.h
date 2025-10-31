#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include "logger.h"
#include "camera.h"
#include "event_system.h"
#include "ui_system.h"
#include "load_system.h"
#include "instrumentor.h"

#if !defined(SOKOL_GFX_INCLUDED) 
#include <sokol/sokol_gfx.h>
#endif

#if !defined(SOKOL_GLUE_INCLUDED) 
#include <sokol/sokol_glue.h>
#endif
#if !defined(SOKOL_APP_INCLUDED) 
#include <sokol/sokol_app.h>
#endif

#if !defined(NK_NUKLEAR_H_) 
#include <nuklear/nuklear.h>
#endif

#if !defined(SOKOL_NUKLEAR_INCLUDED)
#include <sokol/sokol_nuklear.h>
#endif

#include <libem/em_bmp.h>

#ifdef DEBUG
#include "shaders/chunk_debug.glsl.h"
#else
#include "shaders/chunk.glsl.h"
#endif

#define MAX_UI_COMPONENTS 5

typedef struct render_system {
    vec2 dimensions;
    sg_pipeline pip;
    sg_bindings bind;
    sg_pass_action pass_act;
    camera_t cam;
} render_system_t;

typedef struct render_system_desc {
    event_system_t *es;
    vec2 window_size;
    float view_distance;
} render_system_desc_t;

extern void render_sys_init(render_system_t *rs, const render_system_desc_t *desc);
extern void render_sys_cleanup(render_system_t *rs);
extern void render_sys_render(render_system_t *rs, render_data_t data, render_coords_t crd_data);
extern void render_sys_render_ui(render_system_t *rs, ui_component_t **components, size_t component_cnt);
extern void render_sys_scene_start(render_system_t *rs);
extern void render_sys_scene_end(render_system_t *rs);

#endif
