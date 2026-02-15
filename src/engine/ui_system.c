#include "ui_system.h"

static char _get_char(keycode_e kc, bool shift)
{
    RUNTIME_ASSERT(kc <= KEYCODE_GRAVE_ACCENT, "Invalid keycode for character conversion");

    iitvl alpha = { .min = KEYCODE_A, .max = KEYCODE_Z };
    iitvl numer = { .min = KEYCODE_0, .max = KEYCODE_9 };
    if (em_iinterval_contains(alpha, kc))
    {
        if (shift) return (char) kc;
        return (char) (kc + 32);
    }
    else if (!shift)
    {
        return (char) kc;
    }
    else if (em_iinterval_contains(numer, kc))
    {
        switch (kc) {
        case KEYCODE_0: return ')';
        case KEYCODE_1: return '!';
        case KEYCODE_2: return '"';
        case KEYCODE_3: return '#';
        case KEYCODE_4: return '$';
        case KEYCODE_5: return '%';
        case KEYCODE_6: return '^';
        case KEYCODE_7: return '&';
        case KEYCODE_8: return '*';
        case KEYCODE_9: return '(';
        default:        RUNTIME_ASSERT(false, "Unreachable");
        };
    }
    else 
    {
        switch (kc) {
        case KEYCODE_SPACE:         return ' ';
        case KEYCODE_APOSTROPHE:    return '@';
        case KEYCODE_COMMA:         return '<';
        case KEYCODE_MINUS:         return '_';
        case KEYCODE_PERIOD:        return '>';
        case KEYCODE_SLASH:         return '?';
        case KEYCODE_SEMICOLON:     return ':';
        case KEYCODE_EQUAL:         return '+';
        case KEYCODE_LEFT_BRACKET:  return '{';
        case KEYCODE_BACKSLASH:     return '|';
        case KEYCODE_RIGHT_BRACKET: return '}';
        default:
            return (char) kc;
        };
    }

    return (char) kc;
}

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
            c->dragged = false;
            continue;
        }

        bool before = c->hovered;

        // Only consider hovering for interactable components
        switch (c->kind) {
        case COMPONENT_BUTTON:
            c->hovered = uis->mouse_active &&
                         _mouse_over(c->tr, c->bl, ev->framebuf_size, ev->mouse_pos);

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
                thumb_pos.y -= (c->mini ? SPRITE_S_SIZE.y : SPRITE_SIZE.y) 
                               * c->body_style.scale;
                sprite_renderer_move(sr, c->thumb_sprites[1], thumb_pos);

                c->bl.x += c->thumb_x - old_pos;
                c->tr.x += c->thumb_x - old_pos;

                snprintf(c->value_text, 4, "%3i", 
                         (int32_t) roundf(c->thumb_x / c->thumb_max_x * 100.0));
                sprite_renderer_change_str(sr, c->value_sprites, c->value_text);
            }
            break;
        case COMPONENT_INPUT:
            c->hovered = uis->mouse_active &&
                         _mouse_over(c->tr, c->bl, ev->framebuf_size, ev->mouse_pos);
            for (size_t j = 0; j < c->dim.x * c->dim.y; j++)
                c->body_sprites[j]->bg_col = (c->hovered) 
                                           ? c->body_style.hover_bg_col 
                                           : c->body_style.bg_col;
            break;
        default:
            break;
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
        if (!c->hovered) 
        {
            c->typing = false;
            continue;
        }

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
            uis->mouse_active = false;
            break;
        case COMPONENT_INPUT:
            c->typing = true;
            break;
        default:
            break;
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
            uis->mouse_active = true;
            break;
        default:
            break;
        };
    }

    return false;
}

bool _key_down(const event_t *ev, void *args)
{
    ui_system_event_args_t *ev_args = args;
    ui_system_t *uis = ev_args->uis;
    sprite_renderer_t *sr = ev_args->sr;
    for (size_t i = 0; i < uis->comp_count; i++)
    {
        ui_component_t *c = &uis->components[i];
        if (!c->typing || !c->visible) continue;

        switch (c->kind) {
        case COMPONENT_INPUT:
        {
            // ev->modifiers
            switch (ev->keycode) {
            case KEYCODE_LEFT:
                ENGINE_TODO("Add support for left move in text input");
                break;
            case KEYCODE_RIGHT:
                ENGINE_TODO("Add support for right move in text input");
                break;
            case KEYCODE_BACKSPACE:
                if (c->cursor > 0)
                {
                    c->text[--c->cursor] = ' ';
                    sprite_renderer_change_str(sr, c->text_sprites, c->text);
                }
                break;
            default:
                if (ev->keycode <= KEYCODE_GRAVE_ACCENT && 
                    c->cursor < em_min(UI_BUFLEN, c->max_len)) 
                {
                    char key = _get_char(ev->keycode, (ev->modifiers & MODIFIER_SHIFT));
                    c->text[c->cursor++] = key;
                    sprite_renderer_change_str(sr, c->text_sprites, c->text);
                }
                break;
            };
            break;
        }
        default:
            break;
        };
    }
    return false;
}

void ui_sys_init(ui_system_t *uis, const ui_system_desc_t *desc)
{
    uis->mouse_active = true;
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
    event_sys_subscribe_to_event(desc->es, EVENT_KEYDOWN, &(event_subscriber_desc_t) {
        .block_cb = event_block_never,
        .event_cb = _key_down,
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
    ENGINE_ASSERT(uis->comp_count < uis->max_comps, 
                  "Maximum components reached in ui system");

    const vec2 spr_size = (desc->mini) ? SPRITE_S_SIZE : SPRITE_SIZE;

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
        .scale   = comp->body_style.scale,
        .pos     = comp->pos,
        .dim     = comp->dim
    });

    vec2 body_size = em_mul_vec2_f(em_mul_vec2(spr_size, AS_VEC2(desc->dim)),
                                   desc->body_style.scale);
    vec2 text_size = em_mul_vec2_f(CHAR_SIZE, desc->text_style.scale);
    text_size.x *= strlen(desc->text);

    if (desc->text[0] == '\0')
    {
        comp->text[0] = '\0';
        comp->text_sprites = NULL;
    }
    else
    {
        strcpy(comp->text, desc->text);

        float pad_left = (body_size.x - text_size.x) / 2.0;
        float top_ofst = 2.0 * desc->body_style.scale * spr_size.y;
        float pad_top = (body_size.y - top_ofst + text_size.y) / 2.0;
        if (desc->mini) pad_top -= desc->body_style.scale * spr_size.y;
        comp->text_sprites = sprite_renderer_push_str(uis->sr, desc->text, 
                                                      &(sprite_desc_t) {
            .visible = desc->visible,
            .bg_col  = comp->text_style.bg_col,
            .z_index = comp->text_style.z_index,
            .scale   = comp->text_style.scale,
            .pos     = VEC2(comp->pos.x + pad_left, comp->pos.y - pad_top)
        });
    }
    vec2 epsilon = VEC2(1.0, 1.0);
    vec2 tr_ofst = VEC2(body_size.x, desc->body_style.scale * spr_size.y);
    vec2 bl_ofst = VEC2(0.0, - body_size.y + desc->body_style.scale * spr_size.y);

    comp->tr = em_sub_vec2(em_add_vec2(comp->pos, tr_ofst), epsilon);
    comp->bl = em_add_vec2(em_add_vec2(comp->pos, bl_ofst), epsilon);

    if (desc->mini)
    {
        comp->tr.y += desc->body_style.scale * spr_size.y;
        comp->bl.y += desc->body_style.scale * spr_size.y;
    }

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
            .scale   = comp->text_style.scale,
            .z_index = comp->text_style.z_index,
            .pos     = VEC2(comp->pos.x, 
                            comp->pos.y - (comp->text_style.scale * CHAR_SIZE.y) / 2.0),
            .bg_col  = comp->text_style.bg_col
        });
    }

    return res;
}

ui_handle_t ui_sys_make_container(ui_system_t *uis, const ui_container_desc_t *desc)
{
    ENGINE_ASSERT(uis->comp_count < uis->max_comps, 
                  "Maximum components reached in ui system");

    const vec2 spr_size = (desc->mini) ? SPRITE_S_SIZE : SPRITE_SIZE;

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
        .scale   = comp->body_style.scale,
        .pos     = comp->pos,
        .dim     = comp->dim
    });

    vec2 body_size = em_mul_vec2_f(em_mul_vec2(spr_size, AS_VEC2(desc->dim)),
                                   desc->body_style.scale);
    vec2 text_size = em_mul_vec2_f(CHAR_SIZE, desc->text_style.scale);
    text_size.x *= strlen(desc->text);

    if (desc->text[0] == '\0')
    {
        comp->text[0] = '\0';
        comp->text_sprites = NULL;
    }
    else
    {
        strcpy(comp->text, desc->text);

        float pad_left = (body_size.x - text_size.x) / 2.0;
        float pad_top = 0.0; // JUSTIFY_TOP
        if (comp->justify == JUSTIFY_MIDDLE)
        {
            float top_ofst = 2.0 * desc->body_style.scale * spr_size.y;
            pad_top = (body_size.y - top_ofst + text_size.y) / 2.0;
        }
        comp->text_sprites = sprite_renderer_push_str(uis->sr, desc->text, &(sprite_desc_t) {
            .visible = desc->visible,
            .bg_col  = comp->text_style.bg_col,
            .z_index = comp->text_style.z_index,
            .scale   = comp->text_style.scale,
            .pos     = VEC2(comp->pos.x + pad_left, comp->pos.y - pad_top)
        });
    }

    return res;
}

ui_handle_t ui_sys_make_slider(ui_system_t *uis, const ui_slider_desc_t *desc)
{
    ENGINE_ASSERT(uis->comp_count < uis->max_comps, 
                  "Maximum components reached in ui system");

    const vec2 spr_size = (desc->mini) ? SPRITE_S_SIZE : SPRITE_SIZE;
    const vec2 thumb_size = (desc->mini) ? THUMB_S_SIZE : THUMB_SIZE;

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
    comp->thumb_max_x   = (desc->width * spr_size.x - thumb_size.x) 
                        * desc->body_style.scale;
    comp->thumb_origin  = desc->pos;
    comp->thumb_sprites = sprite_renderer_push_thumb(uis->sr, &(ui_sprites_desc_t) {
        .visible = desc->visible,
        .bg_col  = desc->body_style.bg_col,
        .pos     = desc->pos,
        .scale   = desc->body_style.scale,
        .mini    = desc->mini,
        .z_index = desc->body_style.z_index + 0.1
    });
    comp->body_sprites  = sprite_renderer_push_ui(uis->sr, &(ui_sprites_desc_t) {
        .visible = desc->visible,
        .rounded = false,
        .mini    = desc->mini,
        .bg_col  = comp->body_style.bg_col,
        .z_index = comp->body_style.z_index,
        .scale   = comp->body_style.scale,
        .pos     = comp->pos,
        .dim     = comp->dim
    });

    strncpy(comp->value_text, "  0", 3);

    vec2 body_size = VEC2(spr_size.x * desc->body_style.scale * desc->width,
                          spr_size.y * desc->body_style.scale * 2.0);
    vec2 text_size = em_mul_vec2_f(CHAR_SIZE, desc->text_style.scale);
    text_size.x *= (strlen(desc->text) + 3.0);

    float pad_left = (body_size.x - text_size.x) / 2.0;
    float top_ofst = 2.0 * desc->body_style.scale * spr_size.y;
    float pad_top = (body_size.y - top_ofst + text_size.y) / 2.0;
    if (desc->mini) pad_top -= desc->body_style.scale * spr_size.y;

    if (desc->text[0] == '\0')
    {
        comp->text[0] = '\0';
        comp->text_sprites = NULL;

        comp->value_sprites = sprite_renderer_push_str(uis->sr, comp->value_text, 
                                                       &(sprite_desc_t) {
            .visible = desc->visible,
            .bg_col  = desc->text_style.bg_col,
            .scale   = desc->text_style.scale,
            .z_index = desc->text_style.z_index,
            .pos     = VEC2(desc->pos.x + pad_left, desc->pos.y - pad_top)
        });
    }
    else
    {
        strcpy(comp->text, desc->text);

        comp->text_sprites = sprite_renderer_push_str(uis->sr, desc->text, 
                                                      &(sprite_desc_t) {
            .visible = desc->visible,
            .bg_col  = comp->text_style.bg_col,
            .z_index = comp->text_style.z_index,
            .scale   = comp->text_style.scale,
            .pos     = VEC2(comp->pos.x + pad_left, comp->pos.y - pad_top)
        });

        pad_left += text_size.x - (3.0 * CHAR_SIZE.x * desc->text_style.scale);
        comp->value_sprites = sprite_renderer_push_str(uis->sr, comp->value_text, 
                                                       &(sprite_desc_t) {
            .visible = desc->visible,
            .bg_col  = desc->text_style.bg_col,
            .scale   = desc->text_style.scale,
            .z_index = desc->text_style.z_index,
            .pos     = VEC2(desc->pos.x + pad_left, desc->pos.y - pad_top)
        });
    }

    vec2 thumb_scaled = em_mul_vec2_f(thumb_size, desc->body_style.scale);
    comp->tr = VEC2(comp->pos.x + thumb_scaled.x, 
                    comp->pos.y + thumb_scaled.y / 2.0);
    comp->bl = VEC2(comp->pos.x, 
                    comp->pos.y - thumb_scaled.y / 2.0);
    if (desc->mini)
    {
        comp->tr.y += spr_size.y * desc->body_style.scale;
        comp->bl.y += spr_size.y * desc->body_style.scale;
    }
    return res;
}

ui_handle_t ui_sys_make_input(ui_system_t *uis, const ui_input_desc_t *desc)
{
    ENGINE_ASSERT(uis->comp_count < uis->max_comps, 
                  "Maximum components reached in ui system");

    const vec2 spr_size = (desc->mini) ? SPRITE_S_SIZE : SPRITE_SIZE;

    ui_handle_t res = { .id = uis->comp_count };
    ui_component_t *comp = &uis->components[uis->comp_count++];

    comp->kind         = COMPONENT_INPUT;
    comp->pos          = desc->pos;
    comp->dim          = UVEC2(desc->width, 2);
    comp->body_style   = desc->body_style;
    comp->text_style   = desc->text_style;
    comp->visible      = desc->visible;
    comp->mini         = desc->mini;
    comp->max_len      = desc->max_len;
    comp->body_sprites = sprite_renderer_push_ui(uis->sr, &(ui_sprites_desc_t) {
        .visible = desc->visible,
        .mini    = desc->mini,
        .rounded = desc->rounded,
        .bg_col  = comp->body_style.bg_col,
        .z_index = comp->body_style.z_index,
        .scale   = comp->body_style.scale,
        .pos     = comp->pos,
        .dim     = comp->dim
    });

    vec2 body_size = em_mul_vec2_f(em_mul_vec2(spr_size, AS_VEC2(comp->dim)),
                                   desc->body_style.scale);
    vec2 text_size = em_mul_vec2_f(CHAR_SIZE, desc->text_style.scale);
    text_size.x *= desc->max_len;

    float pad_left = (body_size.x - text_size.x) / 2.0;
    float top_ofst = 2.0 * desc->body_style.scale * spr_size.y;
    float pad_top = (body_size.y - top_ofst + text_size.y) / 2.0;
    if (desc->mini) pad_top -= desc->body_style.scale * spr_size.y;

    char tmp[desc->max_len + 1] = {};
    memset(tmp, '?', desc->max_len);
    comp->text_sprites = sprite_renderer_push_str(uis->sr, tmp, &(sprite_desc_t) {
        .visible = desc->visible,
        .bg_col  = comp->text_style.bg_col,
        .z_index = comp->text_style.z_index,
        .scale   = comp->text_style.scale,
        .pos     = VEC2(comp->pos.x + pad_left, comp->pos.y - pad_top)
    });
    memset(tmp, ' ', sizeof(tmp) - 1);
    strcpy(comp->text, tmp);
    sprite_renderer_change_str(uis->sr, comp->text_sprites, tmp);

    vec2 epsilon = VEC2(1.0, 1.0);
    vec2 tr_ofst = VEC2(body_size.x, desc->body_style.scale * spr_size.y);
    vec2 bl_ofst = VEC2(0.0, - body_size.y + desc->body_style.scale * spr_size.y);

    comp->tr = em_sub_vec2(em_add_vec2(comp->pos, tr_ofst), epsilon);
    comp->bl = em_add_vec2(em_add_vec2(comp->pos, bl_ofst), epsilon);

    if (desc->mini)
    {
        comp->tr.y += desc->body_style.scale * spr_size.y;
        comp->bl.y += desc->body_style.scale * spr_size.y;
    }

    return res;
}

void ui_sys_show_component(ui_system_t *uis, ui_handle_t handle, bool show)
{
    ui_component_t *comp = &uis->components[handle.id];
    comp->visible = show;

    switch (comp->kind) {
    case COMPONENT_BUTTON:
    case COMPONENT_CONTAINER:
    case COMPONENT_INPUT:
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
