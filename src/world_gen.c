#import "world_gen.h"
#include "src/geometry.h"

ivec3 *gen_get_required_coords(vec3 c, uint8_t rd, size_t *cnt)
{
	srand(273912312);
	/* Formula gives number of chunks within manhattan distance rd. */
	size_t num = ((2 * rd * rd) + (2 * rd) + 1);
	ivec3 *out = malloc(sizeof(ivec3) * num);

	/* Coords of chunk containing c. */
	int32_t x0 = floorf(c.x / (float) CHUNK_SIZE);
	int32_t y0 = 0.0;
	int32_t z0 = floorf(c.z / (float) CHUNK_SIZE);

	size_t ctr = 0;
	for (int32_t x = x0 - rd - 1; x < x0 + rd + 1; x++)
	{
		for (int32_t z = z0 - rd - 1; z < z0 + rd + 1; z++)
		{
			/* Calculate manhattan distance. */
			int32_t m = em_abs(z - z0) + em_abs(x - x0);
			if (m > rd) 
			{
				continue; // Outside of render distance.
			}

			// out[ctr++] = (ivec3) {x * 8, y0, z * 8};
			out[ctr++] = (ivec3) {x * 16, y0, z * 16};
		}
	}

	*cnt = num;

	return out;
}

chunk_t *gen_new_chunk(int32_t x, int32_t y, int32_t z)
{
	chunk_t *chunk = malloc(sizeof(chunk_t));
	chunk->blocks = malloc(sizeof(chunk_data_t));
	
	if (!chunk || !chunk->blocks) 
	{
		fprintf(stderr, "Failed to allocate new chunk.\n");
		exit(1);
	}

	memset(chunk->blocks->types, CUBETYPE_AIR, sizeof(chunk->blocks->types));

	/* Defaults. */
	chunk->x = x;
	chunk->y = y;
	chunk->z = z;

	chunk->staged = false;
	chunk->visible = true;

	chunk->age = 0;

	chunk->buf_data.v_ofst = 0;
	chunk->buf_data.i_ofst = 0;
	chunk->buf_data.v_len = 0;
	chunk->buf_data.i_len = 0;
	
	// float r = (float) rand() / (float) INT_MAX;

	// uint8_t h = roundf(r * (CHUNK_HEIGHT - 1) / 10);
	// printf("%hhu\n", h);
	// uint8_t h;
	// if (x < 0) h = 10;
	// else h = 20;
	for (int32_t x1 = 0; x1 < CHUNK_SIZE; x1++)
	{
		for (int32_t z1 = 0; z1 < CHUNK_SIZE; z1++)
		{
			// float p = perlin_2d(x + x1, z + z1, 0.02);
			float p = perlin_octave_2d(x + x1, z + z1, 5);
			float n = (p + 1.0) / 2.0;
			uint8_t h = roundf(n * (CHUNK_HEIGHT - 1) * 1.0);
			// uint8_t h = 32;
			chunk->blocks->types[x1][h][z1] = CUBETYPE_GRASS;
		}
	}

	chunk_generate_mesh(chunk);
	return chunk;
}
