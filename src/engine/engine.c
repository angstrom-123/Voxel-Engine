#include "engine.h"

void engine_init(engine_t *engine, const engine_desc_t *desc)
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

    event_sys_init(&engine->_event_sys);

    update_sys_init(&engine->_update_sys, &(update_system_desc_t) {
        .chunk_capacity = MAX_ACTIVE_CHUNKS,
        .free_capacity = desc->num_chunk_slots,
        .request_capacity = QUEUE_SIZE,
        .ticks_per_second = desc->ticks_per_second
    });
    update_sys_init_update_thread(&engine->_update_sys, &(update_system_thread_args_t) {
        .us = &engine->_update_sys
    });

    chunk_sys_init(&engine->_chunk_sys, &(chunk_system_desc_t) {
        .chunk_data_capacity = MAX_ACTIVE_CHUNKS,
        .accumulator_capacity = QUEUE_SIZE,
        .request_capacity = QUEUE_SIZE,
        .seed = desc->seed
    });
    chunk_sys_init_thread(&engine->_chunk_sys, &(chunk_system_thread_args_t) {
        .cs = &engine->_chunk_sys,
        .us = &engine->_update_sys
    });

    render_sys_init(&engine->_render_sys, &(render_system_desc_t) {
        .window_size = (vec2) {
            .x = sapp_width(),
            .y = sapp_height()
        },
        .view_distance = (desc->render_distance + 3) * CHUNK_SIZE,
        .vbo = engine->_update_sys.vbo,
        .ibo = engine->_update_sys.ibo
    });

    update_sys_init_tick_thread(&engine->_update_sys, &(update_system_thread_args_t) {
        .us = &engine->_update_sys
    });

    load_sys_init(&engine->_load_sys, &(load_system_desc_t) {
        .render_dist = desc->render_distance,
        .start_pos = (ivec2) {0, 0}
    });

    load_sys_load_initial(&engine->_load_sys, &engine->_chunk_sys);
}

void engine_cleanup(engine_t *engine)
{
    render_sys_cleanup(&engine->_render_sys);
    load_sys_cleanup(&engine->_load_sys);
    chunk_sys_cleanup(&engine->_chunk_sys);
    update_sys_cleanup(&engine->_update_sys);
    event_sys_cleanup(&engine->_event_sys);
}

void engine_event(engine_t *engine, const event_t *event)
{
    event_sys_get_event(&engine->_event_sys, event);
}

void engine_render(engine_t *engine)
{
    // INSTRUMENT_FUNC_BEGIN();
    render_data_t data = update_sys_get_render_data(&engine->_update_sys);
    render_coords_t coords = load_sys_get_render_coords(&engine->_load_sys);
    update_sys_force_buffer_update(&engine->_update_sys);
    render_sys_render(&engine->_render_sys, data, coords);
    update_sys_return_render_data(&engine->_update_sys, &data);
    // INSTRUMENT_FUNC_END();
}

void engine_frame_update(engine_t *engine)
{
    INSTRUMENT_FUNC_BEGIN();
    event_sys_new_frame(&engine->_event_sys);

    // TODO: Might have to do float division and floorf
    ivec2 cam_chunk = {
        floorf(engine->_render_sys.cam.pos.x / (float) CHUNK_SIZE) * CHUNK_SIZE, 
        floorf(engine->_render_sys.cam.pos.z / (float) CHUNK_SIZE) * CHUNK_SIZE
    };

    load_sys_update(&engine->_load_sys, &engine->_chunk_sys, &engine->_update_sys, cam_chunk);

    // if (load_sys_update(&engine->_load_sys, &engine->_chunk_sys, cam_chunk))
    //     update_sys_refresh_buffers(&engine->_update_sys);
    INSTRUMENT_FUNC_END();
}
