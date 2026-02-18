#ifndef UI_SYSTEM_H
#define UI_SYSTEM_H

#include "sprite_renderer.h"
#include "event_system.h"

#define UI_BUFLEN 32

typedef enum ui_component_type {
    COMPONENT_BUTTON,
    COMPONENT_LABEL,
    COMPONENT_CONTAINER,
    COMPONENT_SLIDER,
    COMPONENT_INPUT
} ui_component_type_e;

typedef struct style {
    float scale;
    float z_index;
    vec4 bg_col;
    vec4 tint_col;
    vec4 hover_bg_col;
    vec4 disabled_bg_col;
} style_t;

typedef enum justify {
    JUSTIFY_TOP,
    JUSTIFY_MIDDLE
} justify_e;

typedef enum input_filter {
    FILTER_NONE,
    FILTER_ALPHA,
    FILTER_NUMER
} input_filter_e;

typedef struct ui_handle {
    int32_t id;
} ui_handle_t;

typedef struct ui_component {
    ui_component_type_e kind;
    vec2 pos;
    uvec2 dim;
    char text[UI_BUFLEN];
    sprite_t **body_sprites;
    sprite_t **text_sprites;
    style_t body_style;
    style_t text_style;
    bool mini;
    // Interactables
    bool visible;
    bool hovered;
    vec2 tr;
    vec2 bl;
    // Button
    void ( *cb)(ui_handle_t, void *);
    void *cb_args;
    bool disabled;
    // Slider 
    int32_t value;
    char value_text[4]; // Includes null terminator
    sprite_t **thumb_sprites;
    sprite_t **value_sprites;
    vec2 thumb_origin;
    float thumb_x;
    float thumb_max_x;
    bool dragged;
    // Container
    justify_e justify;
    // Input 
    bool typing;
    size_t text_end;
    size_t cursor;
    size_t max_len;
    input_filter_e filter;
} ui_component_t;

typedef struct ui_button_desc {
    vec2 pos;
    uvec2 dim;
    style_t body_style;
    style_t text_style;
    bool mini;
    bool rounded;
    bool visible;
    bool disabled;
    char text[UI_BUFLEN];
    void ( *cb)(ui_handle_t, void *);
    void *cb_args;
} ui_button_desc_t;

typedef struct ui_label_desc {
    vec2 pos;
    style_t text_style;
    bool visible;
    char text[UI_BUFLEN];
} ui_label_desc_t;

typedef struct ui_container_desc {
    vec2 pos;
    uvec2 dim;
    style_t body_style;
    style_t text_style;
    bool mini;
    bool rounded;
    bool visible;
    char text[UI_BUFLEN];
    justify_e justify;
} ui_container_desc_t;

typedef struct ui_slider_desc {
    vec2 pos;
    uint32_t width;
    style_t body_style;
    style_t text_style;
    bool mini;
    bool visible;
    char text[UI_BUFLEN];
} ui_slider_desc_t;

typedef struct ui_input_desc {
    vec2 pos;
    uint32_t width;
    style_t body_style;
    style_t text_style;
    bool mini;
    bool rounded;
    bool visible;
    size_t max_len;
    input_filter_e filter;
} ui_input_desc_t;

typedef struct ui_system {
    ui_component_t *components;
    int32_t max_comps;
    int32_t comp_count;
    sprite_renderer_t *sr;
    bool mouse_active;

    struct ui_system_event_args {
        struct ui_system *uis;
        sprite_renderer_t *sr;
    } ev_args;

    char _init_buffer[UI_BUFLEN];
} ui_system_t;

typedef struct ui_system_event_args ui_system_event_args_t;

typedef struct ui_system_desc {
    int32_t max_comps;
    event_system_t *es;
    sprite_renderer_t *sr;
} ui_system_desc_t;

#define UI_INVALID_HANDLE_ID -1

extern void ui_sys_init(ui_system_t *uis, const ui_system_desc_t *desc);
extern void ui_sys_cleanup(ui_system_t *uis);
extern ui_handle_t ui_sys_make_button(ui_system_t *uis, const ui_button_desc_t *desc);
extern ui_handle_t ui_sys_make_label(ui_system_t *uis, const ui_label_desc_t *desc);
extern ui_handle_t ui_sys_make_container(ui_system_t *uis, const ui_container_desc_t *desc);
extern ui_handle_t ui_sys_make_slider(ui_system_t *uis, const ui_slider_desc_t *desc);
extern ui_handle_t ui_sys_make_input(ui_system_t *uis, const ui_input_desc_t *desc);
extern void ui_sys_show_component(ui_system_t *uis, ui_handle_t handle, bool show);
extern void ui_sys_init_handle_buffer(size_t count, ui_handle_t *buffer);
extern void ui_sys_query_text(ui_system_t *uis, ui_handle_t handle, char res[UI_BUFLEN]);
extern void ui_sys_enable_component(ui_system_t *uis, ui_handle_t handle, bool enable);
extern void ui_sys_set_component_text(ui_system_t *uis, ui_handle_t handle, char *text);

#endif
