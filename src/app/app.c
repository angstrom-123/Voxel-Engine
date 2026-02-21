#include "app.h"

static void _show_menu(app_t *app, ui_system_t *uis, enum menu menu, bool show)
{
    app->gui.menu.visible[menu] = show;
    switch (menu) {
    case MENU_NONE:
        break;
    case MENU_MAIN:
        for (size_t i = 0; i < MAINMENU_NUM; i++)
            ui_sys_show_component(uis, app->gui.menu.main[i], show);
        break;
    case MENU_LOAD:
        for (size_t i = 0; i < LOADMENU_NUM; i++)
            ui_sys_show_component(uis, app->gui.menu.load[i], show);
        break;
    case MENU_OPT:
        for (size_t i = 0; i < OPTMENU_NUM; i++)
            ui_sys_show_component(uis, app->gui.menu.options[i], show);
        break;
    case MENU_CREATE:
        for (size_t i = 0; i < CREATEMENU_NUM; i++)
            ui_sys_show_component(uis, app->gui.menu.create[i], show);
        break;
    case MENU_AUTHOR:
        for (size_t i = 0; i < AUTHORMENU_NUM; i++)
            ui_sys_show_component(uis, app->gui.menu.author[i], show);
        break;
    default:
        APP_ASSERT(false, "Unreachable");
        break;
    };
}

static void _switch_menu(app_t *app, ui_system_t *uis, enum menu menu)
{
    for (size_t i = 0; i < MENU_NUM; i++)
    {
        if (app->gui.menu.visible[i] && (enum menu) i != menu)
            _show_menu(app, uis, (enum menu) i, false);
    }

    app->gui.menu.active = menu;
    _show_menu(app, uis, menu, true);
}

static bool _on_mousedown(const event_t *ev, void *args) 
{
    app_event_args_t *ev_args = args;
    engine_t *engine = ev_args->engine;
    app_t *app = ev_args->app;

    if (app->gui.menu.active != MENU_NONE) return false;

    switch (ev->mouse_button) {
    case MOUSE_BUTTON_LEFT:
        sapp_lock_mouse(true);
        cube_type_e block = app->gui.hotbar.types[app->gui.hotbar.curr];
        engine_edit_active_block(engine, block, BLOCK_ACTION_BREAK, NULL);
        break;
    case MOUSE_BUTTON_RIGHT:
    {
        aabb_t player = aabb_with_offset(app->camera_ctl.collider, engine->_render_sys.cam.pos);
        cube_type_e block = app->gui.hotbar.types[app->gui.hotbar.curr];
        engine_edit_active_block(engine, block, BLOCK_ACTION_PLACE, &player);
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
        if (app->gui.menu.active != MENU_NONE)
            break;

        app->gui.hotbar.curr = ev->keycode - KEYCODE_1;
        vec2 pos = HB_ORIGIN;
        pos.x += app->gui.hotbar.curr * HB_SCALE * SPRITE_SIZE.x;
        sprite_renderer_move(&engine->_render_sys.sprite_renderer, app->gui.hotbar.selected_sprite, pos);
        break;

    case KEYCODE_ESCAPE:
        if (app->gui.menu.active == MENU_NONE) 
        {
            _switch_menu(app, &engine->_ui_sys, MENU_OPT);
            sapp_lock_mouse(false);
        }
        else if (app->gui.menu.active == MENU_OPT)
        {
            _switch_menu(app, &engine->_ui_sys, MENU_NONE);
            sapp_lock_mouse(true);
        }
        break;

    default:
        break;
    }

    if (app->gui.menu.active == MENU_CREATE)
    {
        char name_buf[UI_BUFLEN];
        ui_sys_query_text(&engine->_ui_sys, app->gui.menu.create[CREATEMENU_I_NAME], name_buf);
        bool has_name = name_buf[0] != '\0';
        bool not_too_many = app->gui.menu.data.world_num < MAX_WORLDS;
        bool unique = true;
        for (size_t i = 0; i < app->gui.menu.data.world_num && unique; i++)
            unique = strncmp(name_buf, app->gui.menu.data.world_names[i], UI_BUFLEN) != 0;

        bool enable = has_name && not_too_many && unique;
        char *error = "";
        if (!enable) 
        {
            APP_LOG_OK("Disabled create button:\nhas name: %s\nnot too many: %s\nunique: %s",
                       (has_name) ? "true" : "false",
                       (not_too_many) ? "true" : "false",
                       (unique) ? "true" : "false");
            if (!not_too_many) error = "Too Many Worlds (>5)";
            else if (!unique)  error = "Name Must be Unique";
            else               error = "Specify World Name";
        }

        ui_sys_enable_component(&engine->_ui_sys, app->gui.menu.create[CREATEMENU_B_CREATE], enable);
        ui_sys_set_component_text(&engine->_ui_sys, app->gui.menu.create[CREATEMENU_L_ERROR], error);
    }
    else if (app->gui.menu.active == MENU_LOAD)
    {
        char name_buf[UI_BUFLEN];
        ui_sys_query_text(&engine->_ui_sys, app->gui.menu.load[LOADMENU_I_RENAME], name_buf);
        bool changed = strncmp(name_buf, app->gui.menu.data.selected_world, UI_BUFLEN) != 0;
        bool has_name = name_buf[0] != '\0';
        bool unique = true;
        for (size_t i = 0; i < app->gui.menu.data.world_num && unique; i++)
            unique = strncmp(name_buf, app->gui.menu.data.world_names[i], UI_BUFLEN) != 0;

        bool enable = has_name && unique && changed;
        ui_sys_enable_component(&engine->_ui_sys, app->gui.menu.load[LOADMENU_B_RENAME], enable);
    }

    return false;
}

static void _back_to_main_clicked(ui_handle_t handle, void *args)
{
    APP_LOG_OK("Back to main clicked", NULL);
    (void) handle;
    app_event_args_t *ev_args = args;
    app_t *app = ev_args->app;
    engine_t *engine = ev_args->engine;

    _switch_menu(app, &engine->_ui_sys, MENU_MAIN);
}

static void _rename_selected_clicked(ui_handle_t handle, void *args)
{
    APP_LOG_OK("Rename selected clicked", NULL);
    (void) handle;
    app_event_args_t *ev_args = args;
    app_t *a = ev_args->app;
    engine_t *e = ev_args->engine;

    char wd_path[STD_BUFLEN] = {0};
    multicat(wd_path, 2, WORLD_DATA_DIR, a->gui.menu.data.selected_world);
    file_t wd = {
        .flags = FILEFLAG_DIR,
        .base = WORLD_DATA_DIR,
        .path = wd_path,
        .name = a->gui.menu.data.selected_world
    };

    char new_name[UI_BUFLEN];
    ui_sys_query_text(&e->_ui_sys, a->gui.menu.load[LOADMENU_I_RENAME], new_name);

    RUNTIME_ASSERT(file_exists(&wd), "World dir not found");
    RUNTIME_ASSERT(file_rename(&wd, new_name), "Failed to rename world dir");

    size_t selected = MAX_WORLDS;
    for (size_t i = 0; i < a->gui.menu.data.world_num; i++)
    {
        char text[UI_BUFLEN];
        ui_sys_query_text(&e->_ui_sys, a->gui.menu.load[LOADMENU_B_LOAD1 + i], text);
        if (strcmp(a->gui.menu.data.selected_world, text) == 0) 
        {
            selected = i;
            break;
        }
    }
    RUNTIME_ASSERT(selected < MAX_WORLDS, "Could not find the selected world for renaming");

    APP_LOG_WARN("Selected: %zu [%s]", selected, new_name);

    strncpy(a->gui.menu.data.selected_world, new_name, UI_BUFLEN);
    strncpy(a->gui.menu.data.world_names[selected], new_name, UI_BUFLEN);
    ui_sys_set_component_text(&e->_ui_sys, a->gui.menu.load[LOADMENU_B_LOAD1 + selected], new_name);
}

static void _load_selected_clicked(ui_handle_t handle, void *args)
{
    APP_LOG_OK("Load selected clicked", NULL);
    (void) handle;
    app_event_args_t *ev_args = args;
    app_t *app = ev_args->app;
    engine_t *engine = ev_args->engine;

    if (app->gui.menu.data.starting_game)
    {
        APP_LOG_ERROR("Already attempting to load a world", NULL);
        return;
    }

    engine_run_desc_t run_desc = {
        .run_mode = ENGINE_RUN_LOAD,
    };
    strncpy(run_desc.world_name, app->gui.menu.data.selected_world, UI_BUFLEN);
    RUNTIME_ASSERT(run_desc.world_name[0] != '\0', "Cannot load a world if nothing is selected");

    app->gui.menu.data.starting_game = true;

    _switch_menu(app, &engine->_ui_sys, MENU_NONE);
    for (size_t i = 0; i < HOTBAR_SIZE * 2; i++)
        app->gui.hotbar.sprites[i]->visible = true;
    app->gui.hotbar.selected_sprite->visible = true;
    app->gui.crosshair_sprite->visible = true;

    em_romu_duo_state_t s;
    em_romu_duo_init(&s, time(NULL));
    sapp_lock_mouse(true);
    engine_run(engine, &run_desc);
}

void _load_world_clicked(ui_handle_t handle, void *args)
{
    APP_LOG_OK("Load clicked", NULL);
    app_event_args_t *ev_args = args;
    app_t *a = ev_args->app;
    engine_t *e = ev_args->engine;

    char selected_world[UI_BUFLEN];
    ui_sys_query_text(&e->_ui_sys, handle, selected_world);
    strncpy(a->gui.menu.data.selected_world, selected_world, UI_BUFLEN);
    ENGINE_LOG_WARN("Setting rename input text to: [%s]", selected_world);
    ui_sys_set_component_text(&e->_ui_sys, a->gui.menu.load[LOADMENU_I_RENAME], selected_world);
    ui_sys_enable_component(&e->_ui_sys, a->gui.menu.load[LOADMENU_B_DELETE], true);
    ui_sys_enable_component(&e->_ui_sys, a->gui.menu.load[LOADMENU_B_PLAY], true);
}

static void _delete_selected_clicked(ui_handle_t handle, void *args)
{
    APP_LOG_OK("Delete selected clicked", NULL);
    (void) handle;
    app_event_args_t *ev_args = args;
    app_t *a = ev_args->app;
    engine_t *e = ev_args->engine;

    RUNTIME_ASSERT(a->gui.menu.data.selected_world[0] != '\0', "Cannot delete a world if nothing is selected");

    char wd_path[STD_BUFLEN] = {0};
    multicat(wd_path, 2, WORLD_DATA_DIR, a->gui.menu.data.selected_world);
    file_t wd = {
        .flags = FILEFLAG_DIR,
        .base = WORLD_DATA_DIR,
        .path = wd_path,
        .name = a->gui.menu.data.selected_world
    };

    RUNTIME_ASSERT(file_dir_exists(&wd), "World dir not found");
    RUNTIME_ASSERT(file_dir_delete(&wd), "Failed to delete world dir");

    // Shift names up to fill empty slot in world names array, and remove their components.
    bool shift;
    for (size_t i = 0; i < a->gui.menu.data.world_num; i++)
    {
        ui_sys_destroy_component(&e->_ui_sys, a->gui.menu.load[LOADMENU_B_LOAD1 + i]);
        if (strcmp(a->gui.menu.data.selected_world, a->gui.menu.data.world_names[i]) != 0)
            continue;

        memset(a->gui.menu.data.world_names[i], ' ', UI_BUFLEN);
        shift = true;
        if (i == a->gui.menu.data.world_num)
            a->gui.menu.data.world_names[i][0] = '\0';
        else if (shift)
            memcpy(a->gui.menu.data.world_names[i], a->gui.menu.data.world_names[i + 1], UI_BUFLEN);
    }

    // Decrement amount of worlds and set selected world (and display) to be empty.
    a->gui.menu.data.world_num--;
    memset(a->gui.menu.data.selected_world, ' ', UI_BUFLEN);
    ui_sys_set_component_text(&e->_ui_sys, a->gui.menu.load[LOADMENU_I_RENAME], a->gui.menu.data.selected_world);
    a->gui.menu.data.selected_world[0] = '\0';

    // Disable buttons that are only enabled when a world is selected
    ui_sys_enable_component(&e->_ui_sys, a->gui.menu.load[LOADMENU_B_RENAME], false);
    ui_sys_enable_component(&e->_ui_sys, a->gui.menu.load[LOADMENU_B_PLAY], false);
    ui_sys_enable_component(&e->_ui_sys, a->gui.menu.load[LOADMENU_B_DELETE], false);

    // Update list of worlds
    for (size_t i = 0; i < a->gui.menu.data.world_num; i++)
    {
        ui_button_desc_t desc = {
            .pos = VEC2(UI_SPACING.x, TITLE_HEIGHT - (UI_SIZE.y + UI_SPACING.y) * (i + 1)),
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
            .cb = _load_world_clicked,
            .cb_args = &a->ev_args,
            .visible = true
        };

        strncpy(desc.text, a->gui.menu.data.world_names[i], sizeof(a->gui.menu.data.world_names[i]));
        a->gui.menu.load[LOADMENU_B_LOAD1 + i] = ui_sys_make_button(&e->_ui_sys, &desc);
    }
    
}

static void _return_button_clicked(ui_handle_t handle, void *args)
{
    APP_LOG_OK("Return clicked", NULL);
    (void) handle;
    app_event_args_t *ev_args = args;
    app_t *app = ev_args->app;
    engine_t *engine = ev_args->engine;

    RUNTIME_ASSERT(app->gui.menu.active == MENU_OPT, "Wrong app state for this action");

    _switch_menu(app, &engine->_ui_sys, MENU_NONE);
    sapp_lock_mouse(true);

    size_t rd = engine->_ui_sys.components[app->gui.menu.options[OPTMENU_S_RENDER_DIST].id].value;
    if (rd != engine->meta.world.render_dist) engine_set_render_distance(engine, rd);
}

static void _main_menu_new_clicked(ui_handle_t handle, void *args)
{
    APP_LOG_OK("New clicked", NULL);

    (void) handle;
    app_event_args_t *ev_args = args;
    app_t *a = ev_args->app;
    engine_t *e = ev_args->engine;

    _switch_menu(a, &e->_ui_sys, MENU_CREATE);
    char name_buf[UI_BUFLEN];
    ui_sys_query_text(&e->_ui_sys, a->gui.menu.create[CREATEMENU_I_NAME], name_buf);
    bool unique = true;
    bool not_too_many = a->gui.menu.data.world_num < MAX_WORLDS;
    for (size_t i = 0; i < a->gui.menu.data.world_num && unique; i++)
        unique = strncmp(name_buf, a->gui.menu.data.world_names[i], UI_BUFLEN) != 0;

    bool enable = not_too_many && unique;
    char *error = "";
    if (!enable) 
    {
        if (!unique) error = "Name Must be Unique"; 
        else         error = "Too Many Worlds (>5)";
    }

    ui_sys_enable_component(&e->_ui_sys, a->gui.menu.create[CREATEMENU_B_CREATE], enable);
    ui_sys_set_component_text(&e->_ui_sys, a->gui.menu.create[CREATEMENU_L_ERROR], error);
}

static void _main_menu_load_clicked(ui_handle_t handle, void *args)
{
    APP_LOG_OK("Load clicked", NULL);
    (void) handle;
    app_event_args_t *ev_args = args;
    app_t *a = ev_args->app;
    engine_t *e = ev_args->engine;

    _switch_menu(a, &e->_ui_sys, MENU_LOAD);
}

static void _credits_clicked(ui_handle_t handle, void *args)
{
    APP_LOG_OK("Credits clicked", NULL);
    (void) handle;
    app_event_args_t *ev_args = args;
    app_t *a = ev_args->app;
    engine_t *e = ev_args->engine;

    _switch_menu(a, &e->_ui_sys, MENU_AUTHOR);
}

static void _quit_clicked(ui_handle_t handle, void *args)
{
    APP_LOG_OK("Quit clicked", NULL);
    (void) handle;
    (void) args;
    sapp_request_quit();
}

static void _new_clicked(ui_handle_t handle, void *args)
{
    APP_LOG_OK("New clicked", NULL);
    (void) handle;
    app_event_args_t *ev_args = args;
    app_t *a = ev_args->app;
    engine_t *e = ev_args->engine;

    APP_ASSERT(a->gui.menu.data.world_num < MAX_WORLDS, "Too many worlds");

    if (a->gui.menu.data.starting_game)
    {
        APP_LOG_ERROR("Already attempting to create a world", NULL);
        return;
    }

    engine_run_desc_t run_desc = {
        .time = 0,
        .cam_pos = VEC3(8.5, 128.0, 8.5),
        .cam_rot = QUAT(0.0, 0.0, 0.0, 1.0),
        .run_mode = ENGINE_RUN_NEW
    };

    ui_sys_query_text(&e->_ui_sys, a->gui.menu.create[CREATEMENU_I_NAME], run_desc.world_name);
    if (run_desc.world_name[0] == '\0')
        return;

    a->gui.menu.data.starting_game = true;

    em_romu_duo_state_t s;
    em_romu_duo_init(&s, time(NULL));

    char seed_str[UI_BUFLEN];
    ui_sys_query_text(&e->_ui_sys, a->gui.menu.create[CREATEMENU_I_SEED], seed_str);
    if (seed_str[0] == '\0') 
        run_desc.seed = em_romu_duo_random(&s);
    else 
        run_desc.seed = strtoul(seed_str, NULL, 10);

    _switch_menu(a, &e->_ui_sys, MENU_NONE);
    for (size_t i = 0; i < HOTBAR_SIZE * 2; i++)
        a->gui.hotbar.sprites[i]->visible = true;
    a->gui.hotbar.selected_sprite->visible = true;
    a->gui.crosshair_sprite->visible = true;

    sapp_lock_mouse(true);
    engine_run(e, &run_desc);
}

void app_init(engine_t *e, app_t *a, const app_desc_t *desc)
{
    (void) desc;

    a->ev_args = (app_event_args_t) {
        .app = a,
        .engine = e
    };

    load_model_files();

    a->gui.menu.data.world_num = MAX_WORLDS;
    a->gui = (app_gui_t) {
        .menu.active = MENU_MAIN,
        .menu.visible[MENU_MAIN] = true,
        .menu.data = {
            .world_num = MAX_WORLDS,
        },
        .hotbar = (struct hotbar) {
            .curr = 0,
            .sprites = malloc(HOTBAR_SIZE * 2 * sizeof(sprite_t *)),
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
            },
        },
    };
    file_t world_dir = { .path = WORLD_DATA_DIR, .flags = FILEFLAG_DIR };
    size_t max_len = sizeof(a->gui.menu.data.world_names[0]);
    file_list_dir(&world_dir, &a->gui.menu.data.world_num, max_len, a->gui.menu.data.world_names);
    ui_sys_init_handle_buffer(OPTMENU_NUM, &a->gui.menu.options[0]);
    ui_sys_init_handle_buffer(MAINMENU_NUM, &a->gui.menu.main[0]);
    ui_sys_init_handle_buffer(LOADMENU_NUM, &a->gui.menu.load[0]);
    ui_sys_init_handle_buffer(CREATEMENU_NUM, &a->gui.menu.create[0]);
    ui_sys_init_handle_buffer(AUTHORMENU_NUM, &a->gui.menu.author[0]);

    engine_init(e, &(engine_desc_t) {
        .render_distance = 12,
        .ticks_per_second = 20.0,
        .max_time = 24000, // 10 minute cycle @ 20tps
        .base_sun_dir = em_normalize_vec3(VEC3(0.0, 1.0, 0.1)),
        .gen_func = generate_chunk_data,
        .init_cursor_line_renderer = true,
        .init_sprite_renderer = true,
        .init_chunk_renderer = true
    });
    e->meta.cursor.range = 10.0;

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
    // a->hotbar.sprites = 
    for (size_t i = 0; i < HOTBAR_SIZE; i++)
    {
        hb_desc.bg_col = VEC4(0.0, 0.0, 0.0, 0.5);
        hb_desc.uv_offset = sprite_icon_uv_offset(&e->_render_sys.sprite_renderer, IID_SLOT);
        a->gui.hotbar.sprites[i] = sprite_renderer_push(&e->_render_sys.sprite_renderer, &hb_desc);

        hb_desc.bg_col = VEC4(0.0, 0.0, 0.0, 0.0);
        hb_desc.uv_offset = sprite_icon_uv_offset(&e->_render_sys.sprite_renderer, (icon_id_e) a->gui.hotbar.types[i]);
        a->gui.hotbar.sprites[i + HOTBAR_SIZE] = sprite_renderer_push(&e->_render_sys.sprite_renderer, &hb_desc);

        hb_desc.pos.x += HB_SCALE * SPRITE_SIZE.x;
    }

    hb_desc.z_index = 2.0;
    hb_desc.pos.x = HB_ORIGIN.x + a->gui.hotbar.curr * HB_ORIGIN.x;
    hb_desc.uv_offset = sprite_icon_uv_offset(&e->_render_sys.sprite_renderer, IID_SLOT_SELECTED);
    a->gui.hotbar.selected_sprite = sprite_renderer_push(&e->_render_sys.sprite_renderer, &hb_desc);

    // Initialize crosshair sprite
    a->gui.crosshair_sprite = sprite_renderer_push(&e->_render_sys.sprite_renderer, &(sprite_desc_t) {
        .pos = VEC2(-16.0, -16.0),
        .scale = 2.0,
        .z_index = 1.0,
        .uv_offset = sprite_icon_uv_offset(&e->_render_sys.sprite_renderer, IID_CROSSHAIR),
        .visible = false
    });

    // Initialize options menu sprites
    a->gui.menu.options[OPTMENU_L_TITLE] = ui_sys_make_label(&e->_ui_sys, &(ui_label_desc_t) {
        .pos = VEC2(-11.0 * 3.0 * 4.0, TITLE_HEIGHT),
        .text_style = {
            .scale = TITLE_SCALE,
            .z_index = 4.0,
            .bg_col = VEC4(0.0, 0.0, 0.0, 0.0)
        },
        .visible = false,
        .text = "Game Menu"
    });

    a->gui.menu.options[OPTMENU_B_BACK] = ui_sys_make_button(&e->_ui_sys, &(ui_button_desc_t) {
        .pos = VEC2(-UI_SIZE.x / 2.0, TITLE_HEIGHT - UI_SIZE.y - UI_SPACING.y),
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

    a->gui.menu.options[OPTMENU_S_RENDER_DIST] = ui_sys_make_slider(&e->_ui_sys, &(ui_slider_desc_t) {
        .pos = VEC2(-UI_SIZE.x / 2.0, TITLE_HEIGHT - (UI_SIZE.y + UI_SPACING.y) * 2.0),
        .width = UI_DIM.x,
        .body_style = {
            .scale = UI_SCALE,
            .z_index = 3.0,
            .bg_col = INPUT_BG_COL,
            .thumb_bg_col = BG_COL,
            .hover_bg_col = HOVER_COL
        },
        .text_style = {
            .scale = TEXT_SCALE,
            .z_index = 4.0
        },
        .min_value = 3,
        .max_value = 32,
        .value = e->meta.world.render_dist,
        .text = "Render Distance:",
    });

    a->gui.menu.options[OPTMENU_B_QUIT] = ui_sys_make_button(&e->_ui_sys, &(ui_button_desc_t) {
        .pos = VEC2(-UI_SIZE.x / 2.0, TITLE_HEIGHT - (UI_SIZE.y + UI_SPACING.y) * 3.0),
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
        .cb = _quit_clicked,
        .cb_args = &a->ev_args
    });

    // Initialize main menu sprites
    a->gui.menu.main[MAINMENU_L_TITLE] = ui_sys_make_label(&e->_ui_sys, &(ui_label_desc_t) {
        .pos = VEC2(-11.0 * 3.0 * 5.5, TITLE_HEIGHT),
        .text_style = {
            .scale = TITLE_SCALE,
            .z_index = 4.0,
            .bg_col = VEC4(0.0, 0.0, 0.0, 0.0)
        },
        .visible = true,
        .text = "Voxel Engine"
    });

    a->gui.menu.main[MAINMENU_B_CREATE] = ui_sys_make_button(&e->_ui_sys, &(ui_button_desc_t) {
        .pos = VEC2(-UI_SIZE.x / 2.0, TITLE_HEIGHT - (UI_SIZE.y + UI_SPACING.y)),
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
        .text = "New Game",
        .cb = _main_menu_new_clicked,
        .cb_args = &a->ev_args
    });

    a->gui.menu.main[MAINMENU_B_LOAD] = ui_sys_make_button(&e->_ui_sys, &(ui_button_desc_t) {
        .pos = VEC2(-UI_SIZE.x / 2.0, TITLE_HEIGHT - (UI_SIZE.y + UI_SPACING.y) * 2.0),
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
        .text = "Saved Games",
        .cb = _main_menu_load_clicked,
        .cb_args = &a->ev_args
    });

    a->gui.menu.main[MAINMENU_B_AUTHOR] = ui_sys_make_button(&e->_ui_sys, &(ui_button_desc_t) {
        .pos = VEC2(-UI_SIZE.x / 2.0, TITLE_HEIGHT - (UI_SIZE.y + UI_SPACING.y) * 3.0),
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
        .text = "Credits",
        .cb = _credits_clicked,
        .cb_args = &a->ev_args
    });

    a->gui.menu.main[MAINMENU_B_QUIT] = ui_sys_make_button(&e->_ui_sys, &(ui_button_desc_t) {
        .pos = VEC2(-UI_SIZE.x / 2.0, TITLE_HEIGHT - (UI_SIZE.y + UI_SPACING.y) * 4.0),
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
        .cb = _quit_clicked,
        .cb_args = &a->ev_args
    });

    // Initialize create menu sprites
    a->gui.menu.create[CREATEMENU_L_TITLE] = ui_sys_make_label(&e->_ui_sys, &(ui_label_desc_t) {
        .pos = VEC2(-11.0 * 3.0 * 5.5, TITLE_HEIGHT),
        .text_style = {
            .scale = TITLE_SCALE,
            .z_index = 4.0,
            .bg_col = VEC4(0.0, 0.0, 0.0, 0.0)
        },
        .text = "New Game"
    });

    a->gui.menu.create[CREATEMENU_L_NAME] = ui_sys_make_label(&e->_ui_sys, &(ui_label_desc_t) {
        .pos = VEC2(-UI_SIZE.x / 2.0, TITLE_HEIGHT - (UI_SIZE.y + UI_SPACING.y) + TEXT_SIZE.y),
        .text_style = {
            .scale = TEXT_SCALE,
            .z_index = 4.0,
            .bg_col = VEC4(0.0, 0.0, 0.0, 0.0)
        },
        .text = "World Name:"
    });

    a->gui.menu.create[CREATEMENU_I_NAME] = ui_sys_make_input(&e->_ui_sys, &(ui_input_desc_t) {
        .pos = VEC2(-UI_SIZE.x / 2.0, TITLE_HEIGHT - UI_SIZE.y - UI_SPACING.y * 2.0),
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
        .text = "my world",
        .max_len = 20
    });

    a->gui.menu.create[CREATEMENU_L_SEED] = ui_sys_make_label(&e->_ui_sys, &(ui_label_desc_t) {
        .pos = VEC2(-UI_SIZE.x / 2.0, TITLE_HEIGHT - (UI_SIZE.y + UI_SPACING.y) * 2.0 - UI_SPACING.y + TEXT_SIZE.y),
        .text_style = {
            .scale = TEXT_SCALE,
            .z_index = 4.0,
            .bg_col = VEC4(0.0, 0.0, 0.0, 0.0)
        },
        .text = "World Seed: (blank = random)"
    });

    a->gui.menu.create[CREATEMENU_I_SEED] = ui_sys_make_input(&e->_ui_sys, &(ui_input_desc_t) {
        .pos = VEC2(-UI_SIZE.x / 2.0, TITLE_HEIGHT - UI_SIZE.y - UI_SPACING.y * 3.0 - (UI_SIZE.y + UI_SPACING.y)),
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
        .max_len = 11,
        .filter = FILTER_NUMER
    });

    a->gui.menu.create[CREATEMENU_B_CREATE] = ui_sys_make_button(&e->_ui_sys, &(ui_button_desc_t) {
        .pos = VEC2(-UI_SIZE.x / 2.0, TITLE_HEIGHT - UI_SIZE.y - UI_SPACING.y * 4.0 - (UI_SIZE.y + UI_SPACING.y) * 2.0),
        .dim = UI_DIM,
        .body_style = {
            .scale = UI_SCALE,
            .z_index = 3.0,
            .bg_col = BG_COL,
            .hover_bg_col = HOVER_COL,
            .disabled_bg_col = HOVER_COL
        },
        .text_style = {
            .scale = TEXT_SCALE,
            .z_index = 4.0
        },
        .text = "Create World",
        .cb = _new_clicked,
        .cb_args = &a->ev_args
    });

    a->gui.menu.create[CREATEMENU_L_ERROR] = ui_sys_make_label(&e->_ui_sys, &(ui_label_desc_t) {
        .pos = VEC2(-UI_SIZE.x / 2.0, TITLE_HEIGHT - (UI_SIZE.y + UI_SPACING.y) * 3.0 - UI_SPACING.y * 2.0 + TEXT_SIZE.y),
        .text_style = {
            .tint_col = VEC4(0.9, 0.0, 0.0, 1.0),
            .scale = TEXT_SCALE,
            .z_index = 4.0
        },
    });

    a->gui.menu.create[CREATEMENU_B_BACK] = ui_sys_make_button(&e->_ui_sys, &(ui_button_desc_t) {
        .pos = VEC2(-UI_SIZE.x / 2.0, TITLE_HEIGHT - UI_SIZE.y - UI_SPACING.y * 4.0 - (UI_SIZE.y + UI_SPACING.y) * 3.0),
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
        .text = "Back",
        .cb = _back_to_main_clicked,
        .cb_args = &a->ev_args
    });

    // Initialize load menu sprites
    a->gui.menu.load[LOADMENU_L_TITLE] = ui_sys_make_label(&e->_ui_sys, &(ui_label_desc_t) {
        .pos = VEC2(-11.0 * 3.0 * 5.0, TITLE_HEIGHT),
        .text_style = {
            .scale = TITLE_SCALE,
            .z_index = 4.0,
            .bg_col = VEC4(0.0, 0.0, 0.0, 0.0)
        },
        .text = "Saved Games"
    });

    a->gui.menu.load[LOADMENU_I_RENAME] = ui_sys_make_input(&e->_ui_sys, &(ui_input_desc_t) {
        .pos = VEC2(-UI_SIZE.x - UI_SPACING.x, TITLE_HEIGHT - (UI_SIZE.y + UI_SPACING.y)),
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

    a->gui.menu.load[LOADMENU_B_RENAME] = ui_sys_make_button(&e->_ui_sys, &(ui_button_desc_t) {
        .pos = VEC2(-UI_SIZE.x - UI_SPACING.x, TITLE_HEIGHT - (UI_SIZE.y + UI_SPACING.y) * 2.0),
        .dim = UI_DIM,
        .body_style = {
            .scale = UI_SCALE,
            .z_index = 3.0,
            .bg_col = BG_COL,
            .hover_bg_col = HOVER_COL,
            .disabled_bg_col = HOVER_COL
        },
        .text_style = {
            .scale = TEXT_SCALE,
            .z_index = 4.0
        },
        .text = "Rename Selected",
        .cb = _rename_selected_clicked,
        .cb_args = &a->ev_args
    });

    a->gui.menu.load[LOADMENU_B_PLAY] = ui_sys_make_button(&e->_ui_sys, &(ui_button_desc_t) {
        .pos = VEC2(-UI_SIZE.x - UI_SPACING.x, TITLE_HEIGHT - (UI_SIZE.y + UI_SPACING.y) * 3.0),
        .dim = UI_DIM,
        .body_style = {
            .scale = UI_SCALE,
            .z_index = 3.0,
            .bg_col = BG_COL,
            .hover_bg_col = HOVER_COL,
            .disabled_bg_col = HOVER_COL
        },
        .text_style = {
            .scale = TEXT_SCALE,
            .z_index = 4.0
        },
        .text = "Load Selected",
        .cb = _load_selected_clicked,
        .cb_args = &a->ev_args,
    });

    a->gui.menu.load[LOADMENU_B_DELETE] = ui_sys_make_button(&e->_ui_sys, &(ui_button_desc_t) {
        .pos = VEC2(-UI_SIZE.x - UI_SPACING.x, TITLE_HEIGHT - (UI_SIZE.y + UI_SPACING.y) * 4.0),
        .dim = UI_DIM,
        .body_style = {
            .scale = UI_SCALE,
            .z_index = 3.0,
            .bg_col = BG_COL,
            .hover_bg_col = HOVER_COL,
            .disabled_bg_col = HOVER_COL
        },
        .text_style = {
            .scale = TEXT_SCALE,
            .z_index = 4.0
        },
        .text = "Delete Selected",
        .cb = _delete_selected_clicked,
        .cb_args = &a->ev_args
    });

    a->gui.menu.load[LOADMENU_B_BACK] = ui_sys_make_button(&e->_ui_sys, &(ui_button_desc_t) {
        .pos = VEC2(-UI_SIZE.x - UI_SPACING.x, TITLE_HEIGHT - (UI_SIZE.y + UI_SPACING.y) * 5.0),
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
        .text = "Back",
        .cb = _back_to_main_clicked,
        .cb_args = &a->ev_args
    });

    ui_sys_enable_component(&e->_ui_sys, a->gui.menu.load[LOADMENU_B_RENAME], false);
    ui_sys_enable_component(&e->_ui_sys, a->gui.menu.load[LOADMENU_B_PLAY], false);
    ui_sys_enable_component(&e->_ui_sys, a->gui.menu.load[LOADMENU_B_DELETE], false);

    for (size_t i = 0; i < a->gui.menu.data.world_num; i++)
    {
        ui_button_desc_t desc = {
            .pos = VEC2(UI_SPACING.x, TITLE_HEIGHT - (UI_SIZE.y + UI_SPACING.y) * (i + 1)),
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
            .cb = _load_world_clicked,
            .cb_args = &a->ev_args
        };

        strncpy(desc.text, a->gui.menu.data.world_names[i], sizeof(a->gui.menu.data.world_names[i]));
        a->gui.menu.load[LOADMENU_B_LOAD1 + i] = ui_sys_make_button(&e->_ui_sys, &desc);
    }

    // Initialize author menu
    a->gui.menu.author[AUTHORMENU_L_TITLE] = ui_sys_make_label(&e->_ui_sys, &(ui_label_desc_t) {
        .pos = VEC2(-11.0 * 3.0 * 5.0, TITLE_HEIGHT),
        .text_style = {
            .scale = TITLE_SCALE,
            .z_index = 4.0,
            .bg_col = VEC4(0.0, 0.0, 0.0, 0.0)
        },
        .text = "Credits"
    });

    a->gui.menu.author[AUTHORMENU_L_LINE1] = ui_sys_make_label(&e->_ui_sys, &(ui_label_desc_t) {
        .pos = VEC2(-TEXT_SIZE.x * UI_BUFLEN / 2.0, TITLE_HEIGHT - (TEXT_SIZE.y + TEXT_SPACING) * 3.0 - UI_SPACING.y),
        .text_style = {
            .scale = TEXT_SCALE,
            .z_index = 4.0,
            .bg_col = VEC4(0.0, 0.0, 0.0, 0.0)
        },
        .text = "A High-Performance Voxel Engine, created by Ignacy Wegner       "
    });
    a->gui.menu.author[AUTHORMENU_L_LINE2] = ui_sys_make_label(&e->_ui_sys, &(ui_label_desc_t) {
        .pos = VEC2(-TEXT_SIZE.x * UI_BUFLEN / 2.0, TITLE_HEIGHT - (TEXT_SIZE.y + TEXT_SPACING) * 5.0 - UI_SPACING.y),
        .text_style = {
            .scale = TEXT_SCALE,
            .z_index = 4.0,
            .bg_col = VEC4(0.0, 0.0, 0.0, 0.0)
        },
        .text = "Thank you for trying 'Voxel Engine'"
    });
    a->gui.menu.author[AUTHORMENU_L_LINE3] = ui_sys_make_label(&e->_ui_sys, &(ui_label_desc_t) {
        .pos = VEC2(-TEXT_SIZE.x * UI_BUFLEN / 2.0, TITLE_HEIGHT - (TEXT_SIZE.y + TEXT_SPACING) * 7.0 - UI_SPACING.y),
        .text_style = {
            .scale = TEXT_SCALE,
            .z_index = 4.0,
            .bg_col = VEC4(0.0, 0.0, 0.0, 0.0)
        },
        .text = "Creating this has been a larger undertaking than I initially    "
    });
    a->gui.menu.author[AUTHORMENU_L_LINE4] = ui_sys_make_label(&e->_ui_sys, &(ui_label_desc_t) {
        .pos = VEC2(-TEXT_SIZE.x * UI_BUFLEN / 2.0, TITLE_HEIGHT - (TEXT_SIZE.y + TEXT_SPACING) * 8.0 - UI_SPACING.y),
        .text_style = {
            .scale = TEXT_SCALE,
            .z_index = 4.0,
            .bg_col = VEC4(0.0, 0.0, 0.0, 0.0)
        },
        .text = "thought but it was well worth the effort.                       "
    });
    a->gui.menu.author[AUTHORMENU_L_LINE5] = ui_sys_make_label(&e->_ui_sys, &(ui_label_desc_t) {
        .pos = VEC2(-TEXT_SIZE.x * UI_BUFLEN / 2.0, TITLE_HEIGHT - (TEXT_SIZE.y + TEXT_SPACING) * 10.0 - UI_SPACING.y),
        .text_style = {
            .scale = TEXT_SCALE,
            .z_index = 4.0,
            .bg_col = VEC4(0.0, 0.0, 0.0, 0.0)
        },
        .text = "If you like my engine, please give it a star at:                "
    });
    a->gui.menu.author[AUTHORMENU_L_LINE6] = ui_sys_make_label(&e->_ui_sys, &(ui_label_desc_t) {
        .pos = VEC2(-TEXT_SIZE.x * UI_BUFLEN / 2.0, TITLE_HEIGHT - (TEXT_SIZE.y + TEXT_SPACING) * 11.0 - UI_SPACING.y),
        .text_style = {
            .scale = TEXT_SCALE,
            .z_index = 4.0,
            .bg_col = VEC4(0.0, 0.0, 0.0, 0.0)
        },
        .text = "github.com/angstrom-123/Voxel-Engine                            "
    });

    a->gui.menu.author[AUTHORMENU_B_BACK] = ui_sys_make_button(&e->_ui_sys, &(ui_button_desc_t) {
        .pos = VEC2(-UI_SIZE.x / 2.0, TITLE_HEIGHT - UI_SIZE.y - UI_SPACING.y * 4.0 - (UI_SIZE.y + UI_SPACING.y) * 3.0),
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
        .text = "Back",
        .cb = _back_to_main_clicked,
        .cb_args = &a->ev_args
    });

    engine_do_render(e);
}

void app_cleanup(app_t *app)
{
    free(app->gui.hotbar.sprites);
    unload_model_files();
    ctl_cleanup(&app->camera_ctl);
}

void app_frame(engine_t *engine, app_t *app, double dt) 
{
    INSTRUMENT_FUNC_BEGIN();

    bool do_inputs = app->gui.menu.active == MENU_NONE;

    ctl_update_surrounding(&app->camera_ctl, &engine->_render_sys.cam, &engine->_chunk_sys);
    ctl_update_pos(&app->camera_ctl, &engine->_render_sys.cam, &engine->_event_sys, dt, do_inputs);
    ctl_update_view(&app->camera_ctl, &engine->_render_sys.cam, &engine->_event_sys, do_inputs);
    cam_update(&engine->_render_sys.cam);

    INSTRUMENT_FUNC_END();
}

void app_tick(engine_t *engine, app_t *app)
{
    (void) engine;
    (void) app;
}
