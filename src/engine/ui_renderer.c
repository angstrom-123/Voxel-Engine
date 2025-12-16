#include "ui_renderer.h"

void ui_renderer_init(ui_renderer_t *uir, const ui_renderer_desc_t *desc)
{
    rbase_init(&uir->base, &(renderer_base_desc_t) {
        .dimensions = desc->dimensions,
        .cam = NULL
    });

    uir->ctx = NULL;
}

void ui_renderer_cleanup(ui_renderer_t *uir)
{
    (void) uir;
}

void ui_renderer_render_all(ui_renderer_t *uir)
{
    ENGINE_ASSERT(uir->ctx, "Nuklear context must be set to render UI");
    sg_begin_pass(&(sg_pass) {
        .action = (sg_pass_action) {
            .colors[0] = {
                .load_action = SG_LOADACTION_LOAD,
                .clear_value = {0.0, 0.0, 0.0, 0.0}
            }
        },
        .swapchain = sglue_swapchain(),
        .label = "UI renderer pass"
    });

    for (size_t i = 0; i < uir->component_count; i++)
    {
        ui_component_t *uic = uir->components[i];
        if (uic->visible(uic))
            uic->render(uir->ctx, uir->base.dimensions, uic);
    }

    snk_render(uir->base.dimensions.x, uir->base.dimensions.y);
    sg_end_pass();
}
