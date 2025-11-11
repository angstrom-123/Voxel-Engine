#include "renderer_base.h"

void renderer_init_base(renderer_base_t *rb, const renderer_base_desc_t *desc)
{
    rb->dimensions = desc->dimensions;
    rb->pass_act = desc->pass_act;
    if (!desc->dummy)
    {
        rb->pip = sg_make_pipeline(desc->pip_desc);
        rb->cam = desc->cam;
    }
}
