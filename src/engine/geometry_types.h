#ifndef GEOMETRY_TYPES_H
#define GEOMETRY_TYPES_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h> // size_t
#include <stdatomic.h>

#include <libem/em_math.h> // ivec2

#define CHUNK_SIZE 16 
#define CHUNK_HEIGHT 64

typedef struct offset {
    size_t v_ofst;
    size_t i_ofst;
} offset_t;

typedef struct vertex { 
	uint8_t x;
	uint8_t y;
	uint8_t z;
	uint8_t n;
	uint32_t uv;
} vertex_t;

typedef struct chunk_data {
	uint8_t types[CHUNK_SIZE][CHUNK_HEIGHT][CHUNK_SIZE];
} chunk_data_t;

typedef struct mesh {
    size_t i_cnt;
    size_t v_cnt;
	uint32_t __start_canary;
	vertex_t *v_buf;
	uint32_t __mid_canary;
	uint32_t *i_buf;
	uint32_t __end_canary;
} mesh_t;

typedef struct quad_desc {
	uint8_t x;
	uint8_t y;
	uint8_t z;
	uint8_t type;
	uint8_t face;
} quad_desc_t;

typedef enum cube_face_idx {
	FACEIDX_BACK   = 0,
	FACEIDX_FRONT  = 1,
	FACEIDX_RIGHT  = 2,
	FACEIDX_LEFT   = 3,
	FACEIDX_BOTTOM = 4,
	FACEIDX_TOP    = 5
} cube_face_idx_e;

typedef enum cube_type {
	CUBETYPE_AIR   = 0,
	CUBETYPE_GRASS = 1,
	CUBETYPE_DIRT  = 2,
	CUBETYPE_STONE = 3,
	CUBETYPE_SAND  = 4,
	CUBETYPE_LOG   = 5,
	CUBETYPE_LEAF  = 6,
    CUBETYPE_NUM
} cube_type_e;

#endif
