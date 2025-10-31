#include "event_system.h"

bool block_func_always(const event_t *ev, void *args) { 
    (void) ev; 
    (void) args; 
    return true; 
}

bool block_func_never(const event_t *ev, void *args) { 
    (void) ev; 
    (void) args; 
    return false; 
}

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
    /* Handle event subscribers. */
    for (size_t i = 0; i < MAX_EVENT_SUBSCRIBERS; i++)
    {
        event_subscriber_t s = es->subscribers[ev->type][i];
        if (!s.event_cb) // Null function pointer means uninitialized subscriber.
            continue;

        /* Make callback, only continue handling event if not blocking. */
        s.event_cb(ev, s.args);
        if (s.block_cb(ev, s.args))
            return;
    }

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
        es->frame.modifiers_pressed |= ev->modifiers;
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
        break;
    case EVENT_FOCUSED:
        break;
    case EVENT_UNFOCUSED:
        break;
    case EVENT_QUITREQUEST:
        ENGINE_LOG_WARN("Quit requested\n", NULL);
        sapp_quit();
        break;
    case EVENT_RESIZED:
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
    case SAPP_EVENTTYPE_RESIZED:
        type = EVENT_RESIZED;
        break;
    default:
        type = EVENT_NONE;
        break;
    };

    return (event_t) {
        .type          = type,
        .keycode       = (keycode_e) sev->key_code,
        .modifiers     = sev->modifiers,
        .code_utf32    = sev->char_code,
        .mouse_button  = (mouse_button_e) sev->mouse_button,
        .mouse_pos     = (vec2) {sev->mouse_x, sev->mouse_y},
        .mouse_delta   = (vec2) {sev->mouse_dx, sev->mouse_dy},
        .mouse_scroll  = (ivec2) {sev->scroll_x, sev->scroll_y},
        .window_size   = (vec2) {sev->window_width, sev->window_height},
        .framebuf_size = (ivec2) {sev->framebuffer_width, sev->framebuffer_height},
        .handled       = false
    };
}

void event_sys_subscribe_to_event(event_system_t *es, event_type_e type, 
                                  const event_subscriber_desc_t *desc)
{
    event_subscriber_t *s;

    bool found = false;
    size_t ctr = 0;
    while (ctr < MAX_EVENT_SUBSCRIBERS)
    {
        s = &es->subscribers[type][ctr++];
        if (!s->event_cb) // Function pointer is null if not initialized.
        {
            found = true;
            break;
        }
    }

    ENGINE_ASSERT(found, "All event subscriber slots filled");

    *s = (event_subscriber_t) {
        .event_cb = desc->event_cb,
        .block_cb = desc->block_cb,
        .args = desc->args
    };
}

void event_sys_new_frame(event_system_t *es)
{
    es->frame = (struct es_frame) {
        .mouse_delta = (vec2) {0, 0},
        .modifiers_pressed = 0
    };
}
