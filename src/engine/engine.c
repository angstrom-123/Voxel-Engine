#include "engine.h"

static void _api_subscribe_to_event(engine_t *engine, event_type_e type,
                             const event_subscriber_desc_t *desc)
{
    event_sys_subscribe_to_event(&engine->_event_sys, type, desc);
}

static void _api_edit_active_block(engine_t *engine, block_action_e action)
{
    if (!engine->meta.cursor.active)
        return;

    ivec3 cell = engine->meta.cursor.cell;
    ivec2 chunk = engine->meta.cursor.chunk;

    ivec2 data_idx = {1, 1};
    chunk_data_t *cd[3][3];
    chunk_sys_borrow_surrounding_data(&engine->_chunk_sys, chunk, cd);

    switch (action) {
    case BLOCK_ACTION_BREAK:
    {
        cd[data_idx.x][data_idx.y]->types[cell.x][cell.y][cell.z] = CUBETYPE_AIR;
        engine->meta.cursor.active = false;
        break;
    }
    case BLOCK_ACTION_PLACE:
    {
        /* Finding adjacent cell and chunk to place the block in. */
        switch (engine->meta.cursor.face) {
        case FACEIDX_LEFT:
            cell.x -= 1;
            if (cell.x == -1)
            {
                cell.x = CHUNK_SIZE - 1;
                chunk = REL_W(chunk);
                data_idx.x -= 1;
            }
            break;
        case FACEIDX_RIGHT:
            cell.x += 1;
            if (cell.x == CHUNK_SIZE)
            {
                cell.x = 0;
                chunk = REL_E(chunk);
                data_idx.x += 1;
            }
            break;
        case FACEIDX_TOP:
            cell.y += 1;
            if (cell.y == CHUNK_HEIGHT)
            {
                chunk_sys_return_surrounding_data(&engine->_chunk_sys, cd);
                return;
            }
            break;
        case FACEIDX_BOTTOM:
            cell.y -= 1;
            if (cell.y == -1)
            {
                chunk_sys_return_surrounding_data(&engine->_chunk_sys, cd);
                return;
            }
            break;
        case FACEIDX_FRONT:
            cell.z += 1;
            if (cell.z == CHUNK_SIZE)
            {
                cell.z = 0;
                chunk = REL_N(chunk);
                data_idx.y -= 1;
            }
            break;
        case FACEIDX_BACK:
            cell.z -= 1;
            if (cell.z == -1)
            {
                cell.z = CHUNK_SIZE - 1;
                chunk = REL_S(chunk);
                data_idx.y += 1;
            }
            break;
        };
        
        cd[data_idx.x][data_idx.y]->types[cell.x][cell.y][cell.z] = CUBETYPE_SAND;

        break;
    }
    default:
        break;
    };

    chunk_sys_return_surrounding_data(&engine->_chunk_sys, cd);

    CS_REQUEST(&engine->_chunk_sys, CSREQ_REMESH, chunk);
    if (!em_equals_ivec2(chunk, engine->meta.cursor.chunk))
        CS_REQUEST(&engine->_chunk_sys, CSREQ_REMESH, engine->meta.cursor.chunk);

    if (cell.x == CHUNK_SIZE - 1) 
        CS_REQUEST(&engine->_chunk_sys, CSREQ_REMESH, REL_E(chunk));
    else if (cell.x == 0) 
        CS_REQUEST(&engine->_chunk_sys, CSREQ_REMESH, REL_W(chunk));

    if (cell.z == CHUNK_SIZE - 1) 
        CS_REQUEST(&engine->_chunk_sys, CSREQ_REMESH, REL_N(chunk));
    else if (cell.z == 0) 
        CS_REQUEST(&engine->_chunk_sys, CSREQ_REMESH, REL_S(chunk));
}

void engine_init(engine_t *engine, const engine_desc_t *desc)
{
    const size_t MAX_ACTIVE = (2 * em_sqr(desc->render_distance)) 
                            + (2 * desc->render_distance) + 1;
    const size_t QUEUE_SIZE = MAX_ACTIVE * 2;
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
        .no_default_font = false
    });
    ENGINE_LOG_OK("Setup sokol nuklear.\n", NULL);

    event_sys_init(&engine->_event_sys);
    ENGINE_LOG_OK("Setup event system.\n", NULL);

    update_sys_init(&engine->_update_sys, &(update_system_desc_t) {
        .chunk_capacity = MAX_ACTIVE,
        .free_capacity = MAX_ACTIVE,
        .request_capacity = QUEUE_SIZE
    });
    update_sys_init_thread(&engine->_update_sys, &(update_system_thread_args_t) {
        .us = &engine->_update_sys
    });
    ENGINE_LOG_OK("Setup update system.\n", NULL);

    chunk_sys_init(&engine->_chunk_sys, &(chunk_system_desc_t) {
        .chunk_data_capacity = MAX_ACTIVE,
        .request_capacity = QUEUE_SIZE,
        .seed = desc->seed
    });
    chunk_sys_init_thread(&engine->_chunk_sys, &(chunk_system_thread_args_t) {
        .cs = &engine->_chunk_sys,
        .us = &engine->_update_sys
    });
    ENGINE_LOG_OK("Setup chunk system.\n", NULL);

    /* This multiplier brings in the fog closer to mostly prevent the player seeing the void. */
    // const size_t VIEW_DIST_MULTIPLIER = 9;
    const size_t VIEW_DIST_MULTIPLIER = 100;
    render_sys_init(&engine->_render_sys, &(render_system_desc_t) {
        .es = &engine->_event_sys,
        .window_size = {sapp_width(), sapp_height()},
        .view_distance = desc->render_distance * VIEW_DIST_MULTIPLIER,
        .shadow_scale = 3.0
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
    tick_sys_init_thread(&engine->_tick_sys, &(tick_system_thread_args_t) {
        .ts = &engine->_tick_sys,
    });
    ENGINE_LOG_OK("Setup tick system.\n", NULL);

    engine->api.subscribe_to_event = _api_subscribe_to_event;
    engine->api.edit_active_block = _api_edit_active_block;
    ENGINE_LOG_OK("Setup engine api.\n", NULL);

    engine->meta = (engine_meta_t) {
        .cursor = {
            .active = false,
            .cell = {-1, -1, -1},
            .chunk = {0, 0},
            .face = -1,
            .range = -1
        }
    };
    ENGINE_LOG_OK("Setup engine metadata.\n", NULL);

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
    render_sys_render(&engine->_render_sys, &engine->_update_sys, 
                      &engine->_load_sys, &engine->_ui_sys);
}

void engine_frame(engine_t *engine)
{
    event_sys_new_frame(&engine->_event_sys);

    ivec2 cam_chunk = {
        floorf(engine->_render_sys.cam.pos.x / (float) CHUNK_SIZE) * CHUNK_SIZE, 
        floorf(engine->_render_sys.cam.pos.z / (float) CHUNK_SIZE) * CHUNK_SIZE
    };

    if (load_sys_update(&engine->_load_sys, &engine->_chunk_sys, &engine->_update_sys, cam_chunk))
    {
        /* Nothing here for now. */
    }

    hit_desc_t hd = raycast(&engine->_chunk_sys, &(raycast_desc_t) {
        .origin = engine->_render_sys.cam.pos,
        .direction = cam_get_fwd(&engine->_render_sys.cam),
        .range = engine->meta.cursor.range
    });

    engine->meta.cursor.active = hd.hit;
    if (hd.hit) {
        engine->meta.cursor.cell = hd.cell;
        engine->meta.cursor.chunk = hd.ccord;
        engine->meta.cursor.face = hd.side;

        vec3 global_pos = {
            .x = hd.ccord.x + hd.cell.x,
            .y = hd.cell.y,
            .z = hd.ccord.y + hd.cell.z
        };

        engine->_render_sys.cursor_line_renderer.origin = global_pos;
        engine->_render_sys.cursor_active = true;
    } 
    else 
    {
        engine->_render_sys.cursor_active = false;
    }
}

void engine_tick(engine_t *engine)
{
    (void) engine;
}
