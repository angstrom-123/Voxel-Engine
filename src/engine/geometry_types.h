#ifndef GEOMETRY_TYPES_H
#define GEOMETRY_TYPES_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h> // size_t
#include <stdatomic.h>

#include <libem/em_math.h> // ivec2

#define CHUNK_SIZE 16 
#define CHUNK_HEIGHT 128

typedef struct offset {
    uint16_t v_ofst;
    uint16_t i_ofst;
} offset_t;

typedef struct vertex { 
	uint8_t x;
	uint8_t y;
	uint8_t z;
	uint8_t n;
	uint32_t uv;
} vertex_t;

typedef struct packed_vertex {
    uint8_t xo; // 5:x, 3:o
    uint8_t y;  // 8:y
    uint8_t zn; // 5:z, 3:n
    uint8_t t;  // 4:u, 4:v
} packed_vertex_t;

typedef struct chunk_data {
	uint8_t types[CHUNK_SIZE][CHUNK_HEIGHT][CHUNK_SIZE];
    bool edited;
} chunk_data_t;

typedef struct mesh {
    uint16_t i_cnt;
    uint16_t v_cnt;
    packed_vertex_t *v_buf;
    uint16_t *i_buf;
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
	CUBETYPE_AIR,
	CUBETYPE_GRASS,
	CUBETYPE_DIRT,
	CUBETYPE_STONE,
	CUBETYPE_SAND,
	CUBETYPE_LOG,
	CUBETYPE_LEAF,
	CUBETYPE_LOG_P,
	CUBETYPE_LEAF_P,
    CUBETYPE_NUM
} cube_type_e;

typedef enum tex_type {
	TEXTYPE_AIR,
	TEXTYPE_GRASS_TOP,
	TEXTYPE_GRASS_SIDE,
	TEXTYPE_DIRT,
	TEXTYPE_STONE,
	TEXTYPE_SAND,
	TEXTYPE_LOG_TOP,
	TEXTYPE_LOG_SIDE,
	TEXTYPE_LEAF,
	TEXTYPE_LOG_P_TOP,
	TEXTYPE_LOG_P_SIDE,
    TEXTYPE_LEAF_P,
    TEXTYPE_NUM
} tex_type_e;

#endif
