#ifndef EVENT_SYSTEM_H
#define EVENT_SYSTEM_H

#include "logger.h"

#include <libem/em_math.h>

#if !defined(SOKOL_APP_INCLUDED)
    #undef SOKOL_IMPL
    #include <sokol/sokol_app.h>
#endif

typedef enum keycode {
    KEYCODE_INVALID       = 0,
    KEYCODE_SPACE         = 32,
    KEYCODE_APOSTROPHE    = 39,  // '
    KEYCODE_COMMA         = 44,  // ,
    KEYCODE_MINUS         = 45,  // -
    KEYCODE_PERIOD        = 46,  // .
    KEYCODE_SLASH         = 47,  // /
    KEYCODE_0             = 48,
    KEYCODE_1             = 49,
    KEYCODE_2             = 50,
    KEYCODE_3             = 51,
    KEYCODE_4             = 52,
    KEYCODE_5             = 53,
    KEYCODE_6             = 54,
    KEYCODE_7             = 55,
    KEYCODE_8             = 56,
    KEYCODE_9             = 57,
    KEYCODE_SEMICOLON     = 59,  // ;
    KEYCODE_EQUAL         = 61,  // =
    KEYCODE_A             = 65,
    KEYCODE_B             = 66,
    KEYCODE_C             = 67,
    KEYCODE_D             = 68,
    KEYCODE_E             = 69,
    KEYCODE_F             = 70,
    KEYCODE_G             = 71,
    KEYCODE_H             = 72,
    KEYCODE_I             = 73,
    KEYCODE_J             = 74,
    KEYCODE_K             = 75,
    KEYCODE_L             = 76,
    KEYCODE_M             = 77,
    KEYCODE_N             = 78,
    KEYCODE_O             = 79,
    KEYCODE_P             = 80,
    KEYCODE_Q             = 81,
    KEYCODE_R             = 82,
    KEYCODE_S             = 83,
    KEYCODE_T             = 84,
    KEYCODE_U             = 85,
    KEYCODE_V             = 86,
    KEYCODE_W             = 87,
    KEYCODE_X             = 88,
    KEYCODE_Y             = 89,
    KEYCODE_Z             = 90,
    KEYCODE_LEFT_BRACKET  = 91,  // [
    KEYCODE_BACKSLASH     = 92,  /* \ */
    KEYCODE_RIGHT_BRACKET = 93,  // ]
    KEYCODE_GRAVE_ACCENT  = 96,  // `
    KEYCODE_WORLD_1       = 161, // non-US #1
    KEYCODE_WORLD_2       = 162, // non-US #2
    KEYCODE_ESCAPE        = 256,
    KEYCODE_ENTER         = 257,
    KEYCODE_TAB           = 258,
    KEYCODE_BACKSPACE     = 259,
    KEYCODE_INSERT        = 260,
    KEYCODE_DELETE        = 261,
    KEYCODE_RIGHT         = 262,
    KEYCODE_LEFT          = 263,
    KEYCODE_DOWN          = 264,
    KEYCODE_UP            = 265,
    KEYCODE_PAGE_UP       = 266,
    KEYCODE_PAGE_DOWN     = 267,
    KEYCODE_HOME          = 268,
    KEYCODE_END           = 269,
    KEYCODE_CAPS_LOCK     = 280,
    KEYCODE_SCROLL_LOCK   = 281,
    KEYCODE_NUM_LOCK      = 282,
    KEYCODE_PRINT_SCREEN  = 283,
    KEYCODE_PAUSE         = 284,
    KEYCODE_F1            = 290,
    KEYCODE_F2            = 291,
    KEYCODE_F3            = 292,
    KEYCODE_F4            = 293,
    KEYCODE_F5            = 294,
    KEYCODE_F6            = 295,
    KEYCODE_F7            = 296,
    KEYCODE_F8            = 297,
    KEYCODE_F9            = 298,
    KEYCODE_F10           = 299,
    KEYCODE_F11           = 300,
    KEYCODE_F12           = 301,
    KEYCODE_F13           = 302,
    KEYCODE_F14           = 303,
    KEYCODE_F15           = 304,
    KEYCODE_F16           = 305,
    KEYCODE_F17           = 306,
    KEYCODE_F18           = 307,
    KEYCODE_F19           = 308,
    KEYCODE_F20           = 309,
    KEYCODE_F21           = 310,
    KEYCODE_F22           = 311,
    KEYCODE_F23           = 312,
    KEYCODE_F24           = 313,
    KEYCODE_F25           = 314,
    KEYCODE_KP_0          = 320,
    KEYCODE_KP_1          = 321,
    KEYCODE_KP_2          = 322,
    KEYCODE_KP_3          = 323,
    KEYCODE_KP_4          = 324,
    KEYCODE_KP_5          = 325,
    KEYCODE_KP_6          = 326,
    KEYCODE_KP_7          = 327,
    KEYCODE_KP_8          = 328,
    KEYCODE_KP_9          = 329,
    KEYCODE_KP_DECIMAL    = 330,
    KEYCODE_KP_DIVIDE     = 331,
    KEYCODE_KP_MULTIPLY   = 332,
    KEYCODE_KP_SUBTRACT   = 333,
    KEYCODE_KP_ADD        = 334,
    KEYCODE_KP_ENTER      = 335,
    KEYCODE_KP_EQUAL      = 336,
    KEYCODE_LEFT_SHIFT    = 340,
    KEYCODE_LEFT_CONTROL  = 341,
    KEYCODE_LEFT_ALT      = 342,
    KEYCODE_LEFT_SUPER    = 343,
    KEYCODE_RIGHT_SHIFT   = 344,
    KEYCODE_RIGHT_CONTROL = 345,
    KEYCODE_RIGHT_ALT     = 346,
    KEYCODE_RIGHT_SUPER   = 347,
    KEYCODE_MENU          = 348,
    KEYCODE_MAX_INDEX     = 349
} keycode_e;

typedef enum event_type {
    EVENT_NONE,
    EVENT_KEYDOWN,
    EVENT_KEYUP,
    EVENT_KEYTYPED,
    EVENT_MOUSEDOWN,
    EVENT_MOUSEUP,
    EVENT_MOUSEMOVE,
    EVENT_MOUSESCROLL,
    EVENT_FOCUSED,
    EVENT_UNFOCUSED,
    EVENT_QUITREQUEST,
    EVENT_NUM
} event_type_e;

typedef enum modifier {
    MODIFIER_NONE  = 0,
    MODIFIER_SHIFT = 1,
    MODIFIER_CTRL  = 2,
    MODIFIER_ALT   = 4,
    MODIFIER_SUPER = 8,
    MODIFIER_LMB   = 16,
    MODIFIER_RMB   = 32,
    MODIFIER_MMB   = 64,
} modifier_e;

typedef struct event {
    event_type_e type;
    keycode_e keycode;
    uint8_t modifiers;
    int32_t code_utf32;
    vec2 mouse_pos;
    vec2 mouse_delta;
    ivec2 mouse_scroll;
    ivec2 window_size;
    ivec2 framebuf_size;
    bool handled;
} event_t;

typedef struct event_system {
    bool keys_down[KEYCODE_MAX_INDEX];
    uint8_t modifiers_down;
    vec2 mouse_pos;
    ivec2 window_size;
    ivec2 framebuf_size;
    struct {
        vec2 mouse_delta;
    } frame;
} event_system_t;

extern void event_sys_init(event_system_t *es);
extern void event_sys_cleanup(event_system_t *es);
extern void event_sys_get_event(event_system_t *es, const event_t *ev);
extern event_t event_sys_convert_event(const sapp_event *sev);
extern void event_sys_new_frame(event_system_t *es);

#endif
