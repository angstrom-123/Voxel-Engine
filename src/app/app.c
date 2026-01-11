#include "app.h"

static void _on_mousedown(const event_t *ev, void *args) 
{
    // engine_t *engine = args;
    struct md_args *mdargs = args;
    engine_t *engine = mdargs->engine;
    app_t *app = mdargs->app;

    const player_collider_desc_t desc = {
        .pos = engine->_render_sys.cam.pos,
        .collider = app->camera_ctl.collider
    };

    switch (ev->mouse_button) {
    case MOUSE_BUTTON_LEFT:
        engine->api.edit_active_block(engine, BLOCK_ACTION_BREAK, &desc);
        break;
    case MOUSE_BUTTON_RIGHT:
        engine->api.edit_active_block(engine, BLOCK_ACTION_PLACE, &desc);
        break;
    case MOUSE_BUTTON_MIDDLE:
        break;
    default:
        break;
    };
}

static bool streq(const char *s1, const char *s2)
{
    return strcmp(s1, s2) == 0;
}

static void _show_cli(engine_t *engine, app_t *app, int argc, char **argv)
{
    const char *ERROR_MESSAGE = "Invalid arguments. Use the help command for information";

    if (argc == 1) // Default run in temporary world.
    {
        engine->api.start_running(engine, &(engine_run_desc_t) {
            .world_name = "default",
            .seed = 0,
            .time = 0,
            .cam_pos = VEC3(0.0, 100.0, 0.0),
            .cam_rot = QUAT(0.0, 0.0, 0.0, 1.0)
        });
        return;
    }

    if (argc == 2 && streq("help", argv[1])) // Show help.
    {
        world_print_help();
        sapp_request_quit();
        return;
    }

    if (argc == 4) // Either "load" or "delete"
    {
        if (streq("load", argv[1]))
        {
            if (streq("-n", argv[2]) || streq("--name", argv[2]))
            {
                const char *world_name = argv[3];
                world_load(engine, world_name);
                return;
            }
        }
        else if (streq("delete", argv[1]))
        {
            if (streq("-n", argv[2]) || streq("--name", argv[2]))
            {
                const char *world_name = argv[3];
                world_delete(engine, world_name);
                return;
            }
        }

        sapp_request_quit();
        _LOG("RUNTIME", COL_ERR, "%s", ERROR_MESSAGE);
        return;
    }

    if (argc == 6) // Either "new" or "rename"
    {
        if (streq("new", argv[1]))
        {
            const char *world_name;
            const char *world_seed;

            if (streq("-n", argv[2]) || streq("--name", argv[2]))
            {
                world_name = argv[3];
                if (streq("-s", argv[4]) || streq("--seed", argv[4]))
                {
                    world_seed = argv[5];
                    world_new(engine, world_name, strtoul(world_seed, NULL, 10));
                    return;
                }
            }
            else if (streq("-s", argv[2]) || streq("--seed", argv[2]))
            {
                world_seed = argv[3];
                if (streq("-n", argv[4]) || streq("--name", argv[4]))
                {
                    world_name = argv[5];
                    world_new(engine, world_name, strtoul(world_seed, NULL, 10));
                    return;
                }
            }
        }
        else if (streq("rename", argv[1]))
        {
            const char *world_name;
            const char *new_name;

            if (streq("-n", argv[2]) || streq("--name", argv[2]))
            {
                world_name = argv[3];
                if (streq("-nn", argv[4]) || streq("--newname", argv[4]))
                {
                    new_name = argv[5];
                    world_rename(engine, world_name, new_name);
                    return;
                }
            }
            else if (streq("-nn", argv[2]) || streq("--newname", argv[2]))
            {
                new_name = argv[3];
                if (streq("-n", argv[4]) || streq("--name", argv[4]))
                {
                    world_name = argv[5];
                    world_rename(engine, world_name, new_name);
                    return;
                }
            }
        }
    }

    _LOG("RUNTIME", COL_ERR, "%s", ERROR_MESSAGE);
    return;
}

void app_init(engine_t *engine, app_t *app, const app_desc_t *desc)
{
    APP_TODO("Make the render distance editable at runtime");
    #if defined(RELEASE) || defined(PROFILING)
        const size_t RENDER_DISTANCE = 32;
    #elif defined(DEBUG)
        const size_t RENDER_DISTANCE = 12;
    #endif

    for (int i = 0; i < desc->argc; i++)
        APP_LOG_OK("%s", desc->argv[i]);

    engine_init(engine, &(engine_desc_t) {
        .render_distance = RENDER_DISTANCE,
        .ticks_per_second = 20.0,
        .seed = 0,
        .max_time = 24000, // 20 minute cycle @ 20tps
        .base_sun_dir = em_normalize_vec3(VEC3(0.0, 1.0, 0.1))
    });

    app->mousedown_args = (struct md_args) {
        .app = app,
        .engine = engine
    };
    app->needs_physics_update = false;
    ctl_init(&app->camera_ctl, &engine->_render_sys.cam, &(ctl_desc_t) {
        // .start_pos      = VEC3(8.5, 100.0, 8.5),
        .floor_friction = 0.55,
        .air_friction   = 0.02,
        .jump_accel     = 650.0,
        .run_accel      = 600.0,
        .walk_accel     = 400.0,
        .air_accel      = 10.0,
        .turn_speed     = 0.04,
        .gravity        = -35.0,
        .collider_size  = VEC3(0.3, 1.8, 0.3)
    });

    engine->api.subscribe_to_event(engine, EVENT_MOUSEDOWN, &(event_subscriber_desc_t) {
        .event_cb = _on_mousedown,
        .block_cb = event_block_never,
        .args = &app->mousedown_args
    });

    engine->meta.cursor.range = 10.0;

    _show_cli(engine, app, desc->argc, desc->argv);

    // engine->api.start_running(engine, &(engine_run_desc_t) {
    //     .world_name = "default",
    //     .seed = 0,
    //     .time = 0,
    //     .cam_pos = VEC3(0.0, 100.0, 0.0),
    //     .cam_rot = QUAT(0.0, 0.0, 0.0, 1.0)
    // });
}

void app_cleanup(app_t *app)
{
    ctl_cleanup(&app->camera_ctl);
}

void app_frame(engine_t *engine, app_t *app, double dt) 
{
    INSTRUMENT_FUNC_BEGIN();

    ctl_update_surrounding(&app->camera_ctl, &engine->_render_sys.cam, &engine->_chunk_sys);
    ctl_update_pos(&app->camera_ctl, &engine->_render_sys.cam, &engine->_event_sys, dt);
    ctl_update_view(&app->camera_ctl, &engine->_render_sys.cam, &engine->_event_sys);

    cam_update(&engine->_render_sys.cam);

    INSTRUMENT_FUNC_END();
}

void app_tick(engine_t *engine, app_t *app)
{
    (void) engine; (void) app;
}
