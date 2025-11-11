#ifndef APP_H
#define APP_H 

#include "camera_controller.h"
#include "instrumentor.h"
#include "engine.h"

typedef struct app {
    camera_controller_t camera_ctl;
} app_t;

typedef struct app_desc {
} app_desc_t;

extern void app_init(engine_t *engine, app_t *app, const app_desc_t *desc);
extern void app_cleanup(app_t *app);
extern void app_frame(engine_t *engine, app_t *app, double dt);
extern void app_tick(app_t *app);

#endif
