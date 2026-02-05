#include "world_gen.h"

bool _is_tree(ivec2 pos, uint32_t seed)
{
    // TODO!
    (void) pos; (void) seed;
    RUNTIME_ASSERT(false, "Unimplemented");
    return false;
}

uint8_t _get_height(ivec2 pos, uint32_t seed)
{
    float p = perlin_octave_2d(seed, pos.x, pos.y, PERLIN_SCALE, PERLIN_OCTAVES);
    float n = (p + 1.0) / 2.0;
    n = powf(n, 2);
    n += 0.22;
    return floorf(n * (CHUNK_HEIGHT - 1));
}

void _place_tree(chunk_data_t *cd, ivec2 pos, uint32_t seed)
{
    const interval_t ALLOWED_Y = { CHUNK_HEIGHT * 0.2, CHUNK_HEIGHT * 0.75 };
    uint8_t h = _get_height(pos, seed);
    if (!interval_contains(ALLOWED_Y, h))
    {
        APP_LOG_WARN("Failed to place tree at %i %i (Too high or too low)", pos.x, pos.y);
        return;
    }

    ivec2 p = {
        pos.x - floorf((float) pos.x / CHUNK_SIZE) * CHUNK_SIZE,
        pos.y - floorf((float) pos.y / CHUNK_SIZE) * CHUNK_SIZE,
    };
    for (size_t i = 0; i < 5; i++)
        cd->types[p.x][++h][p.y] = CUBETYPE_LOG;
}

chunk_data_t *generate_chunk_data(ivec2 pos, uint32_t seed) 
{
    chunk_data_t *data = malloc(sizeof(chunk_data_t));
    memset(&data->types[0][0][0], 0, sizeof(data->types));

    for (int32_t xx = 0; xx < CHUNK_SIZE; xx++)
    {
        for (int32_t zz = 0; zz < CHUNK_SIZE; zz++)
        {
            ivec2 block_pos = em_add_ivec2(pos, IVEC2(xx, zz));
            uint8_t h = _get_height(block_pos, seed);
            data->types[xx][h][zz] = CUBETYPE_GRASS;

            for (uint8_t y = 0; y < h - 4; y++) 
                data->types[xx][y][zz] = CUBETYPE_STONE;

            for (uint8_t y = h - 4; y < h; y++) 
                data->types[xx][y][zz] = CUBETYPE_DIRT;

            // if (_is_tree(block_pos, seed))
            // {
            //     _place_tree(data, block_pos, seed);
            // }
        }
    }

    data->edited = false;
    return data;
}
