#import "world_gen.h"

#define STONE_Y 26
#define DIRT_Y 30
#define GRASS_Y 31

chunk_set_t gen_chunks_around(em_vec3 centre, uint8_t rd)
{
	size_t num_chunks = ((2 * rd * rd) + (2 * rd) + 1);
	chunk_t **chunks = malloc(sizeof(chunk_t *) * num_chunks);
	size_t cnt = 0;

	int32_t x_pos = floorf(centre.x / (float) CHUNK_SIZE);
	int32_t y_pos = 0.0;
	int32_t z_pos = floorf(centre.z / (float) CHUNK_SIZE);

	for (int32_t x = x_pos - CHUNK_SIZE / 2; x < x_pos + CHUNK_SIZE / 2; x++)
	{
		for (int32_t z = z_pos - CHUNK_SIZE / 2; z < z_pos + CHUNK_SIZE / 2; z++)
		{
			int mhttn = em_abs(z - z_pos) + em_abs(x - x_pos);
			if (mhttn > rd) continue;
			
			chunks[cnt++] = gen_new_chunk((em_vec3) {(float) x * 8.0, (float) y_pos, (float) z * 8.0});
		}
	}

	return (chunk_set_t) {
		.chunks = chunks,
		.count = num_chunks
	};
}

chunk_t *gen_new_chunk(em_vec3 pos)
{
	chunk_t *out = malloc(sizeof(chunk_t));

	out->blocks = malloc(sizeof(chunk_data_t));
	memset(out->blocks->types, 0, sizeof(out->blocks->types));

	out->pos = pos;
	out->loaded = false;
	out->vbo_offsets.v_offset = 0;
	out->vbo_offsets.i_offset = 0;
	out->vbo_offsets.v_len = 0;
	out->vbo_offsets.i_len = 0;

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

				out->blocks->types[x][y][z] = type;
			}
		}
	}

	chunk_generate_mesh(out);

	return out;
}
