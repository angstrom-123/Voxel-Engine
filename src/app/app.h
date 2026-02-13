#ifndef APP_H
#define APP_H 

#include "camera_controller.h"
#include "instrumentor.h"
#include "engine.h"
#include "world_creation.h"
#include "arg_parser.h"
#include "world_gen.h"

#include <stdatomic.h>

#define BUTTON_COL VEC4(0.4, 0.4, 0.4, 1.0)
#define BUTTON_HOVER_COL VEC4(0.5, 0.5, 0.5, 1.0)
#define HOTBAR_SIZE 9
#define HOTBAR_CELL_SIZE VEC2(80.0, 80.0)
#define BASE_HOTBAR_POS VEC2(-HOTBAR_CELL_SIZE.x * (HOTBAR_SIZE / 2.0), \
                             -SCREEN_HEIGHT / 2.0 + HOTBAR_CELL_SIZE.y)

typedef enum menu_component {
    MENUCOMP_L_TITLE,
    MENUCOMP_B_RETURN,
    MENUCOMP_B_QUIT,
    MENUCOMP_NUM
} menu_component_e;

typedef struct app {
    ctl_t camera_ctl;

    struct app_event_args {
        engine_t *engine;
        struct app *app;
    } ev_args;

    struct hotbar {
        cube_type_e types[HOTBAR_SIZE];
        sprite_t *selected_sprite;
        size_t curr;
    } hotbar;

    struct menu {
        bool active;
        ui_handle_t comps[MENUCOMP_NUM];
        // TODO: 
        //      - Make UI components for each menu button
        //          - Probably best to make an array with enum indices 
        //              - Will let me iterate them (e.g., to make them all visible)
        //      - Make callbacks for the onclick of each button 
        //      - Implement a text box for inputting values (will need for main menu)
        //      - Implement a slider? (good for render distance)
    } menu;
} app_t;

typedef struct app_event_args app_event_args_t;

typedef struct app_desc {
    args_t args;
} app_desc_t;

extern void app_init(engine_t *engine, app_t *app, const app_desc_t *desc);
extern void app_cleanup(app_t *app);
extern void app_frame(engine_t *engine, app_t *app, double dt);
extern void app_tick(engine_t *engine, app_t *app);

#endif
