#include "renderer_base.h"

void rbase_init(renderer_base_t *rb, const renderer_base_desc_t *desc)
{
    rb->dimensions = desc->dimensions;
    rb->cam = desc->cam;
}
