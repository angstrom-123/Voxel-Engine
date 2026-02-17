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

#define TITLE_HEIGHT 400.0
#define TITLE_SCALE 3.0
#define UI_SCALE 3.0
#define TEXT_SCALE 2.0
#define UI_DIM UVEC2(10, 2)
#define UI_SPACING VEC2(40.0, 40.0)
#define BG_COL VEC4(0.4, 0.4, 0.4, 1.0)
#define HOVER_COL VEC4(0.6, 0.6, 0.6, 1.0)
#define INPUT_BG_COL VEC4(0.25, 0.25, 0.25, 1.0)

typedef enum opt_menu_component {
    OPTMENUCOMP_L_TITLE,
    OPTMENUCOMP_B_RETURN,
    OPTMENUCOMP_B_QUIT,
    OPTMENUCOMP_SLIDER,
    OPTMENUCOMP_INPUT,
    OPTMENUCOMP_NUM
} opt_menu_component_e;

typedef enum main_menu_component {
    MAINMENUCOMP_L_TITLE,
    MAINMENUCOMP_B_WORLDS,
    MAINMENUCOMP_B_QUIT,
    MAINMENUCOMP_NUM
} main_menu_component_e;

typedef enum world_menu_component {
    WORLDMENUCOMP_B_LOAD1 = 0,
    WORLDMENUCOMP_B_LOAD2 = 1,
    WORLDMENUCOMP_B_LOAD3 = 2,
    WORLDMENUCOMP_B_LOAD4 = 3,
    WORLDMENUCOMP_B_LOAD5 = 4,
    WORLDMENUCOMP_L_TITLE,
    WORLDMENUCOMP_I_NAME,
    WORLDMENUCOMP_I_SEED,
    WORLDMENUCOMP_B_CREATE,
    WORLDMENUCOMP_NUM
} world_menu_component_e;

typedef enum menu {
    MENU_OPT,
    MENU_MAIN,
    MENU_WORLD,
    MENU_NUM
} menu_e;

typedef struct app {
    enum app_state {
        APP_MAIN_MENU,
        APP_WORLDS_MENU,
        APP_IN_GAME,
        APP_GAME_OPTIONS,
        APP_STATE_NUM
    } state;
    ctl_t camera_ctl;

    struct app_event_args {
        engine_t *engine;
        struct app *app;
    } ev_args;

    struct hotbar {
        cube_type_e types[HOTBAR_SIZE];
        sprite_t **sprites;
        sprite_t *selected_sprite;
        size_t curr;
    } hotbar;

    struct options_menu {
        ui_handle_t comps[OPTMENUCOMP_NUM];
    } options_menu;

    struct main_menu {
        ui_handle_t comps[MAINMENUCOMP_NUM];
    } main_menu;

    struct world_menu {
        ui_handle_t comps[WORLDMENUCOMP_NUM];
        bool new_clicked;
    } world_menu;
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
