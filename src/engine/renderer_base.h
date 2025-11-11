#ifndef RENDERER_BASE_H
#define RENDERER_BASE_H 

#include <libem/em_math.h>

#include "include_sokol.h"
#include "camera.h"

typedef struct renderer_base {
    vec2 dimensions;
    sg_pass_action pass_act;
    sg_pipeline pip;
    sg_bindings bind;
    const camera_t *cam;
} renderer_base_t;

typedef struct renderer_base_desc {
    sg_pipeline_desc *pip_desc;
    sg_pass_action pass_act;
    vec2 dimensions;
    const camera_t *cam;
    bool dummy;
} renderer_base_desc_t;

extern void renderer_init_base(renderer_base_t *rb, const renderer_base_desc_t *desc);

#endif
