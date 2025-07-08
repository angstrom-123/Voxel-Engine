#import "world_gen.h"

#define STONE_Y 26
#define DIRT_Y 30
#define GRASS_Y 31

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
		for (size_t y = 0; y < CHUNK_HEIGHT / 2; y++)
		{
			for (size_t z = 0; z < CHUNK_SIZE; z++)
			{
				cube_type_e type;
				if (y <= STONE_Y) type = CUBETYPE_STONE;
				else if (y <= DIRT_Y) type = CUBETYPE_DIRT;
				else type = CUBETYPE_GRASS;
				out->data->block_types[x][y][z] = type;
			}
		}
	}

	chunk_generate_mesh(out);

	return out;
}
