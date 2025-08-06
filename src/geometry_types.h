#ifndef GEOMETRY_TYPES_H
#define GEOMETRY_TYPES_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h> // size_t

#define CHUNK_SIZE 16 
#define CHUNK_HEIGHT 64

typedef struct vertex { 
	uint8_t x;
	uint8_t y;
	uint8_t z;
	uint8_t n;
	uint32_t uv;
} vertex_t;

typedef struct chunk_data {
	int8_t types[CHUNK_SIZE][CHUNK_HEIGHT][CHUNK_SIZE];
} chunk_data_t;

typedef struct mesh {
	uint32_t v_rsrv;  // Size of buffer chunk reserved for vertices
	uint32_t i_rsrv;  // Size of buffer chunk reserved for indices
	uint32_t v_cnt;  // Actual length of vertex data 
	uint32_t i_cnt;  // Actual length of index data
	uint32_t __start_canary;
	vertex_t *v_buf; // Pointer to vertex data
	uint32_t __mid_canary;
	uint32_t *i_buf; // Pointer to index data
	uint32_t __end_canary;
} mesh_t;

typedef struct buf_offsets {
	size_t v_ofst;
	size_t i_ofst;
	size_t v_len;
	size_t i_len;
    size_t num_slots;
} buf_offsets_t;

typedef struct chunk {
	int32_t x;
	int32_t y;
	int32_t z;

	chunk_data_t *blocks;
	buf_offsets_t buf_data;
	mesh_t mesh;
	
	bool staged;
	bool visible;
    bool meshed;

	uint8_t age;
    uint64_t creation_frame;
} chunk_t;

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
    CUBETYPE_NONE  = 10
} cube_type_e;

#endif
