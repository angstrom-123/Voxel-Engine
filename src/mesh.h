#ifndef MESH_H
#define MESH_H 

#include <stdint.h>
#include <stdlib.h>
#include <memory.h>

#include "extra_math.h"

#define UVM INT16_MAX

typedef struct vertex {
	float x;
	float y;
	float z;

	float nx;
	float ny;
	float nz;

	/* NOTE: using int16_t for coords so max value is 32767. Texture atlas is 
	 * 256x256 pixels so for pixel-perfect UVs, U and V must be multiples of 128.
	 * (because 128 is equal to 1 pixel)
	 */
	int16_t u;
	int16_t v;
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

extern cube_t *mesh_generate_cube(void);

#endif
