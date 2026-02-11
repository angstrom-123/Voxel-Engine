#ifndef UI_COMPONENT_H
#define UI_COMPONENT_H

#include "sprite_renderer.h"

#define UI_BUFLEN 32

typedef enum ui_component_type {
    COMPONENT_BUTTON,
    COMPONENT_LABEL
} ui_component_type_e;

typedef struct ui_transform {
    vec2 pos;
    uvec2 dim;
} ui_transform_t;

typedef struct ui_style {
    struct {
        bool mini;
        bool rounded;
        vec2 size;
        float z_index;
        vec4 bg_col;
    } body;
    struct {
        vec2 size;
        float z_index;
        vec4 bg_col;
    } text;
    bool visible;
} ui_style_t;

typedef struct ui_component {
    ui_component_type_e kind;
    sprite_t **body_sprites;
    sprite_t **text_sprites;
    ui_transform_t transform;
    ui_style_t style;
    char text[UI_BUFLEN];
    void ( *cb)(struct ui_component *, void *);
    void *cb_args;

    vec2 _tr;
    vec2 _bl;
} ui_component_t;

typedef struct ui_button_desc {
    ui_transform_t transform;
    ui_style_t style;
    const char text[UI_BUFLEN];
    void ( *cb)(struct ui_component *, void *);
    void *cb_args;
} ui_button_desc_t;

typedef struct ui_label_desc {
    ui_transform_t transform;
    ui_style_t style;
    const char text[UI_BUFLEN];
} ui_label_desc_t;

extern void ui_make_button(ui_component_t *comp, sprite_renderer_t *sr, const ui_button_desc_t *desc);
extern void ui_make_label(ui_component_t *comp, sprite_renderer_t *sr, const ui_label_desc_t *desc);
extern bool ui_button_clicked(ui_component_t *comp, ivec2 screen_size, vec2 mouse_pos); 
extern void ui_show_component(ui_component_t *comp, bool show);
extern void ui_cleanup_component(ui_component_t *comp);

#endif
