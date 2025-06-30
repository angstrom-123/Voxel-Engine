#include "mesh.h"

static const vertex_t cube_vertices[24] = {
	/* coord */ 	    /* default uv */
	/* back */
	{-0.5, -0.5, -0.5,    0,   0},
	{ 0.5, -0.5, -0.5,  UVM,   0},
	{ 0.5,  0.5, -0.5,  UVM, UVM},
	{-0.5,  0.5, -0.5,    0, UVM},
	/* front */
	{-0.5, -0.5,  0.5,    0,   0},
	{ 0.5, -0.5,  0.5,  UVM,   0},
	{ 0.5,  0.5,  0.5,  UVM, UVM},
	{-0.5,  0.5,  0.5,    0, UVM},
	/* left */
	{-0.5, -0.5, -0.5,    0,   0},
	{-0.5,  0.5, -0.5,  UVM,   0},
	{-0.5,  0.5,  0.5,  UVM, UVM},
	{-0.5, -0.5,  0.5,    0, UVM},
	/* right */
	{ 0.5, -0.5, -0.5,    0,   0},
	{ 0.5,  0.5, -0.5,  UVM,   0},
	{ 0.5,  0.5,  0.5,  UVM, UVM},
	{ 0.5, -0.5,  0.5,    0, UVM},
	/* bottom */
	{-0.5, -0.5, -0.5,    0,   0},
	{-0.5, -0.5,  0.5,  UVM,   0},
	{ 0.5, -0.5,  0.5,  UVM, UVM},
	{ 0.5, -0.5, -0.5,    0, UVM},
	/* top */
	{-0.5,  0.5, -0.5,    0,   0},
	{-0.5,  0.5,  0.5,  UVM,   0},
	{ 0.5,  0.5,  0.5,  UVM, UVM},
	{ 0.5,  0.5, -0.5,    0, UVM}
};

static const uint16_t cube_indices[36] = {
	 0,  1,  2,    0,  2,  3,
	 6,  5,  4,	   7,  6,  4,
	 8,  9, 10,	   8, 10, 11,
	14, 13, 12,	  15, 14, 12,
	16, 17, 18,	  16, 18, 19,
	22, 21, 20,	  23, 22, 20
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
