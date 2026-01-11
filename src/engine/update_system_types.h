#ifndef UPDATE_SYSTEM_TYPES_H
#define UPDATE_SYSTEM_TYPES_H

#include <libem/em_math.h>

#include "geometry_types.h"
#include "include_sokol.h"

typedef struct buffer_pair {
    sg_buffer vertex;
    sg_buffer index;
} buffer_pair_t;

typedef struct us_request {
    ivec2 pos;
    mesh_t *mesh;
    enum {
        USREQ_STAGE,
        USREQ_UNSTAGE,
        USREQ_RESTAGE
    } type;
} us_request_t;

#endif
