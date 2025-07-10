#import "world_gen.h"
#include "src/extra_math.h"

#define STONE_Y 26
#define DIRT_Y 30
#define GRASS_Y 31

size_t push_to_state(state_t *state, chunk_set_t data)
{
	size_t new_size = (state->chunk_cnt + data.cnt) * sizeof(chunk_t);
	state->chunks = realloc(state->chunks, new_size);

	memcpy(&(state->chunks[state->chunk_cnt]), data.chunks, data.cnt * sizeof(chunk_t));

	size_t ofst = (size_t) state->chunk_cnt;
	state->chunk_cnt += data.cnt;

	return ofst;
}

em_ivec3 *gen_get_required_coords(em_vec3 centre, uint8_t rd, size_t *cnt)
{
	size_t coord_cnt = ((2 * rd * rd) + (2 * rd) + 1);
	em_ivec3 *out = malloc(sizeof(em_ivec3) * coord_cnt);

	int32_t x_pos = floorf(centre.x / (float) CHUNK_SIZE);
	int32_t y_pos = 0.0;
	int32_t z_pos = floorf(centre.z / (float) CHUNK_SIZE);

	size_t ctr = 0;
	for (int32_t x = x_pos - rd - 1; x < x_pos + rd + 1; x++)
	{
		for (int32_t z = z_pos - rd - 1; z < z_pos + rd + 1; z++)
		{
			int32_t mhttn = em_abs(z - z_pos) + em_abs(x - x_pos);
			if (mhttn > rd) continue;
			
			out[ctr++] = (em_ivec3) {x * 8, y_pos, z * 8};
		}
	}

	*cnt = coord_cnt;
	return out;
}

chunk_set_t gen_chunks_around(em_vec3 centre, uint8_t rd)
{
	size_t count = ((2 * rd * rd) + (2 * rd) + 1);
	chunk_t **chunks = malloc(sizeof(chunk_t *) * count);
	size_t cnt = 0;

	int32_t x_pos = floorf(centre.x / (float) CHUNK_SIZE);
	int32_t y_pos = 0.0;
	int32_t z_pos = floorf(centre.z / (float) CHUNK_SIZE);

	for (int32_t x = x_pos - rd - 1; x < x_pos + rd + 1; x++)
	{
		for (int32_t z = z_pos - rd - 1; z < z_pos + rd + 1; z++)
		{
			int32_t mhttn = em_abs(z - z_pos) + em_abs(x - x_pos);
			if (mhttn > rd) continue;

			chunks[cnt++] = gen_new_chunk(x * 8, y_pos, z * 8);
		}
	}

	return (chunk_set_t) {
		.chunks = chunks,
		.cnt = count
	};
}

chunk_t *gen_new_chunk(int32_t x, int32_t y, int32_t z)
{
	chunk_t *out = malloc(sizeof(chunk_t));

	out->blocks = malloc(sizeof(chunk_data_t));
	memset(out->blocks->types, 0, sizeof(out->blocks->types));

	out->x = x;
	out->y = y;
	out->z = z;

	out->loaded = false;
	out->staged = false;
	out->visible = true;
	out->age = 0;

	out->buf_data.v_ofst = 0;
	out->buf_data.i_ofst = 0;
	out->buf_data.v_len = 0;
	out->buf_data.i_len = 0;

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
