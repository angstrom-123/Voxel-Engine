#include "ui_component.h"

void ui_make_button(ui_component_t *comp, sprite_renderer_t *sr, const ui_button_desc_t *desc)
{
    comp->kind           = COMPONENT_BUTTON;
    comp->pos            = desc->pos;
    comp->dim            = desc->dim;
    comp->body_style     = desc->body_style;
    comp->text_style     = desc->text_style;
    comp->button.cb      = desc->cb;
    comp->button.cb_args = desc->cb_args;
    comp->visible        = desc->visible;
    comp->button.rounded = desc->rounded;
    comp->button.mini    = desc->mini;
    comp->body_sprites   = sprite_renderer_push_ui(sr, &(ui_sprites_desc_t) {
        .visible = comp->visible,
        .rounded = comp->button.rounded,
        .mini    = comp->button.mini,
        .bg_col  = comp->body_style.bg_col,
        .z_index = comp->body_style.z_index,
        .size    = comp->body_style.size,
        .pos     = comp->pos,
        .dim     = comp->dim
    });

    float body_width = comp->body_style.size.x * comp->dim.x;
    float body_height = comp->body_style.size.y * comp->dim.y;
    if (comp->button.mini)
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
            .visible = comp->visible,
            .bg_col  = comp->text_style.bg_col,
            .z_index = comp->text_style.z_index,
            .size    = comp->text_style.size,
            .pos     = VEC2(comp->pos.x + pad_left, comp->pos.y - pad_top)
        });
    }
    comp->tr = VEC2(comp->pos.x + body_width, comp->pos.y + comp->body_style.size.y);
    comp->bl = VEC2(comp->pos.x, comp->pos.y - body_height + comp->body_style.size.y);
}

void ui_make_label(ui_component_t *comp, sprite_renderer_t *sr, const ui_label_desc_t *desc)
{
    comp->kind         = COMPONENT_LABEL;
    comp->pos          = desc->pos;
    comp->text_style   = desc->text_style;
    comp->visible      = desc->visible;

    if (desc->text[0] == '\0')
    {
        comp->text[0] = '\0';
        comp->text_sprites = NULL;
    }
    else 
    {
        strcpy(comp->text, desc->text);
        comp->text_sprites = sprite_renderer_push_str(sr, desc->text, &(sprite_desc_t) {
            .visible = comp->visible,
            .size    = comp->text_style.size,
            .z_index = comp->text_style.z_index,
            .pos     = VEC2(comp->pos.x, comp->pos.y - comp->text_style.size.y / 2.0),
            .bg_col  = VEC4(0.0, 0.0, 0.0, 0.0),
        });
    }
}

bool ui_button_clicked(ui_component_t *comp, ivec2 screen_size, vec2 mouse_pos)
{
    RUNTIME_ASSERT(comp->kind == COMPONENT_BUTTON, "Can only check for button clicks in this function");
    vec2 m_pos = VEC2(mouse_pos.x - screen_size.x / 2.0, screen_size.y / 2.0 - mouse_pos.y);
    if (m_pos.x > comp->tr.x) return false;
    if (m_pos.x < comp->bl.x) return false;
    if (m_pos.y > comp->tr.y) return false;
    if (m_pos.y < comp->bl.y) return false;
    return true;
}

void ui_show_component(ui_component_t *comp, bool show)
{
    comp->visible = show;

    switch (comp->kind) {
    case COMPONENT_BUTTON:
        for (size_t i = 0; i < comp->dim.x * comp->dim.y; i++)
            comp->body_sprites[i]->visible = comp->visible;
        for (size_t i = 0; i < strlen(comp->text); i++)
            comp->text_sprites[i]->visible = comp->visible;
        break;
    case COMPONENT_LABEL:
        for (size_t i = 0; i < strlen(comp->text); i++)
            comp->text_sprites[i]->visible = comp->visible;
        break;
    };
}

void ui_cleanup_component(ui_component_t *comp)
{
    if (comp->body_sprites != NULL) free(comp->body_sprites);
    if (comp->text_sprites != NULL) free(comp->text_sprites);
}
