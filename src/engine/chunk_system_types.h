#ifndef CHUNK_SYSTEM_TYPES_H
#define CHUNK_SYSTEM_TYPES_H

#include <libem/em_math.h>

#include "update_system_types.h"

typedef struct chunk_render_info {
    bool needs_update_o;
    ivec2 pos;
    buffer_pair_t bufs_o;
    mesh_t *mesh_o;
} chunk_render_info_t;

typedef union chunk_set {
    struct {
        // NOTE: This order is important as it allows for correct indexing of data.
        chunk_data_t *nwd;
        chunk_data_t *wd;
        chunk_data_t *swd;
        chunk_data_t *nd;
        chunk_data_t *cd;
        chunk_data_t *sd;
        chunk_data_t *ned;
        chunk_data_t *ed;
        chunk_data_t *sed;
    };
    chunk_data_t *data[3][3];
} chunk_set_t;

typedef struct cs_request {
    enum {
        CSREQ_LOAD,
        CSREQ_MESH,
        CSREQ_UNLOAD,
        CSREQ_PLACE,
        CSREQ_BREAK,
        CSREQ_REMESH,
        CSREQ_INITIAL_LOAD_COMPLETE
    } type;

    ivec2 pos;
    ivec3 cell;
    cube_type_e block;
} cs_request_t;

#endif
