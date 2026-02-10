#include "render_system.h"

static void _on_resize(const event_t *ev, void *args)
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

    chunk_renderer_resize(&rs->chunk_renderer, ev->window_size);

    rs->cursor_line_renderer.base.dimensions = ev->window_size;
    rs->global_line_renderer.base.dimensions = ev->window_size;
    rs->sprite_renderer.base.dimensions = ev->window_size;
}

void render_sys_init(render_system_t *rs, const render_system_desc_t *desc)
{
    const float w = desc->window_size.x;
    const float h = desc->window_size.y;

    /* Cameras. */
    cam_init(&rs->cam, PROJECTION_PERSPECTIVE, &(camera_desc_t) {
        .near   = 0.005,
        .far    = desc->max_distance,
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
        .near   = 0.0,
        .far    = 100.0,
        .left   = -512.0,
        .right  = 512.0,
        .bottom = -512.0,
        .top    = 512.0,
        .pos    = VEC3(0.0, 0.0, 0.0)
    });
    cam_set_scale(&rs->shadow_cam, desc->shadow_scale);

    rs->shadow_cam.view = em_look_at(desc->inv_sun_dir, VEC3(0, (float) CHUNK_HEIGHT / 2, 0), WORLD_Y);
    cam_update(&rs->shadow_cam);

    /* Renderers */
    chunk_renderer_init(&rs->chunk_renderer, &(chunk_renderer_desc_t) {
        .base_desc = &(renderer_base_desc_t) {
            .dimensions = desc->window_size,
            .cam = &rs->cam,
        },
        .shadowmap_base_desc = &(renderer_base_desc_t) {
            .dimensions = VEC2(1024.0, 1024.0),
            .cam = &rs->shadow_cam,
        },
        .sun_dir = em_mul_vec3_f(desc->inv_sun_dir, -1.0),
        .view_distance = desc->view_distance
    });
    chunk_renderer_load_textures(&rs->chunk_renderer);

    line_renderer_init(&rs->cursor_line_renderer, &(line_renderer_desc_t) {
        .max_lines = 12,
        .base_desc = &(renderer_base_desc_t) {
            .dimensions = desc->window_size,
            .cam = &rs->cam,
        }
    });

    line_renderer_init(&rs->global_line_renderer, &(line_renderer_desc_t) {
        .max_lines = 128,
        .base_desc = &(renderer_base_desc_t) {
            .dimensions = desc->window_size,
            .cam = &rs->cam,
        }
    });

    sprite_renderer_init(&rs->sprite_renderer, &(sprite_renderer_desc_t) {
        .max_sprites = 256,
        .base_desc = &(renderer_base_desc_t) {
            .dimensions = desc->window_size,
            .cam = &rs->ortho_cam,
        }
    });
    sprite_renderer_load_textures(&rs->sprite_renderer);

    /* Resize event hook. */
    event_sys_subscribe_to_event(desc->es, EVENT_RESIZED, &(event_subscriber_desc_t) {
        .event_cb = _on_resize,
        .block_cb = event_block_never,
        .args = rs
    });

    /* Block cursor. */
    rs->cursor_active = false;
    const vec3 cursor_col = VEC3(1.0, 1.0, 1.0);
    line_renderer_push_all(&rs->cursor_line_renderer, NULL, 12, (line_desc_t[]) {
        { .from = VEC3(0.0, 0.0, 0.0), .to = VEC3(0.0, 0.0, 1.0), .col = cursor_col },
        { .from = VEC3(0.0, 0.0, 1.0), .to = VEC3(1.0, 0.0, 1.0), .col = cursor_col },
        { .from = VEC3(1.0, 0.0, 1.0), .to = VEC3(1.0, 0.0, 0.0), .col = cursor_col },
        { .from = VEC3(1.0, 0.0, 0.0), .to = VEC3(0.0, 0.0, 0.0), .col = cursor_col },
        { .from = VEC3(0.0, 1.0, 0.0), .to = VEC3(0.0, 1.0, 1.0), .col = cursor_col },
        { .from = VEC3(0.0, 1.0, 1.0), .to = VEC3(1.0, 1.0, 1.0), .col = cursor_col },
        { .from = VEC3(1.0, 1.0, 1.0), .to = VEC3(1.0, 1.0, 0.0), .col = cursor_col },
        { .from = VEC3(1.0, 1.0, 0.0), .to = VEC3(0.0, 1.0, 0.0), .col = cursor_col },
        { .from = VEC3(0.0, 0.0, 0.0), .to = VEC3(0.0, 1.0, 0.0), .col = cursor_col },
        { .from = VEC3(0.0, 0.0, 1.0), .to = VEC3(0.0, 1.0, 1.0), .col = cursor_col },
        { .from = VEC3(1.0, 0.0, 1.0), .to = VEC3(1.0, 1.0, 1.0), .col = cursor_col },
        { .from = VEC3(1.0, 0.0, 0.0), .to = VEC3(1.0, 1.0, 0.0), .col = cursor_col }
    });

    /* Crosshair. */
    const vec2 crosshair_size = VEC2(40.0, 40.0);
    sprite_renderer_push(&rs->sprite_renderer, &(sprite_desc_t) {
        .pos = em_mul_vec2_f(crosshair_size, -0.5),
        .size = crosshair_size,
        .z_index = 1.0
    });
}

void render_sys_cleanup(render_system_t *rs)
{
    chunk_renderer_cleanup(&rs->chunk_renderer);
    line_renderer_cleanup(&rs->cursor_line_renderer);
    line_renderer_cleanup(&rs->global_line_renderer);
    sprite_renderer_cleanup(&rs->sprite_renderer);
}

void render_sys_render(render_system_t *rs, update_system_t *us, load_system_t *ls)
{
    /* Chunks. */
    { rs->chunk_renderer.info.chunk_data = update_sys_borrow_render_data(us);
        rs->chunk_renderer.info.chunk_coords = load_sys_get_render_coords(ls);
        chunk_renderer_render_all(&rs->chunk_renderer);
    } update_sys_return_render_data(us, &rs->chunk_renderer.info.chunk_data);

    /* World Lines. */
    line_renderer_render_all(&rs->global_line_renderer);

    /* Block Cursor. */
    if (rs->cursor_active) line_renderer_render_all(&rs->cursor_line_renderer);

    /* Sprites. */
    sprite_renderer_render_all(&rs->sprite_renderer);

    sg_commit();
}
