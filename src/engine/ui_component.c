#include "ui_component.h"

void ui_make_button(ui_component_t *comp, sprite_renderer_t *sr, const ui_button_desc_t *desc)
{
    comp->kind         = COMPONENT_BUTTON;
    comp->transform    = desc->transform;
    comp->style        = desc->style;
    comp->cb     = desc->cb;
    comp->cb_args = desc->cb_args;
    comp->body_sprites = sprite_renderer_push_ui(sr, &(ui_sprites_desc_t) {
        .visible = comp->style.visible,
        .rounded = comp->style.body.rounded,
        .bg_col  = comp->style.body.bg_col,
        .mini    = comp->style.body.mini,
        .z_index = comp->style.body.z_index,
        .size    = comp->style.body.size,
        .pos     = comp->transform.pos,
        .dim     = comp->transform.dim
    });

    float body_width = comp->style.body.size.x * comp->transform.dim.x;
    if (desc->text[0] == '\0')
    {
        comp->text[0] = '\0';
        comp->text_sprites = NULL;
    }
    else
    {
        float text_width = comp->style.text.size.x * strlen(desc->text);
        float pad_left = (body_width - text_width) / 2.0;
        strcpy(comp->text, desc->text);
        comp->text_sprites = sprite_renderer_push_str(sr, desc->text, &(sprite_desc_t) {
            .visible = comp->style.visible,
            .bg_col  = comp->style.text.bg_col,
            .z_index = comp->style.text.z_index,
            .size    = comp->style.text.size,
            .pos     = VEC2(comp->transform.pos.x + pad_left, comp->transform.pos.y),
        });
    }
    comp->_tr = VEC2(comp->transform.pos.x + body_width, 
                     comp->transform.pos.y + comp->style.body.size.y);
    comp->_bl = VEC2(comp->transform.pos.x, 
                     comp->transform.pos.y - (comp->transform.dim.y - 1) * comp->style.body.size.y);
}

void ui_make_label(ui_component_t *comp, sprite_renderer_t *sr, const ui_label_desc_t *desc)
{
    ENGINE_ASSERT(false, "Unimplemented");

    comp->kind      = COMPONENT_LABEL;
    comp->transform = desc->transform;
    comp->style     = desc->style;
    (void) sr;
}

bool ui_button_clicked(ui_component_t *comp, ivec2 screen_size, vec2 mouse_pos)
{
    RUNTIME_ASSERT(comp->kind == COMPONENT_BUTTON, "Can only check for button clicks in this function");
    vec2 m_pos = VEC2(mouse_pos.x - screen_size.x / 2.0, screen_size.y / 2.0 - mouse_pos.y);
    if (m_pos.x > comp->_tr.x) return false;
    if (m_pos.x < comp->_bl.x) return false;
    if (m_pos.y > comp->_tr.y) return false;
    if (m_pos.y < comp->_bl.y) return false;
    return true;
}

void ui_show_component(ui_component_t *comp, bool show)
{
    comp->style.visible = show;

    for (size_t i = 0; i < comp->transform.dim.x * comp->transform.dim.y; i++)
        comp->body_sprites[i]->visible = show;

    if (comp->text[0] != '\0')
    {
        for (size_t i = 0; i < strlen(comp->text); i++)
            comp->text_sprites[i]->visible = show;
    }
}

void ui_cleanup_component(ui_component_t *comp)
{
    free(comp->body_sprites);
    if (comp->text[0] != '\0') free(comp->text_sprites);
}
