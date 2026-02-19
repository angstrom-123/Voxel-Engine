#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include "logger.h"
#include "camera.h"
#include "event_system.h"
#include "load_system.h"
#include "line_renderer.h"
#include "sprite_renderer.h"
#include "chunk_renderer.h"
#include "instrumentor.h"
#include "include_sokol.h"

#include <libem/em_bmp.h>

typedef struct render_system {
    camera_t cam;
    camera_t ortho_cam;
    camera_t shadow_cam;
    bool cams_initialized;
    chunk_renderer_t chunk_renderer;
    line_renderer_t cursor_line_renderer;
    sprite_renderer_t sprite_renderer;
    atomic_bool rendering_frame;
} render_system_t;

extern void render_sys_init_cameras(render_system_t *rs, event_system_t *es, 
                                    vec3 sun_dir, vec2 window_size);
extern void render_sys_init_chunk_renderer(render_system_t *rs, float view_distance,
                                           vec3 sun_dir, vec2 window_size);
extern void render_sys_init_sprite_renderer(render_system_t *rs, vec2 window_size);
extern void render_sys_init_cursor_renderer(render_system_t *rs, vec2 window_size);
extern void render_sys_cleanup(render_system_t *rs);
extern void render_sys_render(render_system_t *rs, update_system_t *us, 
                              load_system_t *ls, bool show_cursor);
extern void render_sys_set_view_distance(render_system_t *rs, float view_distance);

#endif
