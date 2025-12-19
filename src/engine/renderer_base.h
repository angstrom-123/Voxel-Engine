#ifndef RENDERER_BASE_H
#define RENDERER_BASE_H 

#include <libem/em_math.h>

#include "include_sokol.h"
#include "camera.h"

typedef struct renderer_base {
    vec2 dimensions;
    sg_pass pass;
    sg_pipeline pip;
    sg_bindings bind;
    camera_t *cam;
} renderer_base_t;

typedef struct renderer_base_desc {
    vec2 dimensions;
    camera_t *cam;
} renderer_base_desc_t;

extern void rbase_init(renderer_base_t *rb, const renderer_base_desc_t *desc);

#endif
