#include "app.h"

static void _on_mousedown(const event_t *ev, void *args) 
{
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

static void do_init(engine_t *e, app_t *a)
{
    APP_LOG_WARN("Init called.\n", NULL);
    // TODO: Try with smaller render distance (out of buffers silently??)
    APP_TODO("Make the render distance editable at runtime");
    #if defined(RELEASE) || defined(PROFILING)
        const size_t RENDER_DISTANCE = 24;
    #elif defined(DEBUG)
        const size_t RENDER_DISTANCE = 12;
    #endif

    load_model_files();

    e->api.init_systems(e, &(engine_desc_t) {
        .render_distance = RENDER_DISTANCE,
        .ticks_per_second = 20.0,
        .max_time = 24000, // 10 minute cycle @ 20tps
        .base_sun_dir = em_normalize_vec3(VEC3(0.0, 1.0, 0.1)),
        .gen_func = generate_chunk_data
    });

    ctl_init(&a->camera_ctl, &e->_render_sys.cam, &(ctl_desc_t) {
        .floor_friction = 0.55,
        .air_friction   = 0.02,
        .jump_accel     = 640.0,
        .run_accel      = 600.0,
        .walk_accel     = 400.0,
        .air_accel      = 10.0,
        .turn_speed     = 0.04,
        .gravity        = -38.0,
        .collider_size  = VEC3(0.3, 1.7, 0.3),
        .max_fall_velo  = -20.0
    });

    e->meta.cursor.range = 10.0;

    a->mousedown_args = (struct md_args) {
        .app = a,
        .engine = e
    };
    e->api.subscribe_to_event(e, EVENT_MOUSEDOWN, &(event_subscriber_desc_t) {
        .event_cb = _on_mousedown,
        .block_cb = event_block_never,
        .args = &a->mousedown_args
    });
}

void app_init(engine_t *e, app_t *a, const app_desc_t *desc)
{
    // If showing help message, no need to init the engine or the app.
    parsed_args_t pargs;
    memset(&pargs, 0, sizeof(pargs));
    if (process_args(&desc->args, "a", &pargs))
    {
        if (args_match(&pargs, ARGTYPE_ACTION, "help")) world_print_help();
        else RUNTIME_ASSERT(false, "Invalid arguments. Run with help command for information");

        // Sokol tries to open a window during the entrypoint, this stops it from flashing on the screen.
        exit(0);
    }

    if (desc->args.argc == 1)
    {
        APP_LOG_OK("Running in default mode", NULL);
        em_romu_duo_state_t s;
        em_romu_duo_init(&s, time(NULL));
        do_init(e, a);
        e->api.start_running(e, &(engine_run_desc_t) {
            .world_name = NULL,
            .seed = em_romu_duo_random(&s),
            .time = 0,
            .cam_pos = VEC3(0.0, 128.0, 0.0),
            .cam_rot = QUAT(0.0, 0.0, 0.0, 1.0)
        });

        return;
    }

    if (process_args(&desc->args, "atp", &pargs))
    {
        APP_LOG_OK("Running in load/delete mode", NULL);
        int32_t name_ix = args_index_of(&pargs, ARGTYPE_TACK, "-n");
        if (name_ix == -1) name_ix = args_index_of(&pargs, ARGTYPE_TACK, "--name");
        const char *name = arg_at(&pargs, ARGTYPE_PARAM, name_ix);

        RUNTIME_ASSERT((name), "Name incorrectly specified for load");

        if (args_match(&pargs, ARGTYPE_ACTION, "load"))
        {
            do_init(e, a);
            world_load(e, name);
        }
        else if (args_match(&pargs, ARGTYPE_ACTION, "delete"))
        {
            world_delete(e, name);
            exit(0);
        }
        else RUNTIME_ASSERT(false, "Invalid arguments. Run with help command for information");
        return;
    }

    if (process_args(&desc->args, "atptp", &pargs))
    {
        APP_LOG_OK("Running in new/rename mode", NULL);
        int32_t name_ix = args_index_of(&pargs, ARGTYPE_TACK, "-n");
        if (name_ix == -1) name_ix = args_index_of(&pargs, ARGTYPE_TACK, "--name");
        const char *name = arg_at(&pargs, ARGTYPE_PARAM, name_ix);

        if (args_match(&pargs, ARGTYPE_ACTION, "new"))
        {
            APP_LOG_OK("Matched new\n", NULL);
            int32_t seed_ix = args_index_of(&pargs, ARGTYPE_TACK, "-s");
            if (seed_ix == -1) seed_ix = args_index_of(&pargs, ARGTYPE_TACK, "--seed");
            const char *seed = arg_at(&pargs, ARGTYPE_PARAM, seed_ix);
            uint32_t seed_ui = strtoul(seed, NULL, 10);
            RUNTIME_ASSERT((name), "Name incorrectly specified for new");
            RUNTIME_ASSERT((seed), "Seed incorrectly specified for new");

            do_init(e, a);
            world_new(e, name, seed_ui);
        }
        else if (args_match(&pargs, ARGTYPE_ACTION, "rename"))
        {
            APP_LOG_OK("Matched rename\n", NULL);
            int32_t newname_ix = args_index_of(&pargs, ARGTYPE_TACK, "-nn");
            if (newname_ix == -1) newname_ix = args_index_of(&pargs, ARGTYPE_TACK, "--newname");
            const char *newname = arg_at(&pargs, ARGTYPE_PARAM, newname_ix);
            RUNTIME_ASSERT((name), "Name incorrectly specified for rename");
            RUNTIME_ASSERT((newname), "New name incorrectly specified for rename");
            world_rename(e, name, newname);
            exit(0);
        }
        else RUNTIME_ASSERT(false, "Invalid arguments. Run with help command for information");
        return;
    }

    RUNTIME_ASSERT(false, "Invalid arguments. Run with help command for information");
}

void app_cleanup(app_t *app)
{
    unload_model_files();
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
