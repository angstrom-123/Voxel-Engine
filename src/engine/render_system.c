#include "render_system.h"

static void _on_resize(const event_t *ev, void *args)
{
    render_system_t *rs = args;

    rs->cam.aspect = ev->window_size.x / ev->window_size.y;
    rs->cam.proj   = em_perspective(rs->cam.fov, rs->cam.aspect, 
                                    rs->cam.near, rs->cam.far);
    cam_update(&rs->cam);

    rs->ortho_cam.width  = ev->window_size.x;
    rs->ortho_cam.height = ev->window_size.y;
    rs->ortho_cam.proj   = em_orthographic(rs->ortho_cam.width, rs->ortho_cam.height, 
                                           rs->ortho_cam.near, rs->ortho_cam.far);
    cam_update(&rs->ortho_cam);

    chunk_renderer_resize(&rs->chunk_renderer, ev->window_size);

    rs->cursor_line_renderer.base.dimensions = ev->window_size;
    rs->global_line_renderer.base.dimensions = ev->window_size;
    rs->ui_renderer.base.dimensions = ev->window_size;
}

void render_sys_init(render_system_t *rs, const render_system_desc_t *desc)
{
    const float w = desc->window_size.x;
    const float h = desc->window_size.y;

    /* Cameras. */
    cam_init(&rs->cam, PROJECTION_PERSPECTIVE, &(camera_desc_t) {
        .near      = 0.1,
        .far       = desc->view_distance,
        .aspect    = w / h,
        .fov       = 60.0,
        .pos       = { 0.0, 0.0, 0.0 }
    });

    cam_init(&rs->ortho_cam, PROJECTION_ORTHOGRAPHIC, &(camera_desc_t) {
        .near   = 0.1,
        .far    = 100.0,
        .width  = w,
        .height = h,
        .pos    = { 0.0, 0.0, 0.0 }
    });

    cam_init(&rs->shadow_cam, PROJECTION_ORTHOGRAPHIC, &(camera_desc_t) {
        .near   = 50.0,
        .far    = 250.0,
        .width  = 1024,
        .height = 1024,
        .pos    = { 0.0, 0.0, 0.0 }
    });
    cam_set_scale(&rs->shadow_cam, desc->shadow_scale);

    // TODO: Stop fiddling with this crap and add:
    //       Texel-perfect-alignment: Move cam in discrete steps aligning with texel,
    //                                this should let me just move it with the player
    //                                without stepping at each chunk.
    //       Cascades: For this, need to figure out how to tightly fit the camera 
    //                 frustum to the player's surroundings.

    // vec3 inv_sun_dir = (vec3) { -2.0, -4.0, -1.0 };
    vec3 inv_sun_dir = { -1.0, -5.0, -3.0 };
    vec3 centre = { 0.0, 0.0, 0.0 };
    rs->shadow_cam.view = em_look_at(inv_sun_dir, centre, WORLD_Y);

    cam_update(&rs->shadow_cam);

    /* Renderers */
    chunk_renderer_init(&rs->chunk_renderer, &(chunk_renderer_desc_t) {
        .base_desc = &(renderer_base_desc_t) {
            .dimensions = desc->window_size,
            .cam = &rs->cam,
        },
        .shadowmap_base_desc = &(renderer_base_desc_t) {
            .dimensions = (vec2) { rs->shadow_cam.width, rs->shadow_cam.height },
            .cam = &rs->shadow_cam,
        },
        .inv_sun_dir = inv_sun_dir
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

    ui_renderer_init(&rs->ui_renderer, &(ui_renderer_desc_t) {
        .dimensions = desc->window_size,
    });

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
    ui_renderer_cleanup(&rs->ui_renderer);
}

void render_sys_render(render_system_t *rs, update_system_t *us, 
                       load_system_t *ls, ui_system_t *uis)
{
    /* Chunks. */
    { rs->chunk_renderer.data = update_sys_borrow_render_data(us);

        rs->chunk_renderer.coords = load_sys_get_render_coords(ls);
        chunk_renderer_render_all(&rs->chunk_renderer);

    } update_sys_return_render_data(us, &rs->chunk_renderer.data);

    /* World Lines. */
    line_renderer_render_all(&rs->global_line_renderer);

    /* Block Cursor. */
    if (rs->cursor_active) line_renderer_render_all(&rs->cursor_line_renderer);

    /* Sprites. */
    sprite_renderer_render_all(&rs->sprite_renderer);

    /* UI Components. */
    { rs->ui_renderer.ctx = snk_new_frame();
        rs->ui_renderer.components = ui_sys_get_components(uis, &rs->ui_renderer.component_count);

    } ui_renderer_render_all(&rs->ui_renderer);

    sg_commit();
}
