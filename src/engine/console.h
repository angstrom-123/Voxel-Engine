#ifndef CONSOLE_H
#define CONSOLE_H

#include "logger.h"
#include "ui_component.h"
#include "event_system.h"

#include "include_nuklear.h"
#include <libem/em_math.h>

#define CONSOLE_INPUT_LENGTH 128
#define CONSOLE_ROW_NUM 10

typedef enum console_command {
    COMMAND_NONE,
    COMMAND_SET_RENDER_DISTANCE,
    COMMAND_SET_MOVE_SPEED,
    COMMAND_SET_SENSITIVITY,
} console_command_e;

typedef struct console {
    float _row_height;
    char _bufs[CONSOLE_ROW_NUM][CONSOLE_INPUT_LENGTH];
    struct nk_text_edit _cmdline;
    bool _visible;
    size_t _hist_idx;
    size_t _hist_end;
} console_t;

typedef struct console_desc {
    event_system_t *es;
} console_desc_t;

extern bool console_visible(ui_component_t *component);
extern console_t *console_init(const console_desc_t *desc);
extern void console_render(struct nk_context *ctx, vec2 window_dimensions, ui_component_t *component);

#endif
