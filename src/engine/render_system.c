#include "render_system.h"

static bool _on_resize(const event_t *ev, void *args)
{
    render_system_t *rs = args;

    rs->cam.perspective.aspect = ev->window_size.x / ev->window_size.y;
    rs->cam.proj               = em_perspective(rs->cam.perspective.fov, 
                                                rs->cam.perspective.aspect, 
                                                rs->cam.near, rs->cam.far);
    cam_update(&rs->cam);

    rs->ortho_cam.orthographic = (struct orthographic) {
        .left   = -ev->window_size.x / 2.0,
        .right  = ev->window_size.x / 2.0,
        .bottom = -ev->window_size.y / 2.0,
        .top    = ev->window_size.y / 2.0
    };
    rs->ortho_cam.proj = em_orthographic(rs->ortho_cam.orthographic.left, 
                                         rs->ortho_cam.orthographic.right,
                                         rs->ortho_cam.orthographic.bottom,
                                         rs->ortho_cam.orthographic.top,
                                         rs->ortho_cam.near, rs->ortho_cam.far);
    cam_update(&rs->ortho_cam);

    if (rs->chunk_renderer.initialized)
        chunk_renderer_resize(&rs->chunk_renderer, ev->window_size);

    if (rs->cursor_line_renderer.initialized)
        rs->cursor_line_renderer.base.dimensions = ev->window_size;

    if (rs->sprite_renderer.initialized)
        rs->sprite_renderer.base.dimensions = ev->window_size;

    return false;
}

void render_sys_init_cameras(render_system_t *rs, event_system_t *es, 
                             vec3 sun_dir, vec2 window_size)
{
    if (rs->cams_initialized) return;

    const float w = window_size.x;
    const float h = window_size.y;

    /* Cameras. */
    cam_init(&rs->cam, PROJECTION_PERSPECTIVE, &(camera_desc_t) {
        .near   = 0.005,
        .far    = 2000.0,
        .aspect = w / h,
        .fov    = 80.0,
        .pos    = VEC3(0.0, 0.0, 0.0)
    });

    cam_init(&rs->ortho_cam, PROJECTION_ORTHOGRAPHIC, &(camera_desc_t) {
        .near   = 0.1,
        .far    = 100.0,
        .left   = -w / 2.0,
        .right  = w / 2.0,
        .bottom = -h / 2.0,
        .top    = h / 2.0,
        .pos    = VEC3(0.0, 0.0, 0.0)
    });

    cam_init(&rs->shadow_cam, PROJECTION_ORTHOGRAPHIC, &(camera_desc_t) {
        .near   = -100.0,
        .far    = 100.0,
        .left   = -512.0,
        .right  = 512.0,
        .bottom = -512.0,
        .top    = 512.0,
        .pos    = VEC3(0.0, 0.0, 0.0)
    });
    cam_set_scale(&rs->shadow_cam, 4.0); // Scale up shadows

    rs->shadow_cam.view = em_look_at(em_mul_vec3_f(sun_dir, -1.0), 
                                     VEC3(0, CHUNK_HEIGHT / 2.0, 0), 
                                     WORLD_Y);
    cam_update(&rs->shadow_cam);

    event_sys_subscribe_to_event(es, EVENT_RESIZED, &(event_subscriber_desc_t) {
        .event_cb = _on_resize,
        .block_cb = event_block_never,
        .args = rs
    });

    rs->cams_initialized = true;
}

extern void render_sys_init_chunk_renderer(render_system_t *rs, float view_distance, 
                                           vec3 sun_dir, vec2 window_size)
{
    if (rs->chunk_renderer.initialized) return;

    chunk_renderer_init(&rs->chunk_renderer, &(chunk_renderer_desc_t) {
        .base_desc = &(renderer_base_desc_t) {
            .dimensions = window_size,
            .cam = &rs->cam,
        },
        .shadowmap_base_desc = &(renderer_base_desc_t) {
            .dimensions = VEC2(1024.0, 1024.0),
            .cam = &rs->shadow_cam,
        },
        .sun_dir = sun_dir,
        .view_distance = view_distance
    });
    chunk_renderer_load_textures(&rs->chunk_renderer);
}

void render_sys_init_sprite_renderer(render_system_t *rs, vec2 window_size)
{
    if (rs->sprite_renderer.initialized) return;
    sprite_renderer_init(&rs->sprite_renderer, &(sprite_renderer_desc_t) {
        .max_sprites = 4096,
        .base_desc = &(renderer_base_desc_t) {
            .dimensions = window_size,
            .cam = &rs->ortho_cam,
        }
    });
    sprite_renderer_load_textures(&rs->sprite_renderer);
}

extern void render_sys_init_cursor_renderer(render_system_t *rs, vec2 window_size)
{
    if (rs->cursor_line_renderer.initialized) return;
    line_renderer_init(&rs->cursor_line_renderer, &(line_renderer_desc_t) {
        .max_lines = 12,
        .base_desc = &(renderer_base_desc_t) {
            .dimensions = window_size,
            .cam = &rs->cam,
        }
    });

    const vec3 col = VEC3(1.0, 1.0, 1.0);
    line_renderer_push_all(&rs->cursor_line_renderer, NULL, 12, (line_desc_t[]) {
        { .from = VEC3(0, 0, 0), .to = VEC3(0, 0, 1), .col = col },
        { .from = VEC3(0, 0, 1), .to = VEC3(1, 0, 1), .col = col },
        { .from = VEC3(1, 0, 1), .to = VEC3(1, 0, 0), .col = col },
        { .from = VEC3(1, 0, 0), .to = VEC3(0, 0, 0), .col = col },
        { .from = VEC3(0, 1, 0), .to = VEC3(0, 1, 1), .col = col },
        { .from = VEC3(0, 1, 1), .to = VEC3(1, 1, 1), .col = col },
        { .from = VEC3(1, 1, 1), .to = VEC3(1, 1, 0), .col = col },
        { .from = VEC3(1, 1, 0), .to = VEC3(0, 1, 0), .col = col },
        { .from = VEC3(0, 0, 0), .to = VEC3(0, 1, 0), .col = col },
        { .from = VEC3(0, 0, 1), .to = VEC3(0, 1, 1), .col = col },
        { .from = VEC3(1, 0, 1), .to = VEC3(1, 1, 1), .col = col },
        { .from = VEC3(1, 0, 0), .to = VEC3(1, 1, 0), .col = col }
    });
}

void render_sys_cleanup(render_system_t *rs)
{
    if (rs->chunk_renderer.initialized)
        chunk_renderer_cleanup(&rs->chunk_renderer);

    if (rs->cursor_line_renderer.initialized)
        line_renderer_cleanup(&rs->cursor_line_renderer);

    if (rs->sprite_renderer.initialized)
        sprite_renderer_cleanup(&rs->sprite_renderer);
}

void render_sys_render(render_system_t *rs, update_system_t *us, load_system_t *ls, bool show_cursor)
{
    atomic_store(&rs->rendering_frame, true);
    /* Chunks. */
    if (rs->chunk_renderer.initialized)
    {
        rs->chunk_renderer.info.chunk_data = update_sys_borrow_render_data(us);
        rs->chunk_renderer.info.chunk_coords = load_sys_get_render_coords(ls);
        chunk_renderer_render_all(&rs->chunk_renderer);
        update_sys_return_render_data(us, &rs->chunk_renderer.info.chunk_data);
    }

    /* Block Cursor. */
    if (rs->cursor_line_renderer.initialized && show_cursor) 
        line_renderer_render_all(&rs->cursor_line_renderer);

    /* Sprites. */
    if (rs->sprite_renderer.initialized)
        sprite_renderer_render_all(&rs->sprite_renderer);

    sg_commit();
    atomic_store(&rs->rendering_frame, false);
}

void render_sys_set_view_distance(render_system_t *rs, float view_distance)
{
    rs->chunk_renderer.info.view_distance = view_distance;
}
