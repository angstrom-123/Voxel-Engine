#include "app.h"

static const float HB_SCALE = 5.0;
static const vec2 HB_ORIGIN = VEC2(-SPRITE_SIZE.x * HB_SCALE * HOTBAR_SIZE / 2.0,
                -SCREEN_HEIGHT / 2.0 + SPRITE_SIZE.y * HB_SCALE);

static void _show_menu(app_t *app, ui_system_t *uis, menu_e menu, bool show)
{
    switch (menu) {
    case MENU_MAIN:
        for (size_t i = 0; i < MAINMENUCOMP_NUM; i++)
            ui_sys_show_component(uis, app->main_menu.comps[i], show);
        break;
    case MENU_WORLD:
        for (size_t i = 0; i < WORLDMENUCOMP_NUM; i++)
            ui_sys_show_component(uis, app->world_menu.comps[i], show);
        break;
    case MENU_OPT:
        for (size_t i = 0; i < OPTMENUCOMP_NUM; i++)
            ui_sys_show_component(uis, app->options_menu.comps[i], show);
        break;
    default:
        ENGINE_ASSERT(false, "Unreachable");
        break;
    };
}

static bool _on_mousedown(const event_t *ev, void *args) 
{
    app_event_args_t *ev_args = args;
    engine_t *engine = ev_args->engine;
    app_t *app = ev_args->app;

    if (app->state != APP_IN_GAME) return false;

    switch (ev->mouse_button) {
    case MOUSE_BUTTON_LEFT:
        sapp_lock_mouse(true);
        engine_edit_active_block(engine, app->hotbar.types[app->hotbar.curr], BLOCK_ACTION_BREAK, NULL);
        break;
    case MOUSE_BUTTON_RIGHT:
    {
        aabb_t player_coll = aabb_with_offset(app->camera_ctl.collider, engine->_render_sys.cam.pos);
        engine_edit_active_block(engine, app->hotbar.types[app->hotbar.curr], BLOCK_ACTION_PLACE, &player_coll);
        break;
    }
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
        if (app->state != APP_IN_GAME)
            break;

        app->hotbar.curr = ev->keycode - KEYCODE_1;
        vec2 pos = HB_ORIGIN;
        pos.x += app->hotbar.curr * HB_SCALE * SPRITE_SIZE.x;
        sprite_renderer_move(&engine->_render_sys.sprite_renderer, app->hotbar.selected_sprite, pos);
        break;

    case KEYCODE_ESCAPE:
        if (app->state != APP_GAME_OPTIONS && app->state != APP_IN_GAME)
            break;

        switch (app->state) {
        case APP_IN_GAME:
            app->state = APP_GAME_OPTIONS;
            break;
        case APP_GAME_OPTIONS:
            app->state = APP_IN_GAME;
            break;
        default:
            break;
        };

        sapp_lock_mouse(app->state != APP_GAME_OPTIONS);
        _show_menu(app, &engine->_ui_sys, MENU_OPT, app->state == APP_GAME_OPTIONS);
        break;

    default:
        break;
    }

    return false;
}

static void _return_button_clicked(ui_component_t *comp, void *args)
{
    APP_LOG_OK("Return clicked", NULL);
    (void) comp;
    app_event_args_t *ev_args = args;
    app_t *app = ev_args->app;
    engine_t *engine = ev_args->engine;

    RUNTIME_ASSERT(app->state == APP_GAME_OPTIONS, "Wrong app state for this action");

    app->state = APP_IN_GAME;
    sapp_lock_mouse(true);
    _show_menu(app, &engine->_ui_sys, MENU_OPT, false);
}

static void _main_menu_worlds_clicked(ui_component_t *uic, void *args)
{
    APP_LOG_OK("Worlds clicked", NULL);
    (void) uic;
    app_event_args_t *ev_args = args;
    app_t *a = ev_args->app;
    engine_t *e = ev_args->engine;

    _show_menu(a, &e->_ui_sys, MENU_MAIN, false);
    a->state = APP_WORLDS_MENU;
    _show_menu(a, &e->_ui_sys, MENU_WORLD, true);
}

static void _main_menu_quit_clicked(ui_component_t *uic, void *args)
{
    APP_LOG_OK("Menu quit clicked", NULL);
    (void) uic;
    (void) args;
    sapp_request_quit();
}

static void _world_menu_new_clicked(ui_component_t *uic, void *args)
{
    APP_LOG_OK("New clicked", NULL);
    (void) uic;
    app_event_args_t *ev_args = args;
    app_t *a = ev_args->app;
    engine_t *e = ev_args->engine;

    if (a->world_menu.new_clicked)
        return;

    a->world_menu.new_clicked = true;

    a->state = APP_IN_GAME;
    for (size_t i = 0; i < HOTBAR_SIZE * 2; i++)
        a->hotbar.sprites[i]->visible = true;
    a->hotbar.selected_sprite->visible = true;

    _show_menu(a, &e->_ui_sys, MENU_WORLD, false);

    sapp_lock_mouse(true);

    em_romu_duo_state_t s;
    em_romu_duo_init(&s, time(NULL));
    engine_run(e, &(engine_run_desc_t) {
        .world_name = NULL,
        .seed = em_romu_duo_random(&s),
        .time = 0,
        .cam_pos = VEC3(0.0, 128.0, 0.0),
        .cam_rot = QUAT(0.0, 0.0, 0.0, 1.0)
    });
}

void _world_menu_load_clicked(ui_component_t *uic, void *args)
{
    APP_LOG_OK("Load clicked", NULL);
    (void) uic;
    (void) args;
}

void app_init(engine_t *e, app_t *a, const app_desc_t *desc)
{
    (void) desc;
    #if defined(RELEASE) || defined(PROFILING)
        const size_t RENDER_DISTANCE = 24;
    #elif defined(DEBUG)
        const size_t RENDER_DISTANCE = 12;
    #endif

    load_model_files();

    ui_sys_init_handle_buffer(OPTMENUCOMP_NUM, &a->options_menu.comps[0]);
    ui_sys_init_handle_buffer(MAINMENUCOMP_NUM, &a->main_menu.comps[0]);
    ui_sys_init_handle_buffer(WORLDMENUCOMP_NUM, &a->world_menu.comps[0]);

    a->state = APP_MAIN_MENU;

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

    engine_init(e, &(engine_desc_t) {
        .render_distance = RENDER_DISTANCE,
        .ticks_per_second = 20.0,
        .max_time = 24000, // 10 minute cycle @ 20tps
        .base_sun_dir = em_normalize_vec3(VEC3(0.0, 1.0, 0.1)),
        .gen_func = generate_chunk_data,
        .init_cursor_line_renderer = true,
        .init_sprite_renderer = true,
        .init_chunk_renderer = true
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
    event_sys_subscribe_to_event(&e->_event_sys, EVENT_MOUSEDOWN, &(event_subscriber_desc_t) {
        .event_cb = _on_mousedown,
        .block_cb = event_block_never,
        .args = &a->ev_args
    });

    event_sys_subscribe_to_event(&e->_event_sys, EVENT_KEYDOWN, &(event_subscriber_desc_t) {
        .event_cb = _on_keydown,
        .block_cb = event_block_never,
        .args = &a->ev_args
    });

    // Initialize hotbar sprites
    sprite_desc_t hb_desc = {
        .z_index = 1.0,
        .scale = HB_SCALE,
        .pos = HB_ORIGIN,
        .visible = false,
    };
    a->hotbar.sprites = malloc(HOTBAR_SIZE * 2 * sizeof(sprite_t *));
    for (size_t i = 0; i < HOTBAR_SIZE; i++)
    {
        hb_desc.bg_col = VEC4(0.0, 0.0, 0.0, 0.5);
        hb_desc.uv_offset = sprite_icon_uv_offset(&e->_render_sys.sprite_renderer, IID_SLOT);
        a->hotbar.sprites[i] = sprite_renderer_push(&e->_render_sys.sprite_renderer, &hb_desc);

        hb_desc.bg_col = VEC4(0.0, 0.0, 0.0, 0.0);
        hb_desc.uv_offset = sprite_icon_uv_offset(&e->_render_sys.sprite_renderer, (icon_id_e) a->hotbar.types[i]);
        a->hotbar.sprites[i + HOTBAR_SIZE] = sprite_renderer_push(&e->_render_sys.sprite_renderer, &hb_desc);

        hb_desc.pos.x += HB_SCALE * SPRITE_SIZE.x;
    }

    hb_desc.z_index = 2.0;
    hb_desc.pos.x = HB_ORIGIN.x + a->hotbar.curr * HB_ORIGIN.x;
    hb_desc.uv_offset = sprite_icon_uv_offset(&e->_render_sys.sprite_renderer, IID_SLOT_SELECTED);
    a->hotbar.selected_sprite = sprite_renderer_push(&e->_render_sys.sprite_renderer, &hb_desc);

    const vec2 ui_size = VEC2(UI_DIM.x * SPRITE_SIZE.x * UI_SCALE, 
                              UI_DIM.y * SPRITE_SIZE.y * UI_SCALE);

    // Initialize options menu sprites
    a->options_menu.comps[OPTMENUCOMP_L_TITLE] = ui_sys_make_label(&e->_ui_sys, &(ui_label_desc_t) {
        .pos = VEC2(-11.0 * 3.0 * 4.0, TITLE_HEIGHT),
        .text_style = {
            .scale = TITLE_SCALE,
            .z_index = 4.0,
            .bg_col = VEC4(0.0, 0.0, 0.0, 0.0)
        },
        .visible = false,
        .text = "Game Menu"
    });

    a->options_menu.comps[OPTMENUCOMP_B_QUIT] = ui_sys_make_button(&e->_ui_sys, &(ui_button_desc_t) {
        .pos = VEC2(-ui_size.x / 2.0, TITLE_HEIGHT - ui_size.y - UI_SPACING.y),
        .dim = UI_DIM,
        .body_style = {
            .scale = UI_SCALE,
            .z_index = 3.0,
            .bg_col = BG_COL,
            .hover_bg_col = HOVER_COL,
        },
        .text_style = {
            .scale = TEXT_SCALE,
            .z_index = 4.0
        },
        .text = "Save and Quit",
        .cb = _return_button_clicked
    });

    a->options_menu.comps[OPTMENUCOMP_B_RETURN] = ui_sys_make_button(&e->_ui_sys, &(ui_button_desc_t) {
        .pos = VEC2(-ui_size.x / 2.0, TITLE_HEIGHT - (ui_size.y + UI_SPACING.y) * 2.0),
        .dim = UI_DIM,
        .body_style = {
            .scale = UI_SCALE,
            .z_index = 3.0,
            .bg_col = BG_COL,
            .hover_bg_col = HOVER_COL
        },
        .text_style = {
            .scale = TEXT_SCALE,
            .z_index = 4.0
        },
        .text = "Back to Game",
        .cb = _return_button_clicked,
        .cb_args = &a->ev_args
    });

    a->options_menu.comps[OPTMENUCOMP_SLIDER] = ui_sys_make_slider(&e->_ui_sys, &(ui_slider_desc_t) {
        .pos = VEC2(-ui_size.x / 2.0, TITLE_HEIGHT - (ui_size.y + UI_SPACING.y) * 3.0),
        .width = UI_DIM.x,
        .body_style = {
            .scale = UI_SCALE,
            .bg_col = BG_COL,
            .hover_bg_col = HOVER_COL,
            .z_index = 3.0
        },
        .text_style = {
            .scale = TEXT_SCALE,
            .z_index = 4.0
        },
        .text = "Slider:"
    });

    a->options_menu.comps[OPTMENUCOMP_INPUT] = ui_sys_make_input(&e->_ui_sys, &(ui_input_desc_t) {
        .pos = VEC2(-ui_size.x / 2.0, TITLE_HEIGHT - (ui_size.y + UI_SPACING.y) * 4.0),
        .width = UI_DIM.x,
        .body_style = {
            .scale = UI_SCALE,
            .bg_col = INPUT_BG_COL,
            .hover_bg_col = INPUT_BG_COL,
            .z_index = 3.0
        },
        .text_style = {
            .scale = TEXT_SCALE,
            .z_index = 4.0
        },
        .max_len = 20,
    });

    // Init main menu sprites
    a->main_menu.comps[MAINMENUCOMP_L_TITLE] = ui_sys_make_label(&e->_ui_sys, &(ui_label_desc_t) {
        .pos = VEC2(-11.0 * 3.0 * 5.5, TITLE_HEIGHT),
        .text_style = {
            .scale = TITLE_SCALE,
            .z_index = 4.0,
            .bg_col = VEC4(0.0, 0.0, 0.0, 0.0)
        },
        .visible = true,
        .text = "Voxel Engine"
    });

    a->main_menu.comps[MAINMENUCOMP_B_WORLDS] = ui_sys_make_button(&e->_ui_sys, &(ui_button_desc_t) {
        .pos = VEC2(-ui_size.x - UI_SPACING.x, TITLE_HEIGHT - ui_size.y - UI_SPACING.y),
        .dim = UI_DIM,
        .body_style = {
            .scale = UI_SCALE,
            .z_index = 3.0,
            .bg_col = BG_COL,
            .hover_bg_col = HOVER_COL,
        },
        .text_style = {
            .scale = TEXT_SCALE,
            .z_index = 4.0
        },
        .visible = true,
        .text = "My Worlds",
        .cb = _main_menu_worlds_clicked,
        .cb_args = &a->ev_args
    });

    a->main_menu.comps[MAINMENUCOMP_B_QUIT] = ui_sys_make_button(&e->_ui_sys, &(ui_button_desc_t) {
        .pos = VEC2(UI_SPACING.x, TITLE_HEIGHT - ui_size.y - UI_SPACING.y),
        .dim = UI_DIM,
        .body_style = {
            .scale = UI_SCALE,
            .z_index = 3.0,
            .bg_col = BG_COL,
            .hover_bg_col = HOVER_COL,
        },
        .text_style = {
            .scale = TEXT_SCALE,
            .z_index = 4.0
        },
        .visible = true,
        .text = "Exit to Desktop",
        .cb = _main_menu_quit_clicked,
        .cb_args = &a->ev_args
    });

    // Init world menu sprites
    a->world_menu.comps[WORLDMENUCOMP_L_TITLE] = ui_sys_make_label(&e->_ui_sys, &(ui_label_desc_t) {
        .pos = VEC2(-11.0 * 3.0 * 13.0, TITLE_HEIGHT),
        .text_style = {
            .scale = TITLE_SCALE,
            .z_index = 4.0,
            .bg_col = VEC4(0.0, 0.0, 0.0, 0.0)
        },
        .text = "New World       Load World"
    });

    a->world_menu.comps[WORLDMENUCOMP_I_NAME] = ui_sys_make_input(&e->_ui_sys, &(ui_input_desc_t) {
        .pos = VEC2(-ui_size.x - UI_SPACING.x, TITLE_HEIGHT - (ui_size.y + UI_SPACING.y)),
        .width = UI_DIM.x,
        .body_style = {
            .scale = UI_SCALE,
            .z_index = 3.0,
            .bg_col = INPUT_BG_COL,
            .hover_bg_col = INPUT_BG_COL,
        },
        .text_style = {
            .scale = TEXT_SCALE,
            .z_index = 4.0,
            .hover_bg_col = VEC4(1.0, 1.0, 1.0, 1.0),
        },
        .max_len = 20
    });

    a->world_menu.comps[WORLDMENUCOMP_I_SEED] = ui_sys_make_input(&e->_ui_sys, &(ui_input_desc_t) {
        .pos = VEC2(-ui_size.x - UI_SPACING.x, TITLE_HEIGHT - (ui_size.y + UI_SPACING.y) * 2.0),
        .width = UI_DIM.x,
        .body_style = {
            .scale = UI_SCALE,
            .z_index = 3.0,
            .bg_col = INPUT_BG_COL,
            .hover_bg_col = INPUT_BG_COL,
        },
        .text_style = {
            .scale = TEXT_SCALE,
            .z_index = 4.0,
            .hover_bg_col = VEC4(1.0, 1.0, 1.0, 1.0),
        },
        .max_len = 20,
        .filter = FILTER_NUMER
    });

    a->world_menu.comps[WORLDMENUCOMP_B_CREATE] = ui_sys_make_button(&e->_ui_sys, &(ui_button_desc_t) {
        .pos = VEC2(-ui_size.x - UI_SPACING.x, TITLE_HEIGHT - (ui_size.y + UI_SPACING.y) * 3.0),
        .dim = UI_DIM,
        .body_style = {
            .scale = UI_SCALE,
            .z_index = 3.0,
            .bg_col = BG_COL,
            .hover_bg_col = HOVER_COL,
        },
        .text_style = {
            .scale = TEXT_SCALE,
            .z_index = 4.0
        },
        .text = "New World",
        .cb = _world_menu_new_clicked,
        .cb_args = &a->ev_args
    });

    file_t world_dir = {
        .path = WORLD_DATA_DIR,
        .flags = FILEFLAG_DIR,
    };
    size_t max_names = 5;
    char names[max_names][STD_BUFLEN];
    file_list_dir(&world_dir, &max_names, names);
    for (size_t i = 0; i < max_names; i++)
    {
        ui_button_desc_t desc = {
            .pos = VEC2(UI_SPACING.x, TITLE_HEIGHT - (ui_size.y + UI_SPACING.y) * (i + 1)),
            .dim = UI_DIM,
            .body_style = {
                .scale = UI_SCALE,
                .z_index = 3.0,
                .bg_col = BG_COL,
                .hover_bg_col = HOVER_COL,
            },
            .text_style = {
                .scale = TEXT_SCALE,
                .z_index = 4.0
            },
            .cb = _world_menu_load_clicked,
            .cb_args = &a->ev_args
        };

        strncpy(desc.text, names[i], UI_BUFLEN);
        a->world_menu.comps[WORLDMENUCOMP_B_LOAD1 + i] = ui_sys_make_button(&e->_ui_sys, &desc);
    }

    engine_do_render(e);
}

void app_cleanup(app_t *app)
{
    free(app->hotbar.sprites);
    unload_model_files();
    ctl_cleanup(&app->camera_ctl);
}

void app_frame(engine_t *engine, app_t *app, double dt) 
{
    INSTRUMENT_FUNC_BEGIN();

    ctl_update_surrounding(&app->camera_ctl, &engine->_render_sys.cam, &engine->_chunk_sys);
    ctl_update_pos(&app->camera_ctl, &engine->_render_sys.cam, &engine->_event_sys, dt, app->state == APP_IN_GAME);
    ctl_update_view(&app->camera_ctl, &engine->_render_sys.cam, &engine->_event_sys, app->state == APP_IN_GAME);

    cam_update(&engine->_render_sys.cam);

    INSTRUMENT_FUNC_END();
}

void app_tick(engine_t *engine, app_t *app)
{
    (void) engine;
    (void) app;
}
