#ifndef UI_SYSTEM_H
#define UI_SYSTEM_H 

#include "logger.h"
#include "ui_component.h"

#if !defined(NK_NUKLEAR_H_) 
#include <nuklear/nuklear.h>
#endif

#include <libem/em_math.h>
#include <stdbool.h>

typedef enum ui_component_type {
    COMPONENT_CONSOLE,
    COMPONENT_NUM
} ui_component_type_e;

typedef struct ui_system {
    ui_component_t *components[COMPONENT_NUM];
    size_t component_count;
} ui_system_t;

typedef struct ui_system_desc {
    char __placeholder;
} ui_system_desc_t;

extern void ui_sys_init(ui_system_t *us, const ui_system_desc_t *desc);
extern void ui_sys_cleanup(ui_system_t *us);
extern void ui_sys_add(ui_system_t *us, ui_component_type_e type, const ui_component_desc_t *desc);
extern void ui_sys_get_components(ui_system_t *us, ui_component_t **components);

#endif
