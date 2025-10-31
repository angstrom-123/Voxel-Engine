#include "app.h"

static void _on_mousedown(const event_t *ev, void *args) 
{
    app_t *app = args;

    switch (ev->mouse_button) {
    case MOUSE_BUTTON_LEFT:
        api_raycast_dda(&app->engine_api, EDIT_BREAK, 10);
        break;
    case MOUSE_BUTTON_RIGHT:
        api_raycast_dda(&app->engine_api, EDIT_PLACE, 10);
        break;
    case MOUSE_BUTTON_MIDDLE:
        break;
    default:
        break;
    };
}


void app_init(engine_t *engine, app_t *app, const app_desc_t *desc)
{
    camera_ctl_init(&app->camera_ctl, &engine->_render_sys.cam, &(camera_controller_desc_t) {
        .start_pos = (vec3) {8.0, 100.0, 8.0},
        .move_speed = 20.0,
        .turn_speed = 0.04
    });

    app->engine_api = (engine_api_t) {
        .engine_ptr = desc->engine
    };
    api_subscribe_to_event(&app->engine_api, EVENT_MOUSEDOWN, &(event_subscriber_desc_t) {
        .event_cb = _on_mousedown,
        .block_cb = block_func_never,
        .args = app
    });
}

void app_cleanup(app_t *app)
{
    camera_ctl_cleanup(&app->camera_ctl);
}

void app_frame(engine_t *engine, app_t *app, double dt) 
{
    INSTRUMENT_FUNC_BEGIN();

    camera_ctl_update(&app->camera_ctl, &engine->_render_sys.cam, 
                      &engine->_event_sys, dt);

    INSTRUMENT_FUNC_END();
}
