#import "world_gen.h"

chunk_t *gen_new_chunk(em_vec3 pos)
{
	chunk_t *out = malloc(sizeof(chunk_t));

	out->data = malloc(sizeof(chunk_data_t));
	memset(out->data->block_types, 0, sizeof(out->data->block_types));

	out->pos = pos;
	out->mesh.submeshes = NULL;
	out->mesh.submesh_cnt = 0;

	for (size_t x = 0; x < CHUNK_SIZE; x++)
	{
		for (size_t y = 0; y < CHUNK_HEIGHT; y++)
		{
			for (size_t z = 0; z < CHUNK_SIZE; z++)
			{
				out->data->block_types[x][y][z] = CUBETYPE_GRASS;
			}
		}
	}

	chunk_generate_mesh(out);

	return out;
}
