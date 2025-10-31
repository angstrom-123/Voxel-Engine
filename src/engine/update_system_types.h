#ifndef UPDATE_SYSTEM_TYPES_H
#define UPDATE_SYSTEM_TYPES_H


#if !defined(SOKOL_GFX_INCLUDED) 
#include <sokol/sokol_gfx.h>
#endif

#include <libem/em_math.h>
#include "geometry_types.h"

typedef struct us_buffer_pair {
    sg_buffer vertex;
    sg_buffer index;
} us_buffer_pair_t;

typedef struct us_request {
    enum {
        USREQ_STAGE,
        USREQ_UNSTAGE,
        USREQ_RESTAGE
    } type;
    ivec2 pos;
    mesh_t *mesh;
} us_request_t;

#endif
