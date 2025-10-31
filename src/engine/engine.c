#include "engine.h"

void engine_init(engine_t *engine, const engine_desc_t *desc)
{
    const size_t MAX_ACTIVE = (2 * em_sqr(desc->render_distance)) 
                            + (2 * desc->render_distance) + 1;
    const size_t QUEUE_SIZE = MAX_ACTIVE + (4 * (desc->render_distance + 1));
    const size_t BUF_POOL_SIZE = MAX_ACTIVE * 2 + 20;

    sg_setup(&(sg_desc) {
        .buffer_pool_size = BUF_POOL_SIZE,
        .environment = sglue_environment(),
        .logger.func = slog_func,
    });
    ENGINE_LOG_OK("Setup sokol gfx.\n", NULL);

    snk_setup(&(snk_desc_t) {
        .enable_set_mouse_cursor = false,
        .dpi_scale = sapp_dpi_scale(),
        .logger.func = slog_func,
        // .no_default_font = true // TODO
    });
    ENGINE_LOG_OK("Setup sokol nuklear.\n", NULL);

    event_sys_init(&engine->_event_sys);
    ENGINE_LOG_OK("Setup event system.\n", NULL);

    update_sys_init(&engine->_update_sys, &(update_system_desc_t) {
        .chunk_capacity = MAX_ACTIVE,
        .free_capacity = MAX_ACTIVE,
        .request_capacity = QUEUE_SIZE
    });
    ENGINE_LOG_OK("Setup update system.\n", NULL);
    update_sys_init_thread(&engine->_update_sys, &(update_system_thread_args_t) {
        .us = &engine->_update_sys
    });
    ENGINE_LOG_OK("Setup update system thread.\n", NULL);

    chunk_sys_init(&engine->_chunk_sys, &(chunk_system_desc_t) {
        .chunk_data_capacity = MAX_ACTIVE,
        .request_capacity = QUEUE_SIZE,
        .seed = desc->seed
    });
    ENGINE_LOG_OK("Setup chunk system.\n", NULL);
    chunk_sys_init_thread(&engine->_chunk_sys, &(chunk_system_thread_args_t) {
        .cs = &engine->_chunk_sys,
        .us = &engine->_update_sys
    });
    ENGINE_LOG_OK("Setup chunk system thread.\n", NULL);

    /* This multiplier brings in the fog closer to mostly prevent the player seeing the void. */
    // const size_t VIEW_DIST_MULTIPLIER = 9;
    const size_t VIEW_DIST_MULTIPLIER = 100;
    render_sys_init(&engine->_render_sys, &(render_system_desc_t) {
        .es = &engine->_event_sys,
        .window_size = (vec2) {
            .x = sapp_width(),
            .y = sapp_height()
        },
        .view_distance = desc->render_distance * VIEW_DIST_MULTIPLIER,
    });
    ENGINE_LOG_OK("Setup render system.\n", NULL);

    ui_sys_init(&engine->_ui_sys, &(ui_system_desc_t) {

    });
    ui_sys_add(&engine->_ui_sys, COMPONENT_CONSOLE, &(ui_component_desc_t) {
        .ptr = console_init(&(console_desc_t) {
            .es = &engine->_event_sys
        }),
        .render_callback = console_render,
        .visible = console_visible
    });
    ENGINE_LOG_OK("Setup ui system.\n", NULL);

    load_sys_init(&engine->_load_sys, &(load_system_desc_t) {
        .render_dist = desc->render_distance,
        .start_pos = (ivec2) {0, 0}
    });
    ENGINE_LOG_OK("Setup load system.\n", NULL);

    tick_sys_init(&engine->_tick_sys, &(tick_system_desc_t) {
        .tps = desc->ticks_per_second
    });
    ENGINE_LOG_OK("Setup tick system.\n", NULL);
    tick_sys_init_thread(&engine->_tick_sys, &(tick_system_thread_args_t) {
        .ts = &engine->_tick_sys,
        .ls = &engine->_load_sys
    });
    ENGINE_LOG_OK("Setup tick system thread.\n", NULL);

    load_sys_load_initial(&engine->_load_sys, &engine->_chunk_sys);
    ENGINE_LOG_OK("Request initial chunks.\n", NULL);
}

void engine_cleanup(engine_t *engine)
{
    render_sys_cleanup(&engine->_render_sys);
    load_sys_cleanup(&engine->_load_sys);
    chunk_sys_cleanup(&engine->_chunk_sys);
    update_sys_cleanup(&engine->_update_sys);
    tick_sys_cleanup(&engine->_tick_sys);
    event_sys_cleanup(&engine->_event_sys);
    ui_sys_cleanup(&engine->_ui_sys);
}

void engine_event(engine_t *engine, const event_t *event)
{
    event_sys_get_event(&engine->_event_sys, event);
}

void engine_render(engine_t *engine)
{
    render_sys_scene_start(&engine->_render_sys);

    INSTRUMENT_SCOPE_BEGIN(render_game)

    render_data_t data = update_sys_get_render_data(&engine->_update_sys);
    render_coords_t coords = load_sys_get_render_coords(&engine->_load_sys);
    render_sys_render(&engine->_render_sys, data, coords);
    update_sys_return_render_data(&engine->_update_sys, &data);

    INSTRUMENT_SCOPE_END(render_game)

    INSTRUMENT_SCOPE_BEGIN(render_ui);

    ui_component_t *comps[engine->_ui_sys.component_count];
    ui_sys_get_components(&engine->_ui_sys, comps);
    render_sys_render_ui(&engine->_render_sys, comps, engine->_ui_sys.component_count);

    INSTRUMENT_SCOPE_END(render_ui);

    render_sys_scene_end(&engine->_render_sys);
}

void engine_update_events(engine_t *engine)
{
    event_sys_new_frame(&engine->_event_sys);
}

void engine_frame_update(engine_t *engine)
{
    ivec2 cam_chunk = {
        floorf(engine->_render_sys.cam.pos.x / (float) CHUNK_SIZE) * CHUNK_SIZE, 
        floorf(engine->_render_sys.cam.pos.z / (float) CHUNK_SIZE) * CHUNK_SIZE
    };

    if (load_sys_update(&engine->_load_sys, &engine->_chunk_sys, &engine->_update_sys, cam_chunk))
    {
        /* Nothing here for now. */
    }
}
