#ifndef UI_COMPONENT_H
#define UI_COMPONENT_H

#include "sprite_renderer.h"

#define UI_BUFLEN 32

typedef enum ui_component_type {
    COMPONENT_BUTTON,
    COMPONENT_LABEL,
} ui_component_type_e;

typedef struct ui_transform {
} ui_transform_t;

typedef struct style {
    vec2 size;
    float z_index;
    vec4 bg_col;
} style_t;

typedef struct ui_component {
    ui_component_type_e kind;
    vec2 pos;
    uvec2 dim;
    char text[UI_BUFLEN];
    sprite_t **body_sprites;
    sprite_t **text_sprites;
    style_t body_style;
    style_t text_style;
    vec2 tr;
    vec2 bl;
    bool visible;
    union {
        struct {
            bool mini;
            bool rounded;
            void ( *cb)(struct ui_component *, void *);
            void *cb_args;
        } button;
        struct {
            int32_t value;
        } label;
    };
} ui_component_t;

typedef struct ui_button_desc {
    vec2 pos;
    uvec2 dim;
    style_t body_style;
    style_t text_style;
    bool mini;
    bool rounded;
    bool visible;
    const char text[UI_BUFLEN];
    void ( *cb)(struct ui_component *, void *);
    void *cb_args;
} ui_button_desc_t;

typedef struct ui_label_desc {
    vec2 pos;
    style_t text_style;
    bool visible;
    const char text[UI_BUFLEN];
} ui_label_desc_t;

extern void ui_make_button(ui_component_t *comp, sprite_renderer_t *sr, const ui_button_desc_t *desc);
extern void ui_make_label(ui_component_t *comp, sprite_renderer_t *sr, const ui_label_desc_t *desc);
extern bool ui_button_clicked(ui_component_t *comp, ivec2 screen_size, vec2 mouse_pos); 
extern void ui_show_component(ui_component_t *comp, bool show);
extern void ui_cleanup_component(ui_component_t *comp);

#endif
