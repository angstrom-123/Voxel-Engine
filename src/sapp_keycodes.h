#ifndef SAPP_KEYCODES_H
#define SAPP_KEYCODES_H

/*
 * This header file is a copy of the sapp_keycode enum from sokol_app.h because 
 * I couldn't think of a better way to access those constants without re-including 
 * sokol_app.h for a second time (as this breaks a bunch of stuff)
 *
 * This is for use in camera.c / camera.h for receiving inputs from the sokol 
 * event callback in main.c
 */

typedef enum _sapp_keycode {
    _SAPP_KEYCODE_INVALID          = 0,
    _SAPP_KEYCODE_SPACE            = 32,
    _SAPP_KEYCODE_APOSTROPHE       = 39,  /* ' */
    _SAPP_KEYCODE_COMMA            = 44,  /* , */
    _SAPP_KEYCODE_MINUS            = 45,  /* - */
    _SAPP_KEYCODE_PERIOD           = 46,  /* . */
    _SAPP_KEYCODE_SLASH            = 47,  /* / */
    _SAPP_KEYCODE_0                = 48,
    _SAPP_KEYCODE_1                = 49,
    _SAPP_KEYCODE_2                = 50,
    _SAPP_KEYCODE_3                = 51,
    _SAPP_KEYCODE_4                = 52,
    _SAPP_KEYCODE_5                = 53,
    _SAPP_KEYCODE_6                = 54,
    _SAPP_KEYCODE_7                = 55,
    _SAPP_KEYCODE_8                = 56,
    _SAPP_KEYCODE_9                = 57,
    _SAPP_KEYCODE_SEMICOLON        = 59,  /* ; */
    _SAPP_KEYCODE_EQUAL            = 61,  /* = */
    _SAPP_KEYCODE_A                = 65,
    _SAPP_KEYCODE_B                = 66,
    _SAPP_KEYCODE_C                = 67,
    _SAPP_KEYCODE_D                = 68,
    _SAPP_KEYCODE_E                = 69,
    _SAPP_KEYCODE_F                = 70,
    _SAPP_KEYCODE_G                = 71,
    _SAPP_KEYCODE_H                = 72,
    _SAPP_KEYCODE_I                = 73,
    _SAPP_KEYCODE_J                = 74,
    _SAPP_KEYCODE_K                = 75,
    _SAPP_KEYCODE_L                = 76,
    _SAPP_KEYCODE_M                = 77,
    _SAPP_KEYCODE_N                = 78,
    _SAPP_KEYCODE_O                = 79,
    _SAPP_KEYCODE_P                = 80,
    _SAPP_KEYCODE_Q                = 81,
    _SAPP_KEYCODE_R                = 82,
    _SAPP_KEYCODE_S                = 83,
    _SAPP_KEYCODE_T                = 84,
    _SAPP_KEYCODE_U                = 85,
    _SAPP_KEYCODE_V                = 86,
    _SAPP_KEYCODE_W                = 87,
    _SAPP_KEYCODE_X                = 88,
    _SAPP_KEYCODE_Y                = 89,
    _SAPP_KEYCODE_Z                = 90,
    _SAPP_KEYCODE_LEFT_BRACKET     = 91,  /* [ */
    _SAPP_KEYCODE_BACKSLASH        = 92,  /* \ */
    _SAPP_KEYCODE_RIGHT_BRACKET    = 93,  /* ] */
    _SAPP_KEYCODE_GRAVE_ACCENT     = 96,  /* ` */
    _SAPP_KEYCODE_WORLD_1          = 161, /* non-US #1 */
    _SAPP_KEYCODE_WORLD_2          = 162, /* non-US #2 */
    _SAPP_KEYCODE_ESCAPE           = 256,
    _SAPP_KEYCODE_ENTER            = 257,
    _SAPP_KEYCODE_TAB              = 258,
    _SAPP_KEYCODE_BACKSPACE        = 259,
    _SAPP_KEYCODE_INSERT           = 260,
    _SAPP_KEYCODE_DELETE           = 261,
    _SAPP_KEYCODE_RIGHT            = 262,
    _SAPP_KEYCODE_LEFT             = 263,
    _SAPP_KEYCODE_DOWN             = 264,
    _SAPP_KEYCODE_UP               = 265,
    _SAPP_KEYCODE_PAGE_UP          = 266,
    _SAPP_KEYCODE_PAGE_DOWN        = 267,
    _SAPP_KEYCODE_HOME             = 268,
    _SAPP_KEYCODE_END              = 269,
    _SAPP_KEYCODE_CAPS_LOCK        = 280,
    _SAPP_KEYCODE_SCROLL_LOCK      = 281,
    _SAPP_KEYCODE_NUM_LOCK         = 282,
    _SAPP_KEYCODE_PRINT_SCREEN     = 283,
    _SAPP_KEYCODE_PAUSE            = 284,
    _SAPP_KEYCODE_F1               = 290,
    _SAPP_KEYCODE_F2               = 291,
    _SAPP_KEYCODE_F3               = 292,
    _SAPP_KEYCODE_F4               = 293,
    _SAPP_KEYCODE_F5               = 294,
    _SAPP_KEYCODE_F6               = 295,
    _SAPP_KEYCODE_F7               = 296,
    _SAPP_KEYCODE_F8               = 297,
    _SAPP_KEYCODE_F9               = 298,
    _SAPP_KEYCODE_F10              = 299,
    _SAPP_KEYCODE_F11              = 300,
    _SAPP_KEYCODE_F12              = 301,
    _SAPP_KEYCODE_F13              = 302,
    _SAPP_KEYCODE_F14              = 303,
    _SAPP_KEYCODE_F15              = 304,
    _SAPP_KEYCODE_F16              = 305,
    _SAPP_KEYCODE_F17              = 306,
    _SAPP_KEYCODE_F18              = 307,
    _SAPP_KEYCODE_F19              = 308,
    _SAPP_KEYCODE_F20              = 309,
    _SAPP_KEYCODE_F21              = 310,
    _SAPP_KEYCODE_F22              = 311,
    _SAPP_KEYCODE_F23              = 312,
    _SAPP_KEYCODE_F24              = 313,
    _SAPP_KEYCODE_F25              = 314,
    _SAPP_KEYCODE_KP_0             = 320,
    _SAPP_KEYCODE_KP_1             = 321,
    _SAPP_KEYCODE_KP_2             = 322,
    _SAPP_KEYCODE_KP_3             = 323,
    _SAPP_KEYCODE_KP_4             = 324,
    _SAPP_KEYCODE_KP_5             = 325,
    _SAPP_KEYCODE_KP_6             = 326,
    _SAPP_KEYCODE_KP_7             = 327,
    _SAPP_KEYCODE_KP_8             = 328,
    _SAPP_KEYCODE_KP_9             = 329,
    _SAPP_KEYCODE_KP_DECIMAL       = 330,
    _SAPP_KEYCODE_KP_DIVIDE        = 331,
    _SAPP_KEYCODE_KP_MULTIPLY      = 332,
    _SAPP_KEYCODE_KP_SUBTRACT      = 333,
    _SAPP_KEYCODE_KP_ADD           = 334,
    _SAPP_KEYCODE_KP_ENTER         = 335,
    _SAPP_KEYCODE_KP_EQUAL         = 336,
    _SAPP_KEYCODE_LEFT_SHIFT       = 340,
    _SAPP_KEYCODE_LEFT_CONTROL     = 341,
    _SAPP_KEYCODE_LEFT_ALT         = 342,
    _SAPP_KEYCODE_LEFT_SUPER       = 343,
    _SAPP_KEYCODE_RIGHT_SHIFT      = 344,
    _SAPP_KEYCODE_RIGHT_CONTROL    = 345,
    _SAPP_KEYCODE_RIGHT_ALT        = 346,
    _SAPP_KEYCODE_RIGHT_SUPER      = 347,
    _SAPP_KEYCODE_MENU             = 348,
} _sapp_keycode;

#endif
