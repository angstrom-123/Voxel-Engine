#ifndef CHUNK_SYSTEM_TYPES_H
#define CHUNK_SYSTEM_TYPES_H

#include <libem/em_math.h>

#include "geometry_types.h"

typedef struct chunk_render_info {
    ivec2 pos;
    offset_t offset;
    uint16_t index_cnt;
    uint32_t generation;
} chunk_render_info_t;

typedef struct cs_request {
    enum {
        CSREQ_GEN,
        CSREQ_MESH,
        CSREQ_UNLOAD
    } type;

    ivec2 pos;
} cs_request_t;

#endif
