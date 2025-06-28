#include "mesh.h"

cube_t *mesh_generate_cube(void)
{
	int16_t max = INT16_MAX;

	vertex_t vertices[24] = {
		/* coord */ 	    /* default uv */
		/* back */
		{-1.0, -1.0, -1.0,    0,   0},
		{ 1.0, -1.0, -1.0,  max,   0},
		{ 1.0,  1.0, -1.0,  max, max},
		{-1.0,  1.0, -1.0,    0, max},
		/* front */
		{-1.0, -1.0,  1.0,    0,   0},
		{ 1.0, -1.0,  1.0,  max,   0},
		{ 1.0,  1.0,  1.0,  max, max},
		{-1.0,  1.0,  1.0,    0, max},
		/* left */
		{-1.0, -1.0, -1.0,    0,   0},
		{-1.0,  1.0, -1.0,  max,   0},
		{-1.0,  1.0,  1.0,  max, max},
		{-1.0, -1.0,  1.0,    0, max},
		/* right */
		{ 1.0, -1.0, -1.0,    0,   0},
		{ 1.0,  1.0, -1.0,  max,   0},
		{ 1.0,  1.0,  1.0,  max, max},
		{ 1.0, -1.0,  1.0,    0, max},
		/* bottom */
		{-1.0, -1.0, -1.0,    0,   0},
		{-1.0, -1.0,  1.0,  max,   0},
		{ 1.0, -1.0,  1.0,  max, max},
		{ 1.0, -1.0, -1.0,    0, max},

		{-1.0,  1.0, -1.0,    0,   0},
		{-1.0,  1.0,  1.0,  max,   0},
		{ 1.0,  1.0,  1.0,  max, max},
		{ 1.0,  1.0, -1.0,    0, max}
	};

	uint16_t indices[36] = {
		 0,  1,  2,    0,  2,  3,
		 6,  5,  4,	   7,  6,  4,
		 8,  9, 10,	   8, 10, 11,
		14, 13, 12,	  15, 14, 12,
		16, 17, 18,	  16, 18, 19,
		22, 21, 20,	  23, 22, 20
	};

	cube_t *out = malloc(sizeof(cube_t));
	out->v_len = 24;
	out->i_len = 36;

	memcpy(out->vertices, vertices, sizeof(vertices));
	memcpy(out->indices, indices, sizeof(indices));

	return out;
}
