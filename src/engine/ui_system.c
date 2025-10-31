#include "ui_system.h"

void ui_sys_init(ui_system_t *us, const ui_system_desc_t *desc)
{
    (void) desc;
    memset(&us->components[0], 0, COMPONENT_NUM * sizeof(ui_component_t *));
}

void ui_sys_cleanup(ui_system_t *us)
{
    for (size_t i = 0; i < us->component_count; i++)
    {
        if (us->components[i])
        {
            free(us->components[i]->ptr);
            free(us->components[i]);
        }
    }
}

void ui_sys_add(ui_system_t *us, ui_component_type_e type, const ui_component_desc_t *desc)
{
    ENGINE_ASSERT(us->components[type] == NULL, "Component already added to system.\n");

    ui_component_t *uic = malloc(sizeof(ui_component_t));
    uic->ptr     = desc->ptr;
    uic->render  = desc->render_callback;
    uic->visible = desc->visible;

    us->components[type] = uic;
    us->component_count++;
}

void ui_sys_get_components(ui_system_t *us, ui_component_t **comps)
{
    size_t idx = 0;
    for (size_t i = 0; i < us->component_count; i++)
    {
        ui_component_t *uic = us->components[i];
        if (uic)
            comps[idx++] = uic;
    }
}
