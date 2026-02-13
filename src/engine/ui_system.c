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

static bool _mouse_move(const event_t *ev, void *args)
{
    ui_system_event_args_t *ev_args = args;
    ui_system_t *uis = ev_args->uis;
    sprite_renderer_t *sr = ev_args->sr;
    for (size_t i = 0; i < uis->comp_count; i++)
    {
        ui_component_t *c = &uis->components[i];
        if (!c->visible) 
        {
            c->hovered = false;
            continue;
        }

        bool before = c->hovered;

        // Only consider hovering for interactable components
        switch (c->kind) {
        case COMPONENT_BUTTON:
            if (_mouse_over(c->tr, c->bl, ev->framebuf_size, ev->mouse_pos))
                c->hovered = true;
            else c->hovered = false;

            // No state change, no need to iterate the sprites.
            if (c->hovered == before) 
                continue;

            for (size_t j = 0; j < c->dim.x * c->dim.y; j++)
                c->body_sprites[j]->bg_col = (c->hovered) 
                                           ? c->body_style.hover_bg_col 
                                           : c->body_style.bg_col;
            break;
        case COMPONENT_SLIDER:
            if (_mouse_over(c->tr, c->bl, ev->framebuf_size, ev->mouse_pos))
                c->hovered = true;
            else if (!c->dragged) c->hovered = false;

            // No state change, no need to iterate the sprites.
            if (c->hovered != before) 
            {
                vec4 col = (c->hovered) ? c->body_style.hover_bg_col : c->body_style.bg_col;
                c->thumb_sprites[0]->bg_col = col;
                c->thumb_sprites[1]->bg_col = col;
            }

            if (c->dragged)
            {
                float old_pos = c->thumb_x;
                c->thumb_x += ev->mouse_delta.x;
                c->thumb_x = em_clamp(c->thumb_x, 0.0, c->thumb_max_x);
                vec2 thumb_pos = VEC2(c->thumb_origin.x + c->thumb_x, c->thumb_origin.y);
                sprite_renderer_move(sr, c->thumb_sprites[0], thumb_pos);
                thumb_pos.y -= (c->mini) ? c->body_style.size.y / 2.0 : c->body_style.size.y;
                sprite_renderer_move(sr, c->thumb_sprites[1], thumb_pos);

                c->bl.x += c->thumb_x - old_pos;
                c->tr.x += c->thumb_x - old_pos;

                snprintf(c->value_text, 4, "%3i", 
                         (int32_t) roundf(c->thumb_x / c->thumb_max_x * 100.0));
                sprite_renderer_change_str(sr, c->value_sprites, c->value_text);
            }
            break;
        default:
            continue;
        };
        
    }

    return false;
}

static bool _mouse_click(const event_t *ev, void *args)
{
    (void) ev;
    ui_system_event_args_t *ev_args = args;
    ui_system_t *uis = ev_args->uis;
    for (size_t i = 0; i < uis->comp_count; i++)
    {
        ui_component_t *c = &uis->components[i];
        if (!c->hovered) continue;

        switch (c->kind) {
        case COMPONENT_BUTTON:
            if (c->cb != NULL) 
            {
                c->cb(c, c->cb_args);
                return true;
            }
            break;
        case COMPONENT_SLIDER:
            sapp_show_mouse(false);
            c->dragged = true;
            break;
        default:
            continue;
        };
    }

    return false;
}

static bool _mouse_up(const event_t *ev, void *args)
{
    (void) ev;
    ui_system_event_args_t *ev_args = args;
    ui_system_t *uis = ev_args->uis;
    for (size_t i = 0; i < uis->comp_count; i++)
    {
        ui_component_t *c = &uis->components[i];
        if (!c->dragged) continue;

        switch (c->kind) {
        case COMPONENT_SLIDER:
            sapp_show_mouse(true);
            c->dragged = false;
            break;
        default:
            continue;
        };
    }

    return false;
}

void ui_sys_init(ui_system_t *uis, const ui_system_desc_t *desc)
{
    uis->max_comps = desc->max_comps;
    uis->comp_count = 0;
    uis->components = calloc(desc->max_comps, sizeof(ui_component_t));
    uis->sr = desc->sr;
    uis->ev_args = (ui_system_event_args_t) {
        .sr = desc->sr,
        .uis = uis
    };

    event_sys_subscribe_to_event(desc->es, EVENT_MOUSEMOVE, &(event_subscriber_desc_t) {
        .block_cb = event_block_never,
        .event_cb = _mouse_move,
        .args = &uis->ev_args
    });
    event_sys_subscribe_to_event(desc->es, EVENT_MOUSEDOWN, &(event_subscriber_desc_t) {
        .block_cb = event_block_never,
        .event_cb = _mouse_click,
        .args = &uis->ev_args
    });
    event_sys_subscribe_to_event(desc->es, EVENT_MOUSEUP, &(event_subscriber_desc_t) {
        .block_cb = event_block_never,
        .event_cb = _mouse_up,
        .args = &uis->ev_args
    });
}

void ui_sys_cleanup(ui_system_t *uis)
{
    for (size_t i = 0; i < uis->comp_count; i++)
    {
        ui_component_t *c = &uis->components[i];
        if (c->body_sprites) free(uis->components[i].body_sprites);
        if (c->text_sprites) free(uis->components[i].text_sprites);
        if (c->thumb_sprites) free(uis->components[i].thumb_sprites);
        if (c->value_sprites) free(uis->components[i].value_sprites);
    }
    free(uis->components);
}

ui_handle_t ui_sys_make_button(ui_system_t *uis, const ui_button_desc_t *desc)
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
    comp->visible      = desc->visible;
    comp->mini         = desc->mini;
    comp->body_sprites = sprite_renderer_push_ui(uis->sr, &(ui_sprites_desc_t) {
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
        comp->text_sprites = sprite_renderer_push_str(uis->sr, desc->text, &(sprite_desc_t) {
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

ui_handle_t ui_sys_make_label(ui_system_t *uis, const ui_label_desc_t *desc)
{
    ENGINE_ASSERT(uis->comp_count < uis->max_comps, 
                  "Maximum components reached in ui system");

    ui_handle_t res = { .id = uis->comp_count };
    ui_component_t *comp = &uis->components[uis->comp_count++];

    comp->kind       = COMPONENT_LABEL;
    comp->pos        = desc->pos;
    comp->text_style = desc->text_style;
    comp->visible    = desc->visible;

    if (desc->text[0] == '\0')
    {
        comp->text[0] = '\0';
        comp->text_sprites = NULL;
    }
    else 
    {
        strcpy(comp->text, desc->text);
        comp->text_sprites = sprite_renderer_push_str(uis->sr, desc->text, &(sprite_desc_t) {
            .visible = desc->visible,
            .size    = comp->text_style.size,
            .z_index = comp->text_style.z_index,
            .pos     = VEC2(comp->pos.x, comp->pos.y - comp->text_style.size.y / 2.0),
            .bg_col  = comp->text_style.bg_col
        });
    }

    return res;
}

ui_handle_t ui_sys_make_container(ui_system_t *uis, const ui_container_desc_t *desc)
{
    ENGINE_ASSERT(uis->comp_count < uis->max_comps, 
                  "Maximum components reached in ui system");

    ui_handle_t res = { .id = uis->comp_count };
    ui_component_t *comp = &uis->components[uis->comp_count++];

    comp->kind         = COMPONENT_CONTAINER;
    comp->pos          = desc->pos;
    comp->dim          = desc->dim;
    comp->body_style   = desc->body_style;
    comp->text_style   = desc->text_style;
    comp->justify      = desc->justify;
    comp->visible      = desc->visible;
    comp->mini         = desc->mini;
    comp->body_sprites = sprite_renderer_push_ui(uis->sr, &(ui_sprites_desc_t) {
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
        comp->text_sprites = sprite_renderer_push_str(uis->sr, desc->text, &(sprite_desc_t) {
            .visible = desc->visible,
            .bg_col  = comp->text_style.bg_col,
            .z_index = comp->text_style.z_index,
            .size    = comp->text_style.size,
            .pos     = VEC2(comp->pos.x + pad_left, comp->pos.y - pad_top)
        });
    }

    return res;
}

ui_handle_t ui_sys_make_slider(ui_system_t *uis, const ui_slider_desc_t *desc)
{
    ENGINE_ASSERT(uis->comp_count < uis->max_comps, 
                  "Maximum components reached in ui system");

    ui_handle_t res = { .id = uis->comp_count };
    ui_component_t *comp = &uis->components[uis->comp_count++];

    comp->kind          = COMPONENT_SLIDER;
    comp->pos           = desc->pos;
    comp->dim           = UVEC2(desc->width, 2);
    comp->body_style    = desc->body_style;
    comp->text_style    = desc->text_style;
    comp->visible       = desc->visible;
    comp->mini          = desc->mini;
    comp->thumb_x       = 0.0;
    comp->thumb_max_x   = desc->width * desc->body_style.size.x 
                        - THUMB_SIZE.x * (desc->body_style.size.x / 16.0);
    comp->thumb_origin  = desc->pos;
    comp->thumb_sprites = sprite_renderer_push_thumb(uis->sr, &(ui_sprites_desc_t) {
        .visible = desc->visible,
        .bg_col  = desc->body_style.bg_col,
        .pos     = desc->pos,
        .size    = desc->body_style.size,
        .mini    = desc->mini,
        .z_index = desc->body_style.z_index + 0.1
    });
    comp->body_sprites  = sprite_renderer_push_ui(uis->sr, &(ui_sprites_desc_t) {
        .visible = desc->visible,
        .rounded = false,
        .mini    = desc->mini,
        .bg_col  = comp->body_style.bg_col,
        .z_index = comp->body_style.z_index,
        .size    = comp->body_style.size,
        .pos     = comp->pos,
        .dim     = comp->dim
    });

    strncpy(comp->value_text, "  0", 3);

    float body_width = comp->body_style.size.x * comp->dim.x;
    float body_height = comp->body_style.size.y * comp->dim.y;
    if (desc->mini)
    {
        body_width /= 2.0;
        body_height /= 2.0;
    }

    float pad_top = ((body_height - 2.0 * comp->body_style.size.y) 
                  + comp->text_style.size.y) / 2.0;

    if (desc->text[0] == '\0')
    {
        comp->text[0] = '\0';
        comp->text_sprites = NULL;

        float text_width = comp->text_style.size.x * 3;
        float pad_left = (body_width - text_width) / 2.0;
        comp->value_sprites = sprite_renderer_push_str(uis->sr, comp->value_text, &(sprite_desc_t) {
            .visible = desc->visible,
            .bg_col  = desc->text_style.bg_col,
            .size    = desc->text_style.size,
            .z_index = desc->text_style.z_index,
            .pos     = VEC2(desc->pos.x + pad_left, desc->pos.y - pad_top)
        });
    }
    else
    {
        strcpy(comp->text, desc->text);

        size_t text_len = strlen(comp->text);
        float text_width = comp->text_style.size.x * (text_len + 3);
        float pad_left = (body_width - text_width) / 2.0;
        comp->text_sprites = sprite_renderer_push_str(uis->sr, desc->text, 
                                                      &(sprite_desc_t) {
            .visible = desc->visible,
            .bg_col  = comp->text_style.bg_col,
            .z_index = comp->text_style.z_index,
            .size    = comp->text_style.size,
            .pos     = VEC2(comp->pos.x + pad_left, comp->pos.y - pad_top)
        });

        pad_left += text_len * comp->text_style.size.x;
        comp->value_sprites = sprite_renderer_push_str(uis->sr, comp->value_text, 
                                                       &(sprite_desc_t) {
            .visible = desc->visible,
            .bg_col  = desc->text_style.bg_col,
            .size    = desc->text_style.size,
            .z_index = desc->text_style.z_index,
            .pos     = VEC2(desc->pos.x + pad_left, desc->pos.y - pad_top)
        });
    }

    vec2 thumb_size = em_mul_vec2((desc->mini ? THUMB_S_SIZE : THUMB_SIZE), 
                                  em_div_vec2_f(desc->body_style.size, 16.0));
    comp->tr = VEC2(comp->pos.x + thumb_size.x, comp->pos.y + thumb_size.y / 2.0);
    comp->bl = VEC2(comp->pos.x, comp->pos.y - thumb_size.y / 2.0);
    // TODO
    // comp->tr = VEC2(comp->pos.x + body_width, comp->pos.y + comp->body_style.size.y);
    // comp->bl = VEC2(comp->pos.x, comp->pos.y - body_height + comp->body_style.size.y);

    return res;
}

void ui_sys_show_component(ui_system_t *uis, ui_handle_t handle, bool show)
{
    ui_component_t *comp = &uis->components[handle.id];
    comp->visible = show;

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
