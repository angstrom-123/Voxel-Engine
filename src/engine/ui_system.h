#ifndef UI_SYSTEM_H
#define UI_SYSTEM_H

#include "sprite_renderer.h"
#include "event_system.h"

#define UI_BUFLEN 32

typedef enum ui_component_type {
    COMPONENT_BUTTON,
    COMPONENT_LABEL,
    COMPONENT_CONTAINER,
    COMPONENT_SLIDER
} ui_component_type_e;

typedef struct style {
    vec2 size;
    float z_index;
    vec4 bg_col;
    vec4 hover_bg_col;
} style_t;

typedef enum justify {
    JUSTIFY_TOP,
    JUSTIFY_MIDDLE
} justify_e;

typedef struct ui_component {
    ui_component_type_e kind;
    vec2 pos;
    uvec2 dim;
    char text[UI_BUFLEN];
    sprite_t **body_sprites;
    sprite_t **text_sprites;
    style_t body_style;
    style_t text_style;
    // Interactables
    bool hovered;
    vec2 tr;
    vec2 bl;
    // Button
    void ( *cb)(struct ui_component *, void *);
    void *cb_args;
    // Slider 
    int32_t value;
    char value_text[3];
    sprite_t **thumb_sprites;
    sprite_t **value_sprites;
    // Container
    justify_e justify;
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

typedef struct ui_container_desc {
    vec2 pos;
    uvec2 dim;
    style_t body_style;
    style_t text_style;
    bool mini;
    bool rounded;
    bool visible;
    const char text[UI_BUFLEN];
    justify_e justify;
} ui_container_desc_t;

typedef struct ui_slider_desc {
    vec2 pos;
    uint32_t width;
    style_t body_style;
    style_t text_style;
    bool mini;
    bool rounded;
    bool visible;
    int32_t value;
    const char text[UI_BUFLEN];
} ui_slider_desc_t;

typedef struct ui_system {
    ui_component_t *components;
    size_t max_comps;
    size_t comp_count;
} ui_system_t;

typedef struct ui_system_desc {
    size_t max_comps;
    event_system_t *es;
} ui_system_desc_t;

typedef struct ui_handle {
    size_t id;
} ui_handle_t;

extern void ui_sys_init(ui_system_t *uis, const ui_system_desc_t *desc);
extern void ui_sys_cleanup(ui_system_t *uis);
extern ui_handle_t ui_sys_make_button(ui_system_t *uis, sprite_renderer_t *sr,
                                      const ui_button_desc_t *desc);
extern ui_handle_t ui_sys_make_label(ui_system_t *uis, sprite_renderer_t *sr,
                                     const ui_label_desc_t *desc);
extern ui_handle_t ui_sys_make_container(ui_system_t *uis, sprite_renderer_t *sr,
                                         const ui_container_desc_t *desc);
extern ui_handle_t ui_sys_make_slider(ui_system_t *uis, sprite_renderer_t *sr,
                                          const ui_slider_desc_t *desc);
extern void ui_sys_show_component(ui_system_t *uis, ui_handle_t comp, bool show);

#endif
