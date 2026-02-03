#include "engine.h"

static void _api_subscribe_to_event(engine_t *engine, event_type_e type,
                                    const event_subscriber_desc_t *desc)
{
    event_sys_subscribe_to_event(&engine->_event_sys, type, desc);
}

static void _api_edit_active_block(engine_t *engine, block_action_e action,
                                   const player_collider_desc_t *desc)
{
    if (!engine->meta.cursor.active)
        return;

    ivec3 cell = engine->meta.cursor.cell;
    ivec2 chunk = engine->meta.cursor.chunk;

    switch (action) {
    case BLOCK_ACTION_BREAK:
    {
        CS_REQUEST_BREAK(&engine->_chunk_sys, chunk, cell);
        engine->meta.cursor.active = false;
        break;
    }
    case BLOCK_ACTION_PLACE:
    {
        // Find the position to place the block based on cube face under cursor.
        switch (engine->meta.cursor.face) {
        case FACEIDX_LEFT:
            cell.x -= 1;
            if (cell.x == -1)
            {
                cell.x = CHUNK_SIZE - 1;
                chunk = REL_W(chunk);
            }
            break;
        case FACEIDX_RIGHT:
            cell.x += 1;
            if (cell.x == CHUNK_SIZE)
            {
                cell.x = 0;
                chunk = REL_E(chunk);
            }
            break;
        case FACEIDX_TOP:
            cell.y += 1;
            if (cell.y == CHUNK_HEIGHT) return;
            break;
        case FACEIDX_BOTTOM:
            cell.y -= 1;
            if (cell.y == -1) return;
            break;
        case FACEIDX_FRONT:
            cell.z += 1;
            if (cell.z == CHUNK_SIZE)
            {
                cell.z = 0;
                chunk = REL_N(chunk);
            }
            break;
        case FACEIDX_BACK:
            cell.z -= 1;
            if (cell.z == -1)
            {
                cell.z = CHUNK_SIZE - 1;
                chunk = REL_S(chunk);
            }
            break;
        };

        aabb_t block = aabb_from_voxel_coord(em_add_ivec3(IVEC3(chunk.x, 0, chunk.y), cell));
        if (!aabb_intersecting(block, aabb_with_offset(desc->collider, desc->pos)))
            CS_REQUEST_PLACE(&engine->_chunk_sys, chunk, cell, CUBETYPE_AIR);
        break;
    }
    default:
        break;
    };
}

void _api_init_systems(engine_t *engine, const engine_desc_t *desc)
{
    RUNTIME_ASSERT(desc->render_distance >= 3, "Render distance too low");
    const size_t MAX_ACTIVE = (2 * em_sqr(desc->render_distance)) + 
                              (2 * desc->render_distance) + 1;
    const size_t QUEUE_SIZE = MAX_ACTIVE * 2;
    const size_t BUF_POOL_SIZE = MAX_ACTIVE * 2 + 20;

    sg_setup(&(sg_desc) {
        .buffer_pool_size = BUF_POOL_SIZE,
        .environment = sglue_environment(),
        .logger.func = slog_func,
    });
    ENGINE_LOG_OK("Setup sokol gfx.\n", NULL);

    stm_setup();
    ENGINE_LOG_OK("Setup sokol time.\n", NULL);

    event_sys_init(&engine->_event_sys);
    ENGINE_LOG_OK("Setup event system.\n", NULL);

    update_sys_init(&engine->_update_sys, &(update_system_desc_t) {
        .chunk_capacity = MAX_ACTIVE,
        .free_capacity = MAX_ACTIVE,
        .request_capacity = QUEUE_SIZE
    });
    ENGINE_LOG_OK("Setup update system.\n", NULL);

    chunk_sys_init(&engine->_chunk_sys, &(chunk_system_desc_t) {
        .chunk_data_capacity = MAX_ACTIVE,
        .request_capacity = QUEUE_SIZE,
        .seed = desc->seed
    });
    ENGINE_LOG_OK("Setup chunk system.\n", NULL);

    render_sys_init(&engine->_render_sys, &(render_system_desc_t) {
        .es = &engine->_event_sys,
        .window_size = VEC2(sapp_width(), sapp_height()),
        .view_distance = desc->render_distance * 10.0,
        .max_distance = desc->render_distance * 100.0,
        .shadow_scale = 4.0,
        .inv_sun_dir = em_mul_vec3_f(desc->base_sun_dir, -1.0)
    });
    ENGINE_LOG_OK("Setup render system.\n", NULL);

    load_sys_init(&engine->_load_sys, &(load_system_desc_t) {
        .render_dist = desc->render_distance,
        .start_pos = IVEC2(0, 0)
    });
    ENGINE_LOG_OK("Setup load system.\n", NULL);

    tick_sys_init(&engine->_tick_sys, &(tick_system_desc_t) {
        .tps = desc->ticks_per_second
    });
    ENGINE_LOG_OK("Setup tick system.\n", NULL);

    engine->meta = (engine_meta_t) {
        .cursor = {
            .active = false,
            .cell = IVEC3(-1, -1, -1),
            .chunk = IVEC2(0, 0),
            .face = -1,
            .range = -1
        },
        .world = {
            .base_sun_dir = desc->base_sun_dir,
            .time = 0,
            .max_time = desc->max_time
        },
    };
    // Font is 11x13 so this is a pixel perfect scale.
    const vec2 DEBUG_TEXT_SIZE = em_mul_vec2_f(VEC2(11.0, 13.0), 2.0);
    sprite_t **tmp;
    tmp = sprite_renderer_push_str(&engine->_render_sys.sprite_renderer, 
                                              "00000 00000 00000", 
                                              &(sprite_desc_t) {
        .bg_col = VEC4(0.0, 0.0, 0.0, 0.5),
        .pos = em_mul_vec2_f(engine->_render_sys.sprite_renderer.base.dimensions, -0.5),
        .size = DEBUG_TEXT_SIZE,
        .z_index = 1.0,
        .is_char = true
    });
    memcpy(engine->meta.debug.cur_sprites, tmp, 17 * sizeof(sprite_t *));
    free(tmp);
 
    ENGINE_LOG_OK("Setup engine metadata.\n", NULL);
}

void _api_start_running(engine_t *engine, const engine_run_desc_t *desc)
{
    engine->meta.world.seed = desc->seed;
    engine->meta.world.name = desc->world_name;
    engine->meta.world.time = desc->time;

    if (desc->world_name)
        multicat(engine->_chunk_sys.world_dir_path, 2, WORLD_DATA_DIR, desc->world_name);

    engine->_chunk_sys.seed = desc->seed;
    engine->_render_sys.cam.pos = desc->cam_pos;
    engine->_render_sys.cam.rot = desc->cam_rot;
    engine->_load_sys.curr_pos = IVEC2(
        floorf(engine->_render_sys.cam.pos.x / (float) CHUNK_SIZE) * CHUNK_SIZE, 
        floorf(engine->_render_sys.cam.pos.z / (float) CHUNK_SIZE) * CHUNK_SIZE
    );
    ENGINE_LOG_OK("Initialize world parameters.\n", NULL);

    update_sys_init_thread(&engine->_update_sys, &(update_system_thread_args_t) {
        .us = &engine->_update_sys
    });
    chunk_sys_init_thread(&engine->_chunk_sys, &(chunk_system_thread_args_t) {
        .cs = &engine->_chunk_sys,
        .us = &engine->_update_sys
    });
    ENGINE_LOG_OK("Start generation threads.\n", NULL);

    load_sys_load_initial(&engine->_load_sys, &engine->_chunk_sys);
    ENGINE_LOG_OK("Request initial chunks.\n", NULL);

    chunk_sys_await_initial_load_complete(&engine->_chunk_sys);
    ENGINE_LOG_OK("Initial chunks loaded.\n", NULL);

    tick_sys_init_thread(&engine->_tick_sys, &(tick_system_thread_args_t) {
        .ts = &engine->_tick_sys,
    });
    ENGINE_LOG_OK("Start tick thread.\n", NULL);

    atomic_store(&engine->_running, true);
}

void engine_init(engine_t *engine)
{
    engine->api.subscribe_to_event = _api_subscribe_to_event;
    engine->api.edit_active_block = _api_edit_active_block;
    engine->api.init_systems = _api_init_systems;
    engine->api.start_running = _api_start_running;
    ENGINE_LOG_OK("Setup engine api.\n", NULL);
}

void engine_cleanup(engine_t *engine)
{
    render_sys_cleanup(&engine->_render_sys);
    tick_sys_cleanup(&engine->_tick_sys);
    event_sys_cleanup(&engine->_event_sys);
    load_sys_cleanup(&engine->_load_sys);
    chunk_sys_cleanup(&engine->_chunk_sys);
    update_sys_cleanup(&engine->_update_sys);
}

void engine_event(engine_t *engine, const event_t *event)
{
    event_sys_get_event(&engine->_event_sys, event);
}

void engine_frame(engine_t *engine, double dt)
{
    render_sys_render(&engine->_render_sys, &engine->_update_sys, &engine->_load_sys);

    atomic_fetch_add(&engine->_tick_sys.cum_dt, dt);

    event_sys_new_frame(&engine->_event_sys);
}

void engine_tick(engine_t *engine)
{
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

        char buf[18];
        snprintf(buf, 18, "%05i %05i %05i", 
                 (int) global_pos.x, (int) global_pos.y, (int) global_pos.z);
        sprite_renderer_change_str(&engine->_render_sys.sprite_renderer,
                                   engine->meta.debug.cur_sprites,
                                   buf);
        sprite_renderer_move_str(&engine->_render_sys.sprite_renderer,
                                 engine->meta.debug.cur_sprites, 17,
                                 em_mul_vec2_f(engine->_render_sys.sprite_renderer.base.dimensions, -0.5));
    } 
    else 
    {
        engine->_render_sys.cursor_active = false;
    }

    // Set tick duration and reset count.
    engine->_tick_sys.dt = atomic_exchange(&engine->_tick_sys.cum_dt, 0.0);

    // Progress world time, set sun direction.
    uint64_t time = atomic_fetch_add(&engine->meta.world.time, 1);
    uint64_t max = engine->meta.world.max_time;
    float t = (float) (time % max) / max;
    vec3 sun_dir = em_rotate_vec3(engine->meta.world.base_sun_dir, t * 360.0, VEC3(1.0, 0.1, 0.2));
    engine->_render_sys.chunk_renderer.info.sun_dir = sun_dir;
}
