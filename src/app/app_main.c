#include "app_main.h"

void app_init(app_t *app, const app_desc_t *desc)
{
    (void) desc;
    camera_ctl_init(&app->camera_ctl, &(camera_controller_desc_t) {
        .move_speed = 10.0,
        .turn_speed = 0.04
    });
}

void app_cleanup(app_t *app)
{
    camera_ctl_cleanup(&app->camera_ctl);
}

void app_frame(engine_t *engine, app_t *app, double dt) 
{
    (void) engine;
    (void) app;
    // APP_LOG_OK("App frame.\n", NULL);
    camera_ctl_update(&app->camera_ctl, &engine->_render_sys.cam, 
                      &engine->_event_sys, dt);
}
