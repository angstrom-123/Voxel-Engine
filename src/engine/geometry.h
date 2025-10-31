#ifndef GEOMETRY_H
#define GEOMETRY_H 

#if !defined(SOKOL_GFX_INCLUDED) 
#include <sokol/sokol_gfx.h>
#endif

#include <stdint.h>
#include <stdbool.h>
#include <malloc.h>
#include <stdlib.h> // fprintf, stderr, exit
#include <math.h>

#include <libem/em_math.h>

#include "geometry_types.h"
#include "logger.h"

#define REL_N(pos) (em_add_ivec2((pos), (ivec2) {0, CHUNK_SIZE}))
#define REL_NE(pos) (em_add_ivec2((pos), (ivec2) {CHUNK_SIZE, CHUNK_SIZE}))
#define REL_E(pos) (em_add_ivec2((pos), (ivec2) {CHUNK_SIZE, 0}))
#define REL_SE(pos) (em_add_ivec2((pos), (ivec2) {CHUNK_SIZE, -CHUNK_SIZE}))
#define REL_S(pos) (em_add_ivec2((pos), (ivec2) {0, -CHUNK_SIZE}))
#define REL_SW(pos) (em_add_ivec2((pos), (ivec2) {-CHUNK_SIZE, -CHUNK_SIZE}))
#define REL_W(pos) (em_add_ivec2((pos), (ivec2) {-CHUNK_SIZE, 0}))
#define REL_NW(pos) (em_add_ivec2((pos), (ivec2) {-CHUNK_SIZE, CHUNK_SIZE}))

/* A chunk's mesh will never exceed these maximums. */
// #define V_MAX 33024
// #define I_MAX 49536
#define V_MAX 66048
#define I_MAX 99072

static const uvec3 face_offsets[6][4] = {
	[FACEIDX_BACK  ] = {{1, 0, 0}, {0, 0, 0}, {0, 1, 0}, {1, 1, 0}},
	[FACEIDX_FRONT ] = {{0, 0, 1}, {1, 0, 1}, {1, 1, 1}, {0, 1, 1}},
	[FACEIDX_LEFT  ] = {{0, 0, 0}, {0, 0, 1}, {0, 1, 1}, {0, 1, 0}},
	[FACEIDX_RIGHT ] = {{1, 0, 1}, {1, 0, 0}, {1, 1, 0}, {1, 1, 1}},
	[FACEIDX_BOTTOM] = {{0, 0, 0}, {1, 0, 0}, {1, 0, 1}, {0, 0, 1}},
	[FACEIDX_TOP   ] = {{0, 1, 1}, {1, 1, 1}, {1, 1, 0}, {0, 1, 0}}
};

static const tex_type_e tex_lookup[CUBETYPE_NUM][6] = {
    [CUBETYPE_AIR] = {
        [FACEIDX_BACK  ] = 0,
        [FACEIDX_FRONT ] = 0,
        [FACEIDX_LEFT  ] = 0,
        [FACEIDX_RIGHT ] = 0,
        [FACEIDX_BOTTOM] = 0,
        [FACEIDX_TOP   ] = 0
    },
    [CUBETYPE_GRASS] = {
        [FACEIDX_BACK  ] = 2,
        [FACEIDX_FRONT ] = 2,
        [FACEIDX_LEFT  ] = 2,
        [FACEIDX_RIGHT ] = 2,
        [FACEIDX_BOTTOM] = 3,
        [FACEIDX_TOP   ] = 1
    },
    [CUBETYPE_DIRT] = {
        [FACEIDX_BACK  ] = 3,
        [FACEIDX_FRONT ] = 3,
        [FACEIDX_LEFT  ] = 3,
        [FACEIDX_RIGHT ] = 3,
        [FACEIDX_BOTTOM] = 3,
        [FACEIDX_TOP   ] = 3
    },
    [CUBETYPE_STONE] = {
        [FACEIDX_BACK  ] = 4,
        [FACEIDX_FRONT ] = 4,
        [FACEIDX_LEFT  ] = 4,
        [FACEIDX_RIGHT ] = 4,
        [FACEIDX_BOTTOM] = 4,
        [FACEIDX_TOP   ] = 4
    },
    [CUBETYPE_SAND] = {
        [FACEIDX_BACK  ] = 5,
        [FACEIDX_FRONT ] = 5,
        [FACEIDX_LEFT  ] = 5,
        [FACEIDX_RIGHT ] = 5,
        [FACEIDX_BOTTOM] = 5,
        [FACEIDX_TOP   ] = 5
    },
    [CUBETYPE_LOG] = {
        [FACEIDX_BACK  ] = 7,
        [FACEIDX_FRONT ] = 7,
        [FACEIDX_LEFT  ] = 7,
        [FACEIDX_RIGHT ] = 7,
        [FACEIDX_BOTTOM] = 6,
        [FACEIDX_TOP   ] = 6
    },
    [CUBETYPE_LEAF] = {
        [FACEIDX_BACK  ] = 8,
        [FACEIDX_FRONT ] = 8,
        [FACEIDX_LEFT  ] = 8,
        [FACEIDX_RIGHT ] = 8,
        [FACEIDX_BOTTOM] = 8,
        [FACEIDX_TOP   ] = 8
    },
};

extern bool geom_cube_is_transparent(cube_type_e type);
extern mesh_t *geom_generate_mesh(chunk_data_t *cb, 
                                         chunk_data_t *nb, chunk_data_t *eb, 
                                         chunk_data_t *sb, chunk_data_t *wb);
#endif
