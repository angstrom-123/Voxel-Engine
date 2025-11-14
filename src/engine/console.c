#include "console.h"

#define EDIT_FLAGS NK_EDIT_ALWAYS_INSERT_MODE |\
                   NK_EDIT_ALLOW_TAB |\
                   NK_EDIT_GOTO_END_ON_ACTIVATE
#define HISTORY_FLAGS EDIT_FLAGS | NK_EDIT_READ_ONLY
#define CMDLINE_FLAGS EDIT_FLAGS | NK_EDIT_SELECTABLE

inline static void _newline(console_t *c)
{
    // TODO: Make the console use nk_str for everything in the history for the convenience funcs.
    //       This allows me to abstract the bullcrap with the null terminator being 190.
    for (size_t i = 1; i < CONSOLE_ROW_NUM - 1; i++)
    {
        memset(&c->_bufs[i - 1][0], 190, CONSOLE_INPUT_LENGTH);
        memcpy(&c->_bufs[i - 1][0], &c->_bufs[i][0], CONSOLE_INPUT_LENGTH);
    }

    memset(&c->_bufs[CONSOLE_ROW_NUM - 2][0], 190, CONSOLE_INPUT_LENGTH);
    memcpy(&c->_bufs[CONSOLE_ROW_NUM - 2][0], &c->_bufs[CONSOLE_ROW_NUM - 1][0], c->_cmdline.string.len);
    nk_textedit_delete(&c->_cmdline, 0, c->_cmdline.string.len);

    c->_hist_idx = CONSOLE_ROW_NUM - 2;
    c->_hist_end = em_max(c->_hist_end - 1, 0);
}

inline static void _previous(console_t *c)
{
    if (c->_hist_idx == c->_hist_end)
        return;

    nk_textedit_delete(&c->_cmdline, 0, c->_cmdline.string.len);
    nk_textedit_text(&c->_cmdline, c->_bufs[c->_hist_idx], CONSOLE_INPUT_LENGTH);

    c->_hist_idx--;
}

inline static void _next(console_t *c)
{
    if (c->_hist_idx == CONSOLE_ROW_NUM - 2)
    {
        nk_textedit_delete(&c->_cmdline, 0, c->_cmdline.string.len);
        return;
    }

    c->_hist_idx++;

    nk_textedit_delete(&c->_cmdline, 0, c->_cmdline.string.len);
    nk_textedit_text(&c->_cmdline, c->_bufs[c->_hist_idx], CONSOLE_INPUT_LENGTH);
}

static void _on_keydown(const event_t *ev, void *args)
{
    console_t *c = args;

    switch (ev->keycode) {
    case KEYCODE_F1:
        c->_visible = !c->_visible;
        break;
    case KEYCODE_ESCAPE:
        c->_visible = false;
        break;
    case KEYCODE_ENTER:
        _newline(c);
        break;
    case KEYCODE_UP:
        _previous(c);
        break;
    case KEYCODE_DOWN:
        _next(c);
    default:
        break;
    };
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

    c->_row_height = 24.0;
    c->_visible = false;
    c->_hist_idx = CONSOLE_ROW_NUM - 2;
    c->_hist_end = CONSOLE_ROW_NUM - 2;

    nk_textedit_init_fixed(&c->_cmdline, &c->_bufs[CONSOLE_ROW_NUM - 1], CONSOLE_INPUT_LENGTH);

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
    const vec2 dim = {win_dim.x / 2.0, c->_row_height * CONSOLE_ROW_NUM};

    nk_style_hide_cursor(ctx);
    ctx->style.window.spacing = nk_vec2(0.0, 0.0);
    ctx->style.edit.border = 0.0;
    if (nk_begin(ctx, "Console", GET_UI_RECT(pos, dim), NK_WINDOW_NO_SCROLLBAR | 
                                                        NK_WINDOW_BACKGROUND))
    {
        for (size_t i = 0; i < CONSOLE_ROW_NUM - 1; i++)
        {
            nk_layout_row_dynamic(ctx, c->_row_height, NK_TEXT_ALIGN_LEFT);
            nk_edit_string_zero_terminated(ctx, HISTORY_FLAGS, c->_bufs[i], 
                                           CONSOLE_INPUT_LENGTH, nk_filter_ascii);
        }
                                    
        nk_layout_row_dynamic(ctx, c->_row_height, 1);
        if (!c->_cmdline.active)
        {
            ENGINE_LOG_OK("Focusing console.\n", NULL);
            nk_edit_focus(ctx, CMDLINE_FLAGS);
        }
        nk_edit_buffer(ctx, CMDLINE_FLAGS, &c->_cmdline, nk_filter_ascii);
    } nk_end(ctx);
}
