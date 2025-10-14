#include "game.h"

void game_init(game_t *game, const game_desc_t *desc)
{
    sg_setup(&(sg_desc) {
        .environment = sglue_environment(),
        .logger.func = slog_func,
    });

    snk_setup(&(snk_desc_t) {
        .enable_set_mouse_cursor = false,
        .dpi_scale = sapp_dpi_scale(),
        .logger.func = slog_func
    });

    const size_t QUEUE_SIZE = 128;
    const size_t MAX_ACTIVE_CHUNKS = (2 * em_sqr(desc->render_distance)) 
                                   + (2 * desc->render_distance);

    event_sys_init(&game->event_sys);

    update_sys_init(&game->update_sys, &(update_system_desc_t) {
        .chunk_capacity = MAX_ACTIVE_CHUNKS,
        .free_capacity = desc->num_chunk_slots,
        .request_capacity = QUEUE_SIZE,
        .ticks_per_second = desc->ticks_per_second
    });
    mtx_lock(&game->update_sys.init_lock);
    cnd_wait(&game->update_sys.thread_initialized, &game->update_sys.init_lock);

    chunk_sys_init(&game->chunk_sys, &(chunk_system_desc_t) {
        .chunk_data_capacity = MAX_ACTIVE_CHUNKS,
        .accumulator_capacity = QUEUE_SIZE,
        .request_capacity = QUEUE_SIZE,
        .seed = desc->seed
    });
    chunk_sys_init_thread(&game->chunk_sys, &(chunk_system_thread_args_t) {
        .cs = &game->chunk_sys,
        .us = &game->update_sys
    });
    mtx_lock(&game->chunk_sys.init_lock);
    cnd_wait(&game->chunk_sys.thread_initialized, &game->chunk_sys.init_lock);

    render_sys_init(&game->render_sys, &(render_system_desc_t) {
        .window_size = (vec2) {
            .x = sapp_width(),
            .y = sapp_height()
        },
        .view_distance = (desc->render_distance + 3) * CHUNK_SIZE,
        .vbo = game->update_sys.vbo,
        .ibo = game->update_sys.ibo
    });

    load_sys_init(&game->load_sys, &(load_system_desc_t) {
        .render_dist = desc->render_distance,
        .start_pos = (ivec2) {0, 0}
    });

    load_sys_load_initial(&game->load_sys, &game->chunk_sys);
}

void game_cleanup(game_t *game)
{
    render_sys_cleanup(&game->render_sys);
    load_sys_cleanup(&game->load_sys);
    chunk_sys_cleanup(&game->chunk_sys);
    update_sys_cleanup(&game->update_sys);
    event_sys_cleanup(&game->event_sys);
}

void game_event(game_t *game, const event_t *event)
{
    event_sys_get_event(&game->event_sys, event);
}

void game_render(game_t *game)
{
    render_data_t data = update_sys_get_render_data(&game->update_sys);
    render_coords_t coords = load_sys_get_render_coords(&game->load_sys);
    render_sys_render(&game->render_sys, data, coords);
    update_sys_return_render_data(&game->update_sys, &data);
}
