#ifndef UPDATE_SYSTEM_TYPES_H
#define UPDATE_SYSTEM_TYPES_H

#include <libem/em_math.h>
#include "geometry_types.h"

typedef struct us_request {
    enum {
        USREQ_STAGE,
        USREQ_UNSTAGE
    } type;
    ivec2 pos;
    mesh_t *mesh;
} us_request_t;

#endif
