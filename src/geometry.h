#ifndef GEOMETRY_H
#define GEOMETRY_H 

#if !defined(SOKOL_GFX_INCLUDED) 
#include <sokol/sokol_gfx.h>
#endif

#include <stdint.h>
#include <stdbool.h>
#include <malloc.h>
#include <stdlib.h> // fprintf, stderr, exit

#include <libem/em_math.h>

#include "geometry_types.h"

#define PACK(a, b) ((uint32_t)(((uint32_t)(a * 0xFFFF) << 16) | ((uint32_t)(b * 0xFFFF) & 0xFFFF)))

#define TX(n) (0.0625 * n)
#define SIZ TX(1)

#define MAX_BLOCK_TYPES 8

/* 
 * Defined safe maximums for vertex and index count of newly generated chunk meshes. 
 * Once a chunk has been altered, its size may be much larger.
 */
#define V_MAX 12288
#define I_MAX 18432

/* Maximum multplier to v_max and i_max for a chunk's mesh. */
#define MAX_MULT 3

#define START_CANARY_VAL 0xDEADBEEF
#define MID_CANARY_VAL 0xFEEDBEAD
#define END_CANARY_VAL 0xCAFEBABE

/* 
 * NOTE: using 2x2 cube size to fix some strange scaling issues, this is a 
 * hacky workaround but it works and doesn't have any major side effects.
 */
static const vertex_t face_vertices[6][4] = {
	[FACEIDX_BACK] = {
		{2, 0, 0, 16, PACK(0.0, 0.0)},
		{0, 0, 0, 16, PACK(SIZ, 0.0)},
		{0, 2, 0, 16, PACK(SIZ, SIZ)},
		{2, 2, 0, 16, PACK(0.0, SIZ)}
	},
	[FACEIDX_FRONT] = {
		{0, 0, 2, 32, PACK(0.0, 0.0)},
		{2, 0, 2, 32, PACK(SIZ, 0.0)},
		{2, 2, 2, 32, PACK(SIZ, SIZ)},
		{0, 2, 2, 32, PACK(0.0, SIZ)}
	},
	[FACEIDX_LEFT] = {
		{0, 0, 0,  1, PACK(0.0, 0.0)},
		{0, 0, 2,  1, PACK(SIZ, 0.0)},
		{0, 2, 2,  1, PACK(SIZ, SIZ)},
		{0, 2, 0,  1, PACK(0.0, SIZ)}
	},
	[FACEIDX_RIGHT] = {
		{2, 0, 2,  2, PACK(0.0, 0.0)},
		{2, 0, 0,  2, PACK(SIZ, 0.0)},
		{2, 2, 0,  2, PACK(SIZ, SIZ)},
		{2, 2, 2,  2, PACK(0.0, SIZ)}
	},
	[FACEIDX_BOTTOM] = {
		{0, 0, 0,  8, PACK(0.0, 0.0)},
		{2, 0, 0,  8, PACK(SIZ, 0.0)},
		{2, 0, 2,  8, PACK(SIZ, SIZ)},
		{0, 0, 2,  8, PACK(0.0, SIZ)}
	},
	[FACEIDX_TOP] = {
		{0, 2, 2,  4, PACK(0.0, 0.0)},
		{2, 2, 2,  4, PACK(SIZ, 0.0)},
		{2, 2, 0,  4, PACK(SIZ, SIZ)},
		{0, 2, 0,  4, PACK(0.0, SIZ)}
	}
};

static const vec2 uv_lookup[MAX_BLOCK_TYPES * 6] = {
	[CUBETYPE_AIR * 6 + FACEIDX_BACK]   = {0.0, 0.0},
	[CUBETYPE_AIR * 6 + FACEIDX_FRONT]  = {0.0, 0.0},
	[CUBETYPE_AIR * 6 + FACEIDX_RIGHT]  = {0.0, 0.0},
	[CUBETYPE_AIR * 6 + FACEIDX_LEFT]   = {0.0, 0.0},
	[CUBETYPE_AIR * 6 + FACEIDX_BOTTOM] = {0.0, 0.0},
	[CUBETYPE_AIR * 6 + FACEIDX_TOP]    = {0.0, 0.0},

	[CUBETYPE_GRASS * 6 + FACEIDX_BACK]   = {TX(1), TX(15)},
	[CUBETYPE_GRASS * 6 + FACEIDX_FRONT]  = {TX(1), TX(15)},
	[CUBETYPE_GRASS * 6 + FACEIDX_RIGHT]  = {TX(1), TX(15)},
	[CUBETYPE_GRASS * 6 + FACEIDX_LEFT]   = {TX(1), TX(15)},
	[CUBETYPE_GRASS * 6 + FACEIDX_BOTTOM] = {TX(2), TX(15)},
	[CUBETYPE_GRASS * 6 + FACEIDX_TOP]    = {TX(0), TX(15)},

	[CUBETYPE_DIRT * 6 + FACEIDX_BACK]   = {TX(2), TX(15)},
	[CUBETYPE_DIRT * 6 + FACEIDX_FRONT]  = {TX(2), TX(15)},
	[CUBETYPE_DIRT * 6 + FACEIDX_RIGHT]  = {TX(2), TX(15)},
	[CUBETYPE_DIRT * 6 + FACEIDX_LEFT]   = {TX(2), TX(15)},
	[CUBETYPE_DIRT * 6 + FACEIDX_BOTTOM] = {TX(2), TX(15)},
	[CUBETYPE_DIRT * 6 + FACEIDX_TOP]    = {TX(2), TX(15)},

	[CUBETYPE_STONE * 6 + FACEIDX_BACK]   = {TX(3), TX(15)},
	[CUBETYPE_STONE * 6 + FACEIDX_FRONT]  = {TX(3), TX(15)},
	[CUBETYPE_STONE * 6 + FACEIDX_RIGHT]  = {TX(3), TX(15)},
	[CUBETYPE_STONE * 6 + FACEIDX_LEFT]   = {TX(3), TX(15)},
	[CUBETYPE_STONE * 6 + FACEIDX_BOTTOM] = {TX(3), TX(15)},
	[CUBETYPE_STONE * 6 + FACEIDX_TOP]    = {TX(3), TX(15)},

	[CUBETYPE_SAND * 6 + FACEIDX_BACK]   = {TX(4), TX(15)},
	[CUBETYPE_SAND * 6 + FACEIDX_FRONT]  = {TX(4), TX(15)},
	[CUBETYPE_SAND * 6 + FACEIDX_RIGHT]  = {TX(4), TX(15)},
	[CUBETYPE_SAND * 6 + FACEIDX_LEFT]   = {TX(4), TX(15)},
	[CUBETYPE_SAND * 6 + FACEIDX_BOTTOM] = {TX(4), TX(15)},
	[CUBETYPE_SAND * 6 + FACEIDX_TOP] 	 = {TX(4), TX(15)},

	[CUBETYPE_LOG * 6 + FACEIDX_BACK]   = {TX(6), TX(15)},
	[CUBETYPE_LOG * 6 + FACEIDX_FRONT]  = {TX(6), TX(15)},
	[CUBETYPE_LOG * 6 + FACEIDX_RIGHT]  = {TX(6), TX(15)},
	[CUBETYPE_LOG * 6 + FACEIDX_LEFT]   = {TX(6), TX(15)},
	[CUBETYPE_LOG * 6 + FACEIDX_BOTTOM] = {TX(5), TX(15)},
	[CUBETYPE_LOG * 6 + FACEIDX_TOP]    = {TX(5), TX(15)},

	[CUBETYPE_LEAF * 6 + FACEIDX_BACK]   = {TX(7), TX(15)},
	[CUBETYPE_LEAF * 6 + FACEIDX_FRONT]  = {TX(7), TX(15)},
	[CUBETYPE_LEAF * 6 + FACEIDX_RIGHT]  = {TX(7), TX(15)},
	[CUBETYPE_LEAF * 6 + FACEIDX_LEFT]   = {TX(7), TX(15)},
	[CUBETYPE_LEAF * 6 + FACEIDX_BOTTOM] = {TX(7), TX(15)},
	[CUBETYPE_LEAF * 6 + FACEIDX_TOP]    = {TX(7), TX(15)},
};

extern bool geom_canaries_failed(chunk_t *chunk);
extern bool geom_cube_is_transparent(cube_type_e type);
extern void geom_generate_chunk_mesh(chunk_t *chunk);

#endif
