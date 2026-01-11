#ifndef APP_H
#define APP_H 

#include "camera_controller.h"
#include "instrumentor.h"
#include "engine.h"
#include "world_creation.h"

#include <stdatomic.h>

typedef struct app {
    ctl_t camera_ctl;
    atomic_bool needs_physics_update;

    struct md_args {
        engine_t *engine;
        struct app *app;
    } mousedown_args;
} app_t;

typedef struct app_desc {
    int argc;
    char **argv;
} app_desc_t;

extern void app_init(engine_t *engine, app_t *app, const app_desc_t *desc);
extern void app_cleanup(app_t *app);
extern void app_frame(engine_t *engine, app_t *app, double dt);
extern void app_tick(engine_t *engine, app_t *app);

#endif
