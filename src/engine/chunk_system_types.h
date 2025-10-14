#ifndef CHUNK_SYSTEM_TYPES_H
#define CHUNK_SYSTEM_TYPES_H

#include <libem/em_math.h>

#include "geometry_types.h"

typedef struct chunk_render_info {
    ivec2 pos;
    offset_t offset;
    size_t index_cnt;
} chunk_render_info_t;

typedef struct cs_request {
    enum {
        CSREQ_GEN,
        CSREQ_MESH,
        CSREQ_UNLOAD
    } type;

    ivec2 pos;
} cs_request_t;

typedef struct cs_result {
    ivec2 pos;
    mesh_t *mesh;
} cs_result_t;

#endif
