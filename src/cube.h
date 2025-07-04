#ifndef CUBE_H
#define CUBE_H 

#include <stdint.h>
#include <stdlib.h>
#include <memory.h>
#include <stdbool.h>

#include "extra_math.h"

#define TX(n) (0.0625 * n)
#define SIZ TX(1)
#define MAX_BLOCK_TYPES 8

typedef struct vertex {
	float x;
	float y;
	float z;

	float nx;
	float ny;
	float nz;

	float u;
	float v;

	float f;
} vertex_t;

typedef struct cube {
	size_t v_len;
	size_t i_len;
	vertex_t vertices[24];
	uint16_t indices[36];
} cube_t;

typedef struct cube_instance {
	em_vec3 pos;
	uint8_t type;
} cube_instance_t;

typedef enum cube_face_idx {
	FACEIDX_BACK,
	FACEIDX_FRONT,
	FACEIDX_RIGHT,
	FACEIDX_LEFT,
	FACEIDX_BOTTOM,
	FACEIDX_TOP
} cube_face_idx_e;

typedef enum cube_type {
	CUBETYPE_INVALID = 0,
	CUBETYPE_GRASS   = 1,
	CUBETYPE_DIRT    = 2,
	CUBETYPE_STONE   = 3,
	CUBETYPE_SAND    = 4,
	CUBETYPE_LOG     = 5,
	CUBETYPE_LEAF    = 6,
} cube_type_e;

#pragma pack(1) /* Ensure no padding bytes so memcpy works with shader uniforms */
typedef struct cube_uv_lookup {
	em_vec4 uv_rects[MAX_BLOCK_TYPES * 6];
} cube_uv_lookup_t;
#pragma pack() /* Reset to default packing */

static const vertex_t cube_vertices[24] = {
	 /*  coord  */ 	 /*   normal  */   /*  uv */  /*  face  */
	{0.0, 0.0, 0.0,  0.0,  0.0,  1.0,  SIZ, 0.0,  FACEIDX_BACK},
	{1.0, 0.0, 0.0,  0.0,  0.0,  1.0,  0.0, 0.0,  FACEIDX_BACK},
	{1.0, 1.0, 0.0,  0.0,  0.0,  1.0,  0.0, SIZ,  FACEIDX_BACK},
	{0.0, 1.0, 0.0,  0.0,  0.0,  1.0,  SIZ, SIZ,  FACEIDX_BACK},

	{0.0, 0.0, 1.0,  0.0,  0.0, -1.0,  0.0, 0.0,  FACEIDX_FRONT},
	{1.0, 0.0, 1.0,  0.0,  0.0, -1.0,  SIZ, 0.0,  FACEIDX_FRONT},
	{1.0, 1.0, 1.0,  0.0,  0.0, -1.0,  SIZ, SIZ,  FACEIDX_FRONT},
	{0.0, 1.0, 1.0,  0.0,  0.0, -1.0,  0.0, SIZ,  FACEIDX_FRONT},

	{0.0, 0.0, 0.0,  1.0,  0.0,  0.0,  0.0, 0.0,  FACEIDX_RIGHT},
	{0.0, 1.0, 0.0,  1.0,  0.0,  0.0,  0.0, SIZ,  FACEIDX_RIGHT},
	{0.0, 1.0, 1.0,  1.0,  0.0,  0.0,  SIZ, SIZ,  FACEIDX_RIGHT},
	{0.0, 0.0, 1.0,  1.0,  0.0,  0.0,  SIZ, 0.0,  FACEIDX_RIGHT},

	{1.0, 0.0, 0.0, -1.0,  0.0,  0.0,  SIZ, 0.0,  FACEIDX_LEFT},
	{1.0, 1.0, 0.0, -1.0,  0.0,  0.0,  SIZ, SIZ,  FACEIDX_LEFT},
	{1.0, 1.0, 1.0, -1.0,  0.0,  0.0,  0.0, SIZ,  FACEIDX_LEFT},
	{1.0, 0.0, 1.0, -1.0,  0.0,  0.0,  0.0, 0.0,  FACEIDX_LEFT},

	{0.0, 0.0, 0.0,  0.0, -1.0,  0.0,  SIZ, 0.0,  FACEIDX_BOTTOM},
	{0.0, 0.0, 1.0,  0.0, -1.0,  0.0,  0.0, 0.0,  FACEIDX_BOTTOM},
	{1.0, 0.0, 1.0,  0.0, -1.0,  0.0,  0.0, SIZ,  FACEIDX_BOTTOM},
	{1.0, 0.0, 0.0,  0.0, -1.0,  0.0,  SIZ, SIZ,  FACEIDX_BOTTOM},

	{0.0, 1.0, 0.0,  0.0,  1.0,  0.0,  SIZ, 0.0,  FACEIDX_TOP},
	{0.0, 1.0, 1.0,  0.0,  1.0,  0.0,  0.0, 0.0,  FACEIDX_TOP},
	{1.0, 1.0, 1.0,  0.0,  1.0,  0.0,  0.0, SIZ,  FACEIDX_TOP},
	{1.0, 1.0, 0.0,  0.0,  1.0,  0.0,  SIZ, SIZ,  FACEIDX_TOP}
};

static const uint16_t cube_indices[36] = {
	 0,  1,  2,    0,  2,  3, /* Back   */
	 6,  5,  4,	   7,  6,  4, /* Front  */
	 8,  9, 10,	   8, 10, 11, /* Left   */
	14, 13, 12,	  15, 14, 12, /* Right  */
	16, 17, 18,	  16, 18, 19, /* Bottom */
	22, 21, 20,	  23, 22, 20  /* Top    */
};

/* NOTE: the z and w components of these vectors are redundant as they are always 
 * the same, and could be defined as constants. They are used because the uniform 
 * block that receives this array must be of type vec4 (vec2 is not possible).
 *
 * In the future, could possibly pack the uv data so that x and y are one uv and 
 * z and w are another (packing 2 uv's into one vec4) 
 */
static const em_vec4 uv_lookup[MAX_BLOCK_TYPES * 6] = {
	[CUBETYPE_INVALID * 6 + FACEIDX_BACK]   = {0.0, 0.0, 0.0, 0.0},
	[CUBETYPE_INVALID * 6 + FACEIDX_FRONT]  = {0.0, 0.0, 0.0, 0.0},
	[CUBETYPE_INVALID * 6 + FACEIDX_RIGHT]  = {0.0, 0.0, 0.0, 0.0},
	[CUBETYPE_INVALID * 6 + FACEIDX_LEFT]   = {0.0, 0.0, 0.0, 0.0},
	[CUBETYPE_INVALID * 6 + FACEIDX_BOTTOM] = {0.0, 0.0, 0.0, 0.0},
	[CUBETYPE_INVALID * 6 + FACEIDX_TOP]    = {0.0, 0.0, 0.0, 0.0},

	[CUBETYPE_GRASS * 6 + FACEIDX_BACK]   = {TX(1), TX(15), SIZ, SIZ},
	[CUBETYPE_GRASS * 6 + FACEIDX_FRONT]  = {TX(1), TX(15), SIZ, SIZ},
	[CUBETYPE_GRASS * 6 + FACEIDX_RIGHT]  = {TX(1), TX(15), SIZ, SIZ},
	[CUBETYPE_GRASS * 6 + FACEIDX_LEFT]   = {TX(1), TX(15), SIZ, SIZ},
	[CUBETYPE_GRASS * 6 + FACEIDX_BOTTOM] = {TX(2), TX(15), SIZ, SIZ},
	[CUBETYPE_GRASS * 6 + FACEIDX_TOP]    = {TX(0), TX(15), SIZ, SIZ},

	[CUBETYPE_DIRT * 6 + FACEIDX_BACK]   = {TX(2), TX(15), SIZ, SIZ},
	[CUBETYPE_DIRT * 6 + FACEIDX_FRONT]  = {TX(2), TX(15), SIZ, SIZ},
	[CUBETYPE_DIRT * 6 + FACEIDX_RIGHT]  = {TX(2), TX(15), SIZ, SIZ},
	[CUBETYPE_DIRT * 6 + FACEIDX_LEFT]   = {TX(2), TX(15), SIZ, SIZ},
	[CUBETYPE_DIRT * 6 + FACEIDX_BOTTOM] = {TX(2), TX(15), SIZ, SIZ},
	[CUBETYPE_DIRT * 6 + FACEIDX_TOP]    = {TX(2), TX(15), SIZ, SIZ},

	[CUBETYPE_STONE * 6 + FACEIDX_BACK]   = {TX(3), TX(15), SIZ, SIZ},
	[CUBETYPE_STONE * 6 + FACEIDX_FRONT]  = {TX(3), TX(15), SIZ, SIZ},
	[CUBETYPE_STONE * 6 + FACEIDX_RIGHT]  = {TX(3), TX(15), SIZ, SIZ},
	[CUBETYPE_STONE * 6 + FACEIDX_LEFT]   = {TX(3), TX(15), SIZ, SIZ},
	[CUBETYPE_STONE * 6 + FACEIDX_BOTTOM] = {TX(3), TX(15), SIZ, SIZ},
	[CUBETYPE_STONE * 6 + FACEIDX_TOP]    = {TX(3), TX(15), SIZ, SIZ},

	[CUBETYPE_SAND * 6 + FACEIDX_BACK]   = {TX(4), TX(15), SIZ, SIZ},
	[CUBETYPE_SAND * 6 + FACEIDX_FRONT]  = {TX(4), TX(15), SIZ, SIZ},
	[CUBETYPE_SAND * 6 + FACEIDX_RIGHT]  = {TX(4), TX(15), SIZ, SIZ},
	[CUBETYPE_SAND * 6 + FACEIDX_LEFT]   = {TX(4), TX(15), SIZ, SIZ},
	[CUBETYPE_SAND * 6 + FACEIDX_BOTTOM] = {TX(4), TX(15), SIZ, SIZ},
	[CUBETYPE_SAND * 6 + FACEIDX_TOP] 	 = {TX(4), TX(15), SIZ, SIZ},

	[CUBETYPE_LOG * 6 + FACEIDX_BACK]   = {TX(6), TX(15), SIZ, SIZ},
	[CUBETYPE_LOG * 6 + FACEIDX_FRONT]  = {TX(6), TX(15), SIZ, SIZ},
	[CUBETYPE_LOG * 6 + FACEIDX_RIGHT]  = {TX(6), TX(15), SIZ, SIZ},
	[CUBETYPE_LOG * 6 + FACEIDX_LEFT]   = {TX(6), TX(15), SIZ, SIZ},
	[CUBETYPE_LOG * 6 + FACEIDX_BOTTOM] = {TX(5), TX(15), SIZ, SIZ},
	[CUBETYPE_LOG * 6 + FACEIDX_TOP]    = {TX(5), TX(15), SIZ, SIZ},

	[CUBETYPE_LEAF * 6 + FACEIDX_BACK]   = {TX(7), TX(15), SIZ, SIZ},
	[CUBETYPE_LEAF * 6 + FACEIDX_FRONT]  = {TX(7), TX(15), SIZ, SIZ},
	[CUBETYPE_LEAF * 6 + FACEIDX_RIGHT]  = {TX(7), TX(15), SIZ, SIZ},
	[CUBETYPE_LEAF * 6 + FACEIDX_LEFT]   = {TX(7), TX(15), SIZ, SIZ},
	[CUBETYPE_LEAF * 6 + FACEIDX_BOTTOM] = {TX(7), TX(15), SIZ, SIZ},
	[CUBETYPE_LEAF * 6 + FACEIDX_TOP]    = {TX(7), TX(15), SIZ, SIZ},
};

extern bool cube_is_transparent(cube_type_e type);
extern cube_t *cube_new(void);
extern void cube_init_lookup(cube_uv_lookup_t *lookup);

#endif
