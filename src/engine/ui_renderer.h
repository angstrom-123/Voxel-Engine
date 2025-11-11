#ifndef UI_RENDERER_H
#define UI_RENDERER_H 

#include "renderer_base.h"
#include "include_nuklear.h"
#include "ui_component.h"
#include "ui_system.h"
#include "logger.h"

typedef struct ui_renderer {
    renderer_base_t base;
    struct nk_context *ctx;
    ui_component_t **components;
    size_t component_count;
    bool state_set;
} ui_renderer_t;

typedef struct ui_renderer_desc {
    vec2 dimensions;
} ui_renderer_desc_t;

extern void ui_renderer_init(ui_renderer_t *uir, const ui_renderer_desc_t *desc);
extern void ui_renderer_cleanup(ui_renderer_t *uir);
extern void ui_renderer_render_all(ui_renderer_t *uir);

#endif
