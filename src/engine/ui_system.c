#include "ui_system.h"

static bool _mouse_over(vec2 tr, vec2 bl, ivec2 screen_size, vec2 mouse_pos)
{
    vec2 m_pos = VEC2(mouse_pos.x - screen_size.x / 2.0, screen_size.y / 2.0 - mouse_pos.y);
    if (m_pos.x > tr.x) return false;
    if (m_pos.x < bl.x) return false;
    if (m_pos.y > tr.y) return false;
    if (m_pos.y < bl.y) return false;
    return true;
}

static void _mouse_move(const event_t *ev, void *args)
{
    ui_system_t *uis = args;
    for (size_t i = 0; i < uis->comp_count; i++)
    {
        ui_component_t *c = &uis->components[i];
        bool before = c->hovered;

        // Only consider hovering for interactable components
        switch (c->kind) {
        case COMPONENT_BUTTON:
        case COMPONENT_SLIDER:
            if (_mouse_over(c->tr, c->bl, ev->framebuf_size, ev->mouse_pos))
                c->hovered = true;
            else c->hovered = false;
            break;
        default:
            continue;
        };
        
        // No state change, no need to iterate the sprites.
        if (c->hovered == before) 
            continue;

        for (size_t j = 0; j < c->dim.x * c->dim.y; j++)
            c->body_sprites[j]->bg_col = (c->hovered) 
                                       ? c->body_style.hover_bg_col 
                                       : c->body_style.bg_col;
    }
}

static void _mouse_click(const event_t *ev, void *args)
{
    (void) ev;
    ui_system_t *uis = args;
    for (size_t i = 0; i < uis->comp_count; i++)
    {
        ui_component_t *c = &uis->components[i];
        if (!c->hovered) continue;

        switch (c->kind) {
        case COMPONENT_BUTTON:
            if (c->cb != NULL) c->cb(c, c->cb_args);
            break;
        case COMPONENT_SLIDER:
            ENGINE_LOG_WARN("Slider Clicked, TODO: Implement slider sliding", NULL);
            break;
        default:
            continue;
        };
    }
}

void ui_sys_init(ui_system_t *uis, const ui_system_desc_t *desc)
{
    uis->max_comps = desc->max_comps;
    uis->comp_count = 0;
    uis->components = calloc(desc->max_comps, sizeof(ui_component_t));

    event_sys_subscribe_to_event(desc->es, EVENT_MOUSEMOVE, &(event_subscriber_desc_t) {
        .block_cb = event_block_never,
        .event_cb = _mouse_move,
        .args = uis
    });
    event_sys_subscribe_to_event(desc->es, EVENT_MOUSEDOWN, &(event_subscriber_desc_t) {
        .block_cb = event_block_never,
        .event_cb = _mouse_click,
        .args = uis
    });
}

void ui_sys_cleanup(ui_system_t *uis)
{
    for (size_t i = 0; i < uis->comp_count; i++)
    {
        if (uis->components[i].body_sprites) free(uis->components[i].body_sprites);
        if (uis->components[i].text_sprites) free(uis->components[i].text_sprites);
    }
    free(uis->components);
}

ui_handle_t ui_sys_make_button(ui_system_t *uis, sprite_renderer_t *sr, 
                               const ui_button_desc_t *desc)
{
    ENGINE_ASSERT(uis->comp_count < uis->max_comps, "Maximum components reached in ui system");

    ui_handle_t res = { .id = uis->comp_count };
    ui_component_t *comp = &uis->components[uis->comp_count++];

    comp->kind         = COMPONENT_BUTTON;
    comp->pos          = desc->pos;
    comp->dim          = desc->dim;
    comp->body_style   = desc->body_style;
    comp->text_style   = desc->text_style;
    comp->cb           = desc->cb;
    comp->cb_args      = desc->cb_args;
    comp->body_sprites = sprite_renderer_push_ui(sr, &(ui_sprites_desc_t) {
        .visible = desc->visible,
        .rounded = desc->rounded,
        .mini    = desc->mini,
        .bg_col  = comp->body_style.bg_col,
        .z_index = comp->body_style.z_index,
        .size    = comp->body_style.size,
        .pos     = comp->pos,
        .dim     = comp->dim
    });

    float body_width = comp->body_style.size.x * comp->dim.x;
    float body_height = comp->body_style.size.y * comp->dim.y;
    if (desc->mini)
    {
        body_width /= 2.0;
        body_height /= 2.0;
    }

    if (desc->text[0] == '\0')
    {
        comp->text[0] = '\0';
        comp->text_sprites = NULL;
    }
    else
    {
        strcpy(comp->text, desc->text);

        float text_width = comp->text_style.size.x * strlen(comp->text);
        float pad_left = (body_width - text_width) / 2.0;
        float pad_top = ((body_height - 2.0 * comp->body_style.size.y) 
                      + comp->text_style.size.y) / 2.0;
        comp->text_sprites = sprite_renderer_push_str(sr, desc->text, &(sprite_desc_t) {
            .visible = desc->visible,
            .bg_col  = comp->text_style.bg_col,
            .z_index = comp->text_style.z_index,
            .size    = comp->text_style.size,
            .pos     = VEC2(comp->pos.x + pad_left, comp->pos.y - pad_top)
        });
    }
    comp->tr = VEC2(comp->pos.x + body_width - 1.0, 
                    comp->pos.y + comp->body_style.size.y - 1.0);
    comp->bl = VEC2(comp->pos.x + 1.0, 
                    comp->pos.y - body_height + comp->body_style.size.y + 1.0);

    return res;
}

ui_handle_t ui_sys_make_label(ui_system_t *uis, sprite_renderer_t *sr, const ui_label_desc_t *desc)
{
    ENGINE_ASSERT(uis->comp_count < uis->max_comps, "Maximum components reached in ui system");

    ui_handle_t res = { .id = uis->comp_count };
    ui_component_t *comp = &uis->components[uis->comp_count++];

    comp->kind       = COMPONENT_LABEL;
    comp->pos        = desc->pos;
    comp->text_style = desc->text_style;

    if (desc->text[0] == '\0')
    {
        comp->text[0] = '\0';
        comp->text_sprites = NULL;
    }
    else 
    {
        strcpy(comp->text, desc->text);
        comp->text_sprites = sprite_renderer_push_str(sr, desc->text, &(sprite_desc_t) {
            .visible = desc->visible,
            .size    = comp->text_style.size,
            .z_index = comp->text_style.z_index,
            .pos     = VEC2(comp->pos.x, comp->pos.y - comp->text_style.size.y / 2.0),
            .bg_col  = comp->text_style.bg_col
        });
    }

    return res;
}

ui_handle_t ui_sys_make_container(ui_system_t *uis, sprite_renderer_t *sr, 
                                  const ui_container_desc_t *desc)
{
    ENGINE_ASSERT(uis->comp_count < uis->max_comps, "Maximum components reached in ui system");

    ui_handle_t res = { .id = uis->comp_count };
    ui_component_t *comp = &uis->components[uis->comp_count++];

    comp->kind         = COMPONENT_CONTAINER;
    comp->pos          = desc->pos;
    comp->dim          = desc->dim;
    comp->body_style   = desc->body_style;
    comp->text_style   = desc->text_style;
    comp->justify      = desc->justify;
    comp->body_sprites = sprite_renderer_push_ui(sr, &(ui_sprites_desc_t) {
        .visible = desc->visible,
        .rounded = desc->rounded,
        .mini    = desc->mini,
        .bg_col  = comp->body_style.bg_col,
        .z_index = comp->body_style.z_index,
        .size    = comp->body_style.size,
        .pos     = comp->pos,
        .dim     = comp->dim
    });

    float body_width = comp->body_style.size.x * comp->dim.x;
    float body_height = comp->body_style.size.y * comp->dim.y;
    if (desc->mini)
    {
        body_width /= 2.0;
        body_height /= 2.0;
    }

    if (desc->text[0] == '\0')
    {
        comp->text[0] = '\0';
        comp->text_sprites = NULL;
    }
    else
    {
        strcpy(comp->text, desc->text);

        float text_width = comp->text_style.size.x * strlen(comp->text);
        float pad_left = (body_width - text_width) / 2.0;
        float pad_top = 0.0; // JUSTIFY_TOP
        if (comp->justify == JUSTIFY_MIDDLE)
        {
            pad_top = ((body_height - 2.0 * comp->body_style.size.y) 
                    + comp->text_style.size.y) / 2.0;
        }
        comp->text_sprites = sprite_renderer_push_str(sr, desc->text, &(sprite_desc_t) {
            .visible = desc->visible,
            .bg_col  = comp->text_style.bg_col,
            .z_index = comp->text_style.z_index,
            .size    = comp->text_style.size,
            .pos     = VEC2(comp->pos.x + pad_left, comp->pos.y - pad_top)
        });
    }

    return res;
}

ui_handle_t ui_sys_make_slider(ui_system_t *uis, sprite_renderer_t *sr, 
                               const ui_slider_desc_t *desc)
{
    ENGINE_ASSERT(uis->comp_count < uis->max_comps, "Maximum components reached in ui system");

    ui_handle_t res = { .id = uis->comp_count };
    ui_component_t *comp = &uis->components[uis->comp_count++];

    comp->kind         = COMPONENT_SLIDER;
    comp->pos          = desc->pos;
    comp->dim          = UVEC2(desc->width, 2);
    comp->body_style   = desc->body_style;
    comp->text_style   = desc->text_style;
    comp->value        = desc->value;
    // TODO: Put value in string and push as sprites 
    //       Push the thumb sprites 
    //       Get the sliding around working
    //       Remember to bound it at the edges
    // comp->slider.value_sprites = sprite_renderer_push_str(sr, ito
    comp->body_sprites = sprite_renderer_push_ui(sr, &(ui_sprites_desc_t) {
        .visible = desc->visible,
        .rounded = desc->rounded,
        .mini    = desc->mini,
        .bg_col  = comp->body_style.bg_col,
        .z_index = comp->body_style.z_index,
        .size    = comp->body_style.size,
        .pos     = comp->pos,
        .dim     = comp->dim
    });

    float body_width = comp->body_style.size.x * comp->dim.x;
    float body_height = comp->body_style.size.y * comp->dim.y;
    if (desc->mini)
    {
        body_width /= 2.0;
        body_height /= 2.0;
    }

    if (desc->text[0] == '\0')
    {
        comp->text[0] = '\0';
        comp->text_sprites = NULL;
    }
    else
    {
        strcpy(comp->text, desc->text);

        float text_width = comp->text_style.size.x * strlen(comp->text);
        float pad_left = (body_width - text_width) / 2.0;
        float pad_top = ((body_height - 2.0 * comp->body_style.size.y) 
                      + comp->text_style.size.y) / 2.0;
        comp->text_sprites = sprite_renderer_push_str(sr, desc->text, &(sprite_desc_t) {
            .visible = desc->visible,
            .bg_col  = comp->text_style.bg_col,
            .z_index = comp->text_style.z_index,
            .size    = comp->text_style.size,
            .pos     = VEC2(comp->pos.x + pad_left, comp->pos.y - pad_top)
        });
    }
    comp->tr = VEC2(comp->pos.x + body_width, comp->pos.y + comp->body_style.size.y);
    comp->bl = VEC2(comp->pos.x, comp->pos.y - body_height + comp->body_style.size.y);

    return res;
}

void ui_sys_show_component(ui_system_t *uis, ui_handle_t handle, bool show)
{
    ui_component_t *comp = &uis->components[handle.id];

    switch (comp->kind) {
    case COMPONENT_BUTTON:
    case COMPONENT_CONTAINER:
        for (size_t i = 0; i < comp->dim.x * comp->dim.y; i++)
            comp->body_sprites[i]->visible = show;
        for (size_t i = 0; i < strlen(comp->text); i++)
            comp->text_sprites[i]->visible = show;
        break;
    case COMPONENT_LABEL:
        for (size_t i = 0; i < strlen(comp->text); i++)
            comp->text_sprites[i]->visible = show;
        break;
    case COMPONENT_SLIDER:
        for (size_t i = 0; i < comp->dim.x * comp->dim.y; i++)
            comp->body_sprites[i]->visible = show;
        for (size_t i = 0; i < strlen(comp->text); i++)
            comp->text_sprites[i]->visible = show;
        for (size_t i = 0; i < 2; i++)
            comp->thumb_sprites[i]->visible = show;
        for (size_t i = 0; i < 3; i++)
            comp->value_sprites[i]->visible = show;
        break;
    };
}
