#ifndef APP_H
#define APP_H 

#include "camera_controller.h"
#include "instrumentor.h"
#include "engine.h"
#include "arg_parser.h"
#include "world_gen.h"

#include <stdatomic.h>

#define MAX_WORLDS 5

#define BUTTON_COL VEC4(0.4, 0.4, 0.4, 1.0)
#define BUTTON_HOVER_COL VEC4(0.5, 0.5, 0.5, 1.0)
#define HOTBAR_SIZE 9

#define TITLE_HEIGHT 400.0
#define TITLE_SCALE 3.0
#define UI_SCALE 3.0
#define TEXT_SCALE 2.0
#define SUBTEXT_SCALE 1.0
#define UI_DIM UVEC2(16, 2)
#define UI_SPACING VEC2(40.0, 40.0)
#define TEXT_SPACING 10.0
#define BG_COL VEC4(0.4, 0.4, 0.4, 1.0)
#define HOVER_COL VEC4(0.6, 0.6, 0.6, 1.0)
#define INPUT_BG_COL VEC4(0.25, 0.25, 0.25, 1.0)
#define HB_SCALE 5.0
#define HB_ORIGIN VEC2(-SPRITE_SIZE.x * HB_SCALE * HOTBAR_SIZE / 2.0, -SCREEN_HEIGHT / 2.0 + SPRITE_SIZE.y * HB_SCALE)
#define UI_SIZE VEC2(UI_DIM.x * SPRITE_SIZE.x * UI_SCALE, UI_DIM.y * SPRITE_SIZE.y * UI_SCALE)
#define TEXT_SIZE VEC2(CHAR_SIZE.x * TEXT_SCALE, CHAR_SIZE.y * TEXT_SCALE)

typedef enum opt_menu_component {
    OPTMENU_L_TITLE,
    OPTMENU_B_BACK,
    OPTMENU_S_RENDER_DIST,
    OPTMENU_B_QUIT,
    OPTMENU_NUM
} opt_menu_component_e;

typedef enum main_menu_component {
    MAINMENU_L_TITLE,
    MAINMENU_B_CREATE,
    MAINMENU_B_LOAD,
    MAINMENU_B_AUTHOR,
    MAINMENU_B_QUIT,
    MAINMENU_NUM
} main_menu_component_e;

typedef enum load_menu_component {
    LOADMENU_B_LOAD1 = 0,
    LOADMENU_B_LOAD2 = 1,
    LOADMENU_B_LOAD3 = 2,
    LOADMENU_B_LOAD4 = 3,
    LOADMENU_B_LOAD5 = 4,
    LOADMENU_B_DELETE,
    LOADMENU_B_PLAY,
    LOADMENU_L_RENAME,
    LOADMENU_I_RENAME, // Show world name in an input.
    LOADMENU_B_RENAME, // If input is edited, can click rename to change name.
    LOADMENU_L_TITLE,
    LOADMENU_B_BACK,
    LOADMENU_NUM
} load_menu_component_e;

typedef enum create_menu_component {
    CREATEMENU_L_TITLE,
    CREATEMENU_L_NAME,
    CREATEMENU_I_NAME,
    CREATEMENU_L_SEED,
    CREATEMENU_I_SEED,
    CREATEMENU_B_CREATE,
    CREATEMENU_L_ERROR,
    CREATEMENU_B_BACK,
    CREATEMENU_NUM
} create_menu_component_e;

typedef enum author_menu_component {
    AUTHORMENU_L_TITLE,
    AUTHORMENU_L_LINE1,
    AUTHORMENU_L_LINE2,
    AUTHORMENU_L_LINE3,
    AUTHORMENU_L_LINE4,
    AUTHORMENU_L_LINE5,
    AUTHORMENU_L_LINE6,
    AUTHORMENU_B_BACK,
    AUTHORMENU_NUM
} author_menu_component_e;

typedef struct app_gui {
    struct hotbar {
        cube_type_e types[HOTBAR_SIZE];
        sprite_t **sprites;
        sprite_t *selected_sprite;
        size_t curr;
    } hotbar;

    sprite_t *crosshair_sprite;

    struct {
        enum menu {
            MENU_NONE,
            MENU_OPT,
            MENU_MAIN,
            MENU_LOAD,
            MENU_CREATE,
            MENU_AUTHOR,
            MENU_NUM
        } active;

        struct {
            bool starting_game;
            size_t world_num;
            char world_names[MAX_WORLDS][UI_BUFLEN];
            char selected_world[UI_BUFLEN];
        } data;

        bool visible[MENU_NUM];
        ui_handle_t options[OPTMENU_NUM];
        ui_handle_t main[MAINMENU_NUM];
        ui_handle_t load[LOADMENU_NUM];
        ui_handle_t create[CREATEMENU_NUM];
        ui_handle_t author[AUTHORMENU_NUM];
    } menu;
} app_gui_t;

typedef struct app {
    ctl_t camera_ctl;

    app_gui_t gui;

    struct app_event_args {
        engine_t *engine;
        struct app *app;
    } ev_args;
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
