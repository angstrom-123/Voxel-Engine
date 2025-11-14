#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include "logger.h"
#include "camera.h"
#include "event_system.h"
#include "ui_renderer.h"
#include "ui_system.h"
#include "load_system.h"
#include "line_renderer.h"
#include "sprite_renderer.h"
#include "chunk_renderer.h"
#include "instrumentor.h"

#include "include_sokol.h"
#include <libem/em_bmp.h>

#ifdef DEBUG
#include "shaders/chunk_debug.glsl.h"
#else
#include "shaders/chunk.glsl.h"
#endif

#define MAX_UI_COMPONENTS 5

typedef struct render_system {
    camera_t cam;
    camera_t ortho_cam;
    chunk_renderer_t chunk_renderer;
    line_renderer_t cursor_line_renderer;
    line_renderer_t global_line_renderer;
    sprite_renderer_t sprite_renderer;
    ui_renderer_t ui_renderer;
    bool cursor_active;
} render_system_t;

typedef struct render_system_desc {
    event_system_t *es;
    vec2 window_size;
    float view_distance;
} render_system_desc_t;

extern void render_sys_init(render_system_t *rs, const render_system_desc_t *desc);
extern void render_sys_cleanup(render_system_t *rs);
extern void render_sys_render(render_system_t *rs, update_system_t *us, 
                              load_system_t *ls, ui_system_t *uis);

#endif
