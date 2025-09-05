#include "world_gen.h"

ivec2_tuple_t gen_get_coords(vec3 c, uint8_t rd)
{
    assert(rd > 2);

    /* Formula gives number of chunks within manhattan distance rd. */
    size_t inner_cnt = (2 * em_sqr(rd - 2)) + (2 * (rd - 2)) + 1;
    size_t edge_cnt = (rd - 1) * 4;
    size_t fringe_cnt = rd * 4;
    size_t outer_cnt = (rd + 1) * 4;

    ivec2 *buf = malloc(sizeof(ivec2) * (inner_cnt + edge_cnt + fringe_cnt + outer_cnt));
    ivec2 *inner = &buf[0];
    ivec2 *edge = &buf[inner_cnt];
    ivec2 *fringe = &buf[inner_cnt + edge_cnt];
    ivec2 *outer = &buf[inner_cnt + edge_cnt + fringe_cnt];

    /* Coords of chunk containing c. */
    int32_t x0 = floorf(c.x / (float) CHUNK_SIZE);
    int32_t z0 = floorf(c.z / (float) CHUNK_SIZE);

    size_t ctr_a = 0;
    size_t ctr_b = 0;
    size_t ctr_c = 0;
    size_t ctr_d = 0;
    for (int32_t x = x0 - rd - 1; x < x0 + rd + 2; x++) // NOTE: Was -2 but I changed it.
    {
        for (int32_t z = z0 - rd - 1; z < z0 + rd + 2; z++)
        {
            ivec2 crd = {x * 16, z * 16};

            /* Calculate manhattan distance. */
            int32_t m = em_abs(z - z0) + em_abs(x - x0);
            if (m > rd) // outside of render distance.
            {
                if (m == rd + 1) // 1 beyond render dist : outer.
                    outer[ctr_d++] = crd;

                continue; // Too far : discard
            }

            if (m == rd) // Exactly at render distance : fringe.
                fringe[ctr_c++] = crd;
            else if (m == rd - 1) // Just before render distance : edge.
                edge[ctr_b++] = crd;
            else // Within middle of chunks : inner.
                inner[ctr_a++] = crd;
        }
    }

    ivec2_tuple_t res = {
        .arrays = malloc(4 * sizeof(ivec2 *)),
        .sizes = malloc(4 * sizeof(size_t))
    };

    res.arrays[0] = inner;
    res.sizes[0] = inner_cnt;

    res.arrays[1] = edge;
    res.sizes[1] = edge_cnt;

    res.arrays[2] = fringe;
    res.sizes[2] = fringe_cnt;

    res.arrays[3] = outer;
    res.sizes[3] = outer_cnt;

    return res;
}

chunk_t *gen_new_chunk(uint32_t seed, int32_t x, int32_t z)
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
    chunk->z = z;
    chunk->dirty = false;
    chunk->edited = false;
    chunk->creation_frame = 0;
    chunk->offsets.v_ofst = 0;
    chunk->offsets.i_ofst = 0;

    for (int32_t x1 = 0; x1 < CHUNK_SIZE; x1++)
    {
        for (int32_t z1 = 0; z1 < CHUNK_SIZE; z1++)
        {
            float p = perlin_octave_2d(seed, x + x1, z + z1, 3);
            float n = (p + 1.0) / 2.0;
            uint8_t h = floor(n * (CHUNK_HEIGHT - 1));
            chunk->blocks->types[x1][h][z1] = CUBETYPE_GRASS;

            for (uint8_t y = 0; y < h - 4; y++) 
                chunk->blocks->types[x1][y][z1] = CUBETYPE_STONE;

            for (uint8_t y = h - 4; y < h; y++) 
                chunk->blocks->types[x1][y][z1] = CUBETYPE_DIRT;
        }
    }

    return chunk;
}
