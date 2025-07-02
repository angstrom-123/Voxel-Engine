#include "mesh.h"

static const vertex_t cube_vertices[24] = {
	/* coord */ 	    /* default uv */
	/* Back */
	{ 0.0,  0.0,  0.0,  0.0,  0.0,  1.0,    0,   0},
	{ 1.0,  0.0,  0.0,  0.0,  0.0,  1.0,  UVM,   0},
	{ 1.0,  1.0,  0.0,  0.0,  0.0,  1.0,  UVM, UVM},
	{ 0.0,  1.0,  0.0,  0.0,  0.0,  1.0,    0, UVM},
	/* Front */
	{ 0.0,  0.0,  1.0,  0.0,  0.0, -1.0,    0,   0},
	{ 1.0,  0.0,  1.0,  0.0,  0.0, -1.0,  UVM,   0},
	{ 1.0,  1.0,  1.0,  0.0,  0.0, -1.0,  UVM, UVM},
	{ 0.0,  1.0,  1.0,  0.0,  0.0, -1.0,    0, UVM},
	/* Left */
	{ 0.0,  0.0,  0.0, -1.0,  0.0,  0.0,    0,   0},
	{ 0.0,  1.0,  0.0, -1.0,  0.0,  0.0,  UVM,   0},
	{ 0.0,  1.0,  1.0, -1.0,  0.0,  0.0,  UVM, UVM},
	{ 0.0,  0.0,  1.0, -1.0,  0.0,  0.0,    0, UVM},
	/* Right */
	{ 1.0,  0.0,  0.0,  1.0,  0.0,  0.0,     0,   0},
	{ 1.0,  1.0,  0.0,  1.0,  0.0,  0.0,   UVM,   0},
	{ 1.0,  1.0,  1.0,  1.0,  0.0,  0.0,   UVM, UVM},
	{ 1.0,  0.0,  1.0,  1.0,  0.0,  0.0,     0, UVM},
	/* Bottom */
	{ 0.0,  0.0,  0.0,  0.0, -1.0,  0.0,     0,   0},
	{ 0.0,  0.0,  1.0,  0.0, -1.0,  0.0,   UVM,   0},
	{ 1.0,  0.0,  1.0,  0.0, -1.0,  0.0,   UVM, UVM},
	{ 1.0,  0.0,  0.0,  0.0, -1.0,  0.0,     0, UVM},
	/* Top */
	{ 0.0,  1.0,  0.0,  0.0,  1.0,  0.0,     0,   0},
	{ 0.0,  1.0,  1.0,  0.0,  1.0,  0.0,   UVM,   0},
	{ 1.0,  1.0,  1.0,  0.0,  1.0,  0.0,   UVM, UVM},
	{ 1.0,  1.0,  0.0,  0.0,  1.0,  0.0,     0, UVM}
};

static const uint16_t cube_indices[36] = {
	 0,  1,  2,    0,  2,  3, /* Back */
	 6,  5,  4,	   7,  6,  4, /* Front */
	 8,  9, 10,	   8, 10, 11, /* Left */
	14, 13, 12,	  15, 14, 12, /* Right */
	16, 17, 18,	  16, 18, 19, /* Bottom */
	22, 21, 20,	  23, 22, 20  /* Top */
};

cube_t *mesh_generate_cube(void)
{
	cube_t *out = malloc(sizeof(cube_t));

	out->v_len = sizeof(cube_vertices) / sizeof(cube_vertices[0]);
	out->i_len = sizeof(cube_indices) / sizeof(cube_indices[1]);

	memcpy(out->vertices, cube_vertices, sizeof(cube_vertices));
	memcpy(out->indices, cube_indices, sizeof(cube_indices));

	return out;
}
