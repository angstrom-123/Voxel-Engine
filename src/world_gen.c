#import "world_gen.h"
#include "src/object.h"
#include <stdio.h>

#define STONE_Y 26
#define DIRT_Y 30
#define GRASS_Y 31

chunk_t **gen_chunks_around(em_vec3 centre, uint8_t rd)
{
	size_t num_chunks = ((2 * rd * rd) + (2 * rd) + 1);
	chunk_t **out = malloc(sizeof(chunk_t *) * num_chunks);
	size_t cnt = 0;

	int32_t x_pos = floorf(centre.x / (float) CHUNK_SIZE);
	int32_t y_pos = floorf(centre.y / (float) CHUNK_SIZE);
	int32_t z_pos = floorf(centre.z / (float) CHUNK_SIZE);

	for (int32_t x = x_pos - CHUNK_SIZE / 2; x < x_pos + CHUNK_SIZE / 2; x++)
	{
		for (int32_t z = z_pos - CHUNK_SIZE / 2; z < z_pos + CHUNK_SIZE / 2; z++)
		{
			size_t mhttn = em_abs(x) + em_abs(z);
			if (mhttn > rd) continue;
			
			out[cnt++] = gen_new_chunk((em_vec3) {(float) x, (float) y_pos, (float) z});
		}
	}

	return out;
}

chunk_t *gen_new_chunk(em_vec3 pos)
{
	chunk_t *out = malloc(sizeof(chunk_t));

	out->data = malloc(sizeof(chunk_data_t));
	memset(out->data->block_types, 0, sizeof(out->data->block_types));

	out->pos = pos;
	out->loaded = false;

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
