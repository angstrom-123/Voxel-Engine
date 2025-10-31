#ifndef CHUNK_SYSTEM_TYPES_H
#define CHUNK_SYSTEM_TYPES_H

#include <libem/em_math.h>

#include "geometry.h"
#include "update_system_types.h"

typedef struct chunk_render_info {
    ivec2 pos;
    mesh_t *mesh;
    us_buffer_pair_t bufs;
    bool needs_update;
} chunk_render_info_t;

typedef struct cs_request {
    enum {
        CSREQ_GEN,
        CSREQ_MESH,
        CSREQ_UNLOAD,
        CSREQ_REMESH
    } type;

    ivec2 pos;
} cs_request_t;

#endif
