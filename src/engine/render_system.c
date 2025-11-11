#include "render_system.h"

void _on_resize(const event_t *ev, void *args)
{
    render_system_t *rs = args;
    rs->chunk_renderer.base.dimensions = ev->window_size;
    rs->cursor_line_renderer.base.dimensions = ev->window_size;
    rs->global_line_renderer.base.dimensions = ev->window_size;
    rs->ui_renderer.base.dimensions = ev->window_size;
}

void render_sys_init(render_system_t *rs, const render_system_desc_t *desc)
{
    /* Camera. */
    cam_init(&rs->cam, &(camera_desc_t) {
        .near      = 0.1,
        .far       = desc->view_distance,
        .aspect    = desc->window_size.x / desc->window_size.y,
        .fov       = 60.0,
        .pos       = {0.0, 0.0, 0.0},
    });

    /* Renderers. */
    chunk_renderer_init(&rs->chunk_renderer, &(chunk_renderer_desc_t) {
        .dimensions = desc->window_size,
        .cam = &rs->cam
    });
    chunk_renderer_load_textures(&rs->chunk_renderer);

    line_renderer_init(&rs->cursor_line_renderer, &(line_renderer_desc_t) {
        .max_lines = 12,
        .dimensions = desc->window_size,
        .cam = &rs->cam
    });

    line_renderer_init(&rs->global_line_renderer, &(line_renderer_desc_t) {
        .max_lines = 128,
        .dimensions = desc->window_size,
        .cam = &rs->cam
    });

    ui_renderer_init(&rs->ui_renderer, &(ui_renderer_desc_t) {
        .dimensions = desc->window_size
    });

    /* Resize event hook. */
    event_sys_subscribe_to_event(desc->es, EVENT_RESIZED, &(event_subscriber_desc_t) {
        .event_cb = _on_resize,
        .block_cb = event_block_never,
        .args = rs
    });

    /* Block cursor. */
    rs->cursor_active = false;
    const vec3 cursor_col = {1.0, 1.0, 1.0};
    line_renderer_push_all(&rs->cursor_line_renderer, NULL, 12, (line_desc_t[]) {
        {.from = {0.0, 0.0, 0.0}, .to = {0.0, 0.0, 1.0}, .col = cursor_col},
        {.from = {0.0, 0.0, 1.0}, .to = {1.0, 0.0, 1.0}, .col = cursor_col},
        {.from = {1.0, 0.0, 1.0}, .to = {1.0, 0.0, 0.0}, .col = cursor_col},
        {.from = {1.0, 0.0, 0.0}, .to = {0.0, 0.0, 0.0}, .col = cursor_col},
        {.from = {0.0, 1.0, 0.0}, .to = {0.0, 1.0, 1.0}, .col = cursor_col},
        {.from = {0.0, 1.0, 1.0}, .to = {1.0, 1.0, 1.0}, .col = cursor_col},
        {.from = {1.0, 1.0, 1.0}, .to = {1.0, 1.0, 0.0}, .col = cursor_col},
        {.from = {1.0, 1.0, 0.0}, .to = {0.0, 1.0, 0.0}, .col = cursor_col},
        {.from = {0.0, 0.0, 0.0}, .to = {0.0, 1.0, 0.0}, .col = cursor_col},
        {.from = {0.0, 0.0, 1.0}, .to = {0.0, 1.0, 1.0}, .col = cursor_col},
        {.from = {1.0, 0.0, 1.0}, .to = {1.0, 1.0, 1.0}, .col = cursor_col},
        {.from = {1.0, 0.0, 0.0}, .to = {1.0, 1.0, 0.0}, .col = cursor_col}
    });
}

void render_sys_cleanup(render_system_t *rs)
{
    chunk_renderer_cleanup(&rs->chunk_renderer);
    line_renderer_cleanup(&rs->cursor_line_renderer);
    line_renderer_cleanup(&rs->global_line_renderer);
    ui_renderer_cleanup(&rs->ui_renderer);
}

void render_sys_render(render_system_t *rs, update_system_t *us, 
                       load_system_t *ls, ui_system_t *uis)
{
    {
        rs->chunk_renderer.data = update_sys_borrow_render_data(us);

        rs->chunk_renderer.coords = load_sys_get_render_coords(ls);
        chunk_renderer_render_all(&rs->chunk_renderer);

        update_sys_return_render_data(us, &rs->chunk_renderer.data);
    }

    line_renderer_render_all(&rs->global_line_renderer);

    if (rs->cursor_active) line_renderer_render_all(&rs->cursor_line_renderer);

    {
        rs->ui_renderer.ctx = snk_new_frame();
        rs->ui_renderer.components = ui_sys_get_components(uis, &rs->ui_renderer.component_count);
        ui_renderer_render_all(&rs->ui_renderer);
    }

    sg_commit();
}
