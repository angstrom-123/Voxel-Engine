#include "cube.h"

bool cube_is_transparent(cube_type_e type)
{
	switch (type) {
		case CUBETYPE_LEAF: return true;
		default: return false;
	};
}

cube_t *cube_new(void)
{
	cube_t *out = malloc(sizeof(cube_t));

	out->v_len = sizeof(cube_vertices) / sizeof(cube_vertices[0]);
	out->i_len = sizeof(cube_indices) / sizeof(cube_indices[1]);

	memcpy(out->vertices, cube_vertices, sizeof(cube_vertices));
	memcpy(out->indices, cube_indices, sizeof(cube_indices));

	return out;
}

void cube_init_lookup(cube_uv_lookup_t *lookup)
{
	memcpy(lookup->uv_rects, uv_lookup, sizeof(uv_lookup));
}
