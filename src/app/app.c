#include "app.h"
#include "engine.h"

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

void app_init(engine_t *engine, app_t *app, const app_desc_t *desc)
{
    (void) desc;
    app->mousedown_args = (struct md_args) {
        .app = app,
        .engine = engine
    };

    app->needs_physics_update = false;
    ctl_init(&app->camera_ctl, &engine->_render_sys.cam, &(ctl_desc_t) {
        .start_pos      = VEC3(8.5, 100.0, 8.5),
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
}
