#include "event_system.h"

void event_sys_init(event_system_t *es) 
{
    (void) es;
}

void event_sys_cleanup(event_system_t *es)
{
    (void) es;
}

void event_sys_get_event(event_system_t *es, const event_t *ev)
{
    switch (ev->type) {
    case EVENT_NONE:
        break;
    case EVENT_KEYDOWN:
        es->keys_down[ev->keycode] = true;
        if (ev->keycode == KEYCODE_ESCAPE)
            sapp_lock_mouse(false);
        break;
    case EVENT_KEYUP:
        es->keys_down[ev->keycode] = false;
        break;
    case EVENT_KEYTYPED:
        // ENGINE_LOG_WARN("Keytyped: %i\n", ev->code_utf32);
        break;
    case EVENT_MOUSEDOWN:
        es->modifiers_down = ev->modifiers;
        sapp_lock_mouse(true);
        break;
    case EVENT_MOUSEUP:
        es->modifiers_down = ev->modifiers;
        break;
    case EVENT_MOUSEMOVE:
        es->mouse_pos = ev->mouse_pos;
        es->frame.mouse_delta = em_add_vec2(es->frame.mouse_delta, ev->mouse_delta);
        break;
    case EVENT_MOUSESCROLL:
        // ENGINE_LOG_WARN("Mouse scrolled: %i\n", ev->mouse_scroll.y);
        break;
    case EVENT_FOCUSED:
        // ENGINE_LOG_WARN("Focused window\n", NULL);
        break;
    case EVENT_UNFOCUSED:
        // ENGINE_LOG_WARN("Unfocused window\n", NULL);
        break;
    case EVENT_QUITREQUEST:
        // ENGINE_LOG_WARN("Quit requested\n", NULL);
        sapp_quit();
        break;
    default:
        break;
    };
}

event_t event_sys_convert_event(const sapp_event *sev)
{
    /* Ugly conversion between event types, ignoring events I don't need. */
    event_type_e type;
    switch (sev->type) {
    case SAPP_EVENTTYPE_KEY_DOWN:
        type = EVENT_KEYDOWN;
        break;
    case SAPP_EVENTTYPE_KEY_UP:
        type = EVENT_KEYUP;
        break;
    case SAPP_EVENTTYPE_CHAR:
        type = EVENT_KEYTYPED;
        break;
    case SAPP_EVENTTYPE_MOUSE_DOWN:
        type = EVENT_MOUSEDOWN;
        break;
    case SAPP_EVENTTYPE_MOUSE_UP:
        type = EVENT_MOUSEUP;
        break;
    case SAPP_EVENTTYPE_MOUSE_MOVE:
        type = EVENT_MOUSEMOVE;
        break;
    case SAPP_EVENTTYPE_MOUSE_SCROLL:
        type = EVENT_MOUSESCROLL;
        break;
    case SAPP_EVENTTYPE_FOCUSED:
        type = EVENT_FOCUSED;
        break;
    case SAPP_EVENTTYPE_UNFOCUSED:
        type = EVENT_UNFOCUSED;
        break;
    case SAPP_EVENTTYPE_QUIT_REQUESTED:
        type = EVENT_QUITREQUEST;
        break;
    default:
        type = EVENT_NONE;
        break;
    };

    return (event_t) {
        .type          = type,
        .keycode       = (keycode_e) sev->key_code, // Indexed the same way.
        .modifiers     = sev->modifiers, // Flags are the same.
        .code_utf32    = sev->char_code,
        .mouse_pos     = (vec2) {sev->mouse_x, sev->mouse_y},
        .mouse_delta   = (vec2) {sev->mouse_dx, sev->mouse_dy},
        .mouse_scroll  = (ivec2) {sev->scroll_x, sev->scroll_y},
        .window_size   = (ivec2) {sev->window_width, sev->window_height},
        .framebuf_size = (ivec2) {sev->framebuffer_width, sev->framebuffer_height},
        .handled       = false
    };
}

void event_sys_new_frame(event_system_t *es)
{
    es->frame.mouse_delta = (vec2) {0.0, 0.0};
}
