#include "ui_system.h"

void ui_sys_init(ui_system_t *uis, const ui_system_desc_t *desc)
{
    (void) desc;
    memset(&uis->components[0], 0, COMPONENT_NUM * sizeof(ui_component_t *));
}

void ui_sys_cleanup(ui_system_t *uis)
{
    for (size_t i = 0; i < uis->component_count; i++)
    {
        if (uis->components[i])
        {
            free(uis->components[i]->ptr);
            free(uis->components[i]);
        }
    }
}

void ui_sys_add(ui_system_t *uis, ui_component_type_e type, const ui_component_desc_t *desc)
{
    ENGINE_ASSERT(uis->components[type] == NULL, "Component already added to system.\n");

    ui_component_t *uic = malloc(sizeof(ui_component_t));
    uic->ptr     = desc->ptr;
    uic->render  = desc->render_callback;
    uic->visible = desc->visible;

    uis->components[type] = uic;
    uis->component_count++;
}

ui_component_t **ui_sys_get_components(ui_system_t *uis, size_t *cnt)
{
    *cnt = uis->component_count;
    return uis->components;
}
