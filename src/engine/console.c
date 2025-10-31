#include "console.h"

static void _on_keydown(const event_t *ev, void *args)
{
    console_t *c = args;

    if (ev->keycode == KEYCODE_F1)
    {
        c->_visible = !c->_visible;
        return;
    }

    if (!c->_visible)
        return;

    // TODO: see how to input into nuklear edit box programatically.
    //       might have to use a different type of ui element.
    //       might have to implement my own text input 
    //          how will i render it?
    //              pass it to nuklear?
    ENGINE_LOG_OK("TODO: Implement typing into console.\n", NULL);
}

static bool _block_keydown(const event_t *ev, void *args)
{
    (void) ev;
    console_t *c = args;
    return c->_visible;
}

bool console_visible(ui_component_t *component)
{
    console_t *c = component->ptr;
    return c->_visible;
}

console_t *console_init(const console_desc_t *desc)
{
    console_t *c = malloc(sizeof(console_t));

    c->_row_height = 20.0;
    c->_row_num = 10;
    c->_input_buf[CONSOLE_INPUT_LENGTH - 1] = '\0';
    c->_visible = false;

    event_sys_subscribe_to_event(desc->es, EVENT_KEYDOWN, &(event_subscriber_desc_t) {
        .event_cb = _on_keydown,
        .block_cb = _block_keydown,
        .args = c
    });

    return c;
}

void console_render(struct nk_context *ctx, vec2 win_dim, ui_component_t *component)
{
    console_t *c = component->ptr;

    const vec2 pos = {0, 0};
    const vec2 dim = {win_dim.x / 2.0, c->_row_height * c->_row_num + 50.0};

    nk_style_hide_cursor(ctx);

    if (nk_begin(ctx, "Console", GET_UI_RECT(pos, dim), 
                 NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BACKGROUND))
    {
        for (size_t i = 0; i < c->_row_num - 1; i++)
        {
            nk_layout_row_dynamic(ctx, c->_row_height, 1);
            nk_label(ctx, "Console line.", NK_TEXT_LEFT);
        }

        nk_layout_row_dynamic(ctx, c->_row_height, 1);
        nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, c->_input_buf, 
                                       CONSOLE_INPUT_LENGTH, nk_filter_ascii);
    }
    nk_end(ctx);
}
