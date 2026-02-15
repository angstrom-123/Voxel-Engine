#include "app.h"

static const float HB_SCALE = 5.0;
static const vec2 HB_ORIGIN = VEC2(-SPRITE_SIZE.x * HB_SCALE * HOTBAR_SIZE / 2.0,
                -SCREEN_HEIGHT / 2.0 + SPRITE_SIZE.y * HB_SCALE);

static bool _on_mousedown(const event_t *ev, void *args) 
{
    app_event_args_t *ev_args = args;
    engine_t *engine = ev_args->engine;
    app_t *app = ev_args->app;

    if (app->menu.active) return false;

    switch (ev->mouse_button) {
    case MOUSE_BUTTON_LEFT:
        sapp_lock_mouse(true);
        engine->api.edit_active_block(engine, app->hotbar.types[app->hotbar.curr], 
                                      BLOCK_ACTION_BREAK, NULL);
        break;
    case MOUSE_BUTTON_RIGHT:
        engine->api.edit_active_block(engine, app->hotbar.types[app->hotbar.curr], 
                                      BLOCK_ACTION_PLACE, &(player_collider_desc_t) {
            .pos = engine->_render_sys.cam.pos,
            .collider = app->camera_ctl.collider
        });
        break;
    default:
        break;
    };
    
    return false;
}

static bool _on_keydown(const event_t *ev, void *args)
{
    app_event_args_t *ev_args = args;
    app_t *app = ev_args->app;
    engine_t *engine = ev_args->engine;
    switch (ev->keycode) {
    case KEYCODE_1: case KEYCODE_2: case KEYCODE_3: case KEYCODE_4: case KEYCODE_5:
    case KEYCODE_6: case KEYCODE_7: case KEYCODE_8: case KEYCODE_9:
        if (app->menu.active)
            break;

        app->hotbar.curr = ev->keycode - KEYCODE_1;
        vec2 selected_pos = HB_ORIGIN;
        selected_pos.x += app->hotbar.curr * HB_SCALE * SPRITE_SIZE.x;
        engine->api.move_sprite(engine, app->hotbar.selected_sprite, selected_pos);
        break;

    case KEYCODE_ESCAPE:
        app->menu.active = !app->menu.active;
        sapp_lock_mouse(!app->menu.active);
        for (size_t i = 0; i < MENUCOMP_NUM; i++)
            ui_sys_show_component(&engine->_ui_sys, app->menu.comps[i], app->menu.active);
        break;

    default:
        break;
    }

    // TODO: Change this for when handling input fields
    return false;
}

static void _quit_button_pressed(ui_component_t *comp, void *args)
{
    (void) comp;
    (void) args;
    sapp_request_quit();
}

static void _return_button_pressed(ui_component_t *comp, void *args)
{
    (void) comp;
    app_event_args_t *ev_args = args;
    app_t *app = ev_args->app;
    engine_t *engine = ev_args->engine;

    app->menu.active = false;
    sapp_lock_mouse(true);
    for (size_t i = 0; i < MENUCOMP_NUM; i++)
        ui_sys_show_component(&engine->_ui_sys, app->menu.comps[i], false);
}

static void _do_init(engine_t *e, app_t *a)
{
    APP_LOG_WARN("Init called.\n", NULL);
    APP_TODO("Make the render distance editable at runtime");
    #if defined(RELEASE) || defined(PROFILING)
        const size_t RENDER_DISTANCE = 24;
    #elif defined(DEBUG)
        const size_t RENDER_DISTANCE = 12;
    #endif

    load_model_files();

    a->hotbar = (struct hotbar) {
        .curr = 0,
        .types = {
            CUBETYPE_AIR,
            CUBETYPE_DIRT,
            CUBETYPE_GRASS,
            CUBETYPE_STONE,
            CUBETYPE_SAND,
            CUBETYPE_LOG,
            CUBETYPE_LEAF,
            CUBETYPE_LOG_P,
            CUBETYPE_LEAF_P
        }
    };

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

    a->ev_args = (app_event_args_t) {
        .app = a,
        .engine = e
    };
    e->api.subscribe_to_event(e, EVENT_MOUSEDOWN, &(event_subscriber_desc_t) {
        .event_cb = _on_mousedown,
        .block_cb = event_block_never,
        .args = &a->ev_args
    });
    e->api.subscribe_to_event(e, EVENT_KEYDOWN, &(event_subscriber_desc_t) {
        .event_cb = _on_keydown,
        .block_cb = event_block_never,
        .args = &a->ev_args
    });


    sprite_desc_t hb_desc = {
        .z_index = 1.0,
        .scale = HB_SCALE,
        .pos = HB_ORIGIN,
        .is_char = true,
        .visible = true
    };
    for (size_t i = 0; i < HOTBAR_SIZE; i++)
    {
        hb_desc.bg_col = VEC4(0.0, 0.0, 0.0, 0.5);
        e->api.place_icon_sprite(e, IID_SLOT, &hb_desc);
        e->api.place_icon_sprite(e, (icon_id_e) a->hotbar.types[i], &hb_desc);

        hb_desc.pos.x += HB_SCALE * SPRITE_SIZE.x;
    }

    hb_desc.z_index = 2.0;
    hb_desc.pos.x = HB_ORIGIN.x + a->hotbar.curr * HB_ORIGIN.x;
    hb_desc.bg_col = VEC4(0.0, 0.0, 0.0, 0.0);
    a->hotbar.selected_sprite = e->api.place_icon_sprite(e, IID_SLOT_SELECTED, &hb_desc);

    a->menu.comps[MENUCOMP_L_TITLE] = ui_sys_make_label(&e->_ui_sys, &(ui_label_desc_t) {
        .pos = VEC2(-11.0 * 3.0 * 4.0, 300.0),
        .text_style = {
            .scale = 3.0,
            .z_index = 4.0,
            .bg_col = VEC4(0.0, 0.0, 0.0, 0.0)
        },
        .visible = false,
        .text = "Game Menu"
    });

    a->menu.comps[MENUCOMP_B_QUIT] = ui_sys_make_button(&e->_ui_sys, &(ui_button_desc_t) {
        .pos = VEC2(-48.0 * 5.0, 100.0),
        .dim = UVEC2(10, 2),
        .body_style = {
            .scale = 3.0,
            .z_index = 3.0,
            .bg_col = VEC4(0.4, 0.4, 0.4, 1.0),
            .hover_bg_col = VEC4(0.6, 0.6, 0.6, 1.0)
        },
        .text_style = {
            .scale = 2.0,
            .z_index = 4.0
        },
        .text = "Save and Quit",
        .cb = _quit_button_pressed
    });

    a->menu.comps[MENUCOMP_B_RETURN] = ui_sys_make_button(&e->_ui_sys, &(ui_button_desc_t) {
        .pos = VEC2(-48.0 * 5.0, 195.0),
        .dim = UVEC2(10, 2),
        .body_style = {
            .scale = 3.0,
            .z_index = 3.0,
            .bg_col = VEC4(0.4, 0.4, 0.4, 1.0),
            .hover_bg_col = VEC4(0.6, 0.6, 0.6, 1.0)
        },
        .text_style = {
            .scale = 2.0,
            .z_index = 4.0
        },
        .text = "Back to Game",
        .cb = _return_button_pressed,
        .cb_args = &a->ev_args
    });

    a->menu.comps[MENUCOMP_SLIDER] = ui_sys_make_slider(&e->_ui_sys, &(ui_slider_desc_t) {
        .pos = VEC2(-48.0 * 5.0, 5.0),
        .width = 10,
        .body_style = {
            .scale = 3.0,
            .bg_col = VEC4(0.4, 0.4, 0.4, 1.0),
            .hover_bg_col = VEC4(0.6, 0.6, 0.6, 1.0),
            .z_index = 3.0
        },
        .text_style = {
            .scale = 2.0,
            .z_index = 4.0
        },
        .text = "Slider:"
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
        _do_init(e, a);
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
            _do_init(e, a);
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

            _do_init(e, a);
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
    ctl_update_pos(&app->camera_ctl, &engine->_render_sys.cam, &engine->_event_sys, dt, !app->menu.active);
    ctl_update_view(&app->camera_ctl, &engine->_render_sys.cam, &engine->_event_sys, !app->menu.active);

    cam_update(&engine->_render_sys.cam);

    INSTRUMENT_FUNC_END();
}

void app_tick(engine_t *engine, app_t *app)
{
    (void) engine; (void) app;
}
