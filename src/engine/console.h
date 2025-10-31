#ifndef CONSOLE_H
#define CONSOLE_H

#ifndef NK_NUKLEAR_H_
#include <nuklear/nuklear.h>
#endif // NK_NUKLEAR_H_

#include <libem/em_math.h>

#include "logger.h"
#include "ui_component.h"
#include "event_system.h"

#define CONSOLE_INPUT_LENGTH 128

typedef enum console_command {
    COMMAND_NONE,
    COMMAND_SET_RENDER_DISTANCE,
    COMMAND_SET_MOVE_SPEED,
    COMMAND_SET_SENSITIVITY,
} console_command_e;

typedef struct console {
    size_t _row_num;
    float _row_height;
    char _input_buf[CONSOLE_INPUT_LENGTH];
    bool _visible;
} console_t;

typedef struct console_desc {
    event_system_t *es;
} console_desc_t;

extern bool console_visible(ui_component_t *component);
extern console_t *console_init(const console_desc_t *desc);
extern void console_render(struct nk_context *ctx, vec2 window_dimensions, 
                           ui_component_t *component);

#endif
