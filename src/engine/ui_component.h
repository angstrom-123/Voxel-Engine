#ifndef UI_COMPONENT_H
#define UI_COMPONENT_H

#include <libem/em_math.h>

#if !defined(NK_NUKLEAR_H_) 
#include <nuklear/nuklear.h>
#endif

#include <stdbool.h>

typedef struct ui_component {
    void *ptr;
    void (*render)(struct nk_context *ctx, vec2 window_dimensions, struct ui_component *this);
    bool (*visible)(struct ui_component *);
} ui_component_t;

typedef struct ui_component_desc {
    void *ptr;
    void (*render_callback)(struct nk_context *, vec2, ui_component_t *);
    bool (*visible)(struct ui_component *);
} ui_component_desc_t;

extern bool (*visible_always)(ui_component_t *);

#define GET_UI_RECT(pos, siz) ((struct nk_rect) {pos.x, pos.y, siz.x, siz.y})

#endif
