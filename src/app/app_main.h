#ifndef APP_MAIN_H
#define APP_MAIN_H 

#include "engine/engine.h"
#include "engine/camera_controller.h"

typedef struct app {
    camera_controller_t camera_ctl;
} app_t;

typedef struct app_desc {
    int32_t placeholder;
} app_desc_t;

extern void app_init(app_t *app_instance, const app_desc_t *desc);
extern void app_cleanup(app_t *app_instance);
extern void app_frame(engine_t *engine_instance, app_t *app_instance, double dt);

#endif
