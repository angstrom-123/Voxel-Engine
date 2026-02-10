#include "world_gen.h"

static const i_interval_t ALLOWED_CRD = { 0, CHUNK_SIZE - 1 };

bool _is_tree(ivec3 pos, uint32_t seed)
{
    int32_t tree_seed = perlin_pair(pos.x, pos.z) + seed;
    int32_t tree_hash = perlin_hash(tree_seed);
    float n = (float) tree_hash / (float) INT32_MAX;
    const float THRES = 0.997;
    return n > THRES;
}

uint8_t _get_height(ivec2 pos, uint32_t seed)
{
    float p = perlin_octave_2d(seed, pos.x, pos.y, PERLIN_SCALE, PERLIN_OCTAVES);
    float n = (p + 1.0) / 2.0;
    n = powf(n, 2);
    n += 0.22;
    return floorf(n * (CHUNK_HEIGHT - 1));
}

void _place_tree(ivec3 pos, ivec2 chunk_pos, chunk_data_t *data, gen_tree_t tree)
{
    const interval_t ALLOWED_Y = { CHUNK_HEIGHT * 0.2, CHUNK_HEIGHT * 0.75 };

    if (!interval_contains(ALLOWED_Y, pos.y)) return;

    pos.y += 1;
    ivec2 p = em_sub_ivec2(IVEC2(pos.x, pos.z), chunk_pos);

    for (int32_t x = 0; x < tree.size_x; x++)
    {
    for (int32_t y = 0; y < tree.size_y; y++)
    {
    for (int32_t z = 0; z < tree.size_z; z++)
    {
        cube_type_e type = tree.types[x][y][z];
        if (type == CUBETYPE_AIR) continue;
        
        ivec3 block_crd = IVEC3(x + p.x - (tree.size_x / 2), 
                                y + pos.y,
                                z + p.y - (tree.size_z / 2));

        if (!interval_contains_i(ALLOWED_CRD, block_crd.x)) continue;
        if (!interval_contains_i(ALLOWED_CRD, block_crd.z)) continue;

        data->types[INDEX_3(block_crd)] = type;
    }
    }
    }
}

bool load_model_files()
{
    file_t f = {
        .base = MODEL_DATA_DIR,
        .path = MODEL_DATA_DIR SEP "tree_basic",
        .name = "tree_basic",
    };
    RUNTIME_ASSERT(file_open(&f, USAGE_READ), "Failed to open tree file for reading");
    size_t hdr_len = 3;
    char hdr_buf[hdr_len][STD_BUFLEN];
    RUNTIME_ASSERT(file_read_lines(&f, &hdr_len, STD_BUFLEN, hdr_buf),
                   "Failed to read tree file header");
    tree_basic.size_x = strtoul(hdr_buf[0], NULL, 10);
    tree_basic.size_y = strtoul(hdr_buf[1], NULL, 10);
    tree_basic.size_z = strtoul(hdr_buf[2], NULL, 10);

    size_t body_len = tree_basic.size_z * tree_basic.size_y;
    char body_buf[body_len][STD_BUFLEN];
    RUNTIME_ASSERT(file_read_lines(&f, &body_len, STD_BUFLEN, body_buf), 
                   "Failed to read tree file body");
    cube_type_e body_data[body_len][tree_basic.size_x];
    for (size_t i = 0; i < body_len; i++)
    {
        char tmp[STD_BUFLEN];
        strcpy(tmp, body_buf[i]);
        char *tok = strtok(body_buf[i], " ");
        for (int32_t j = 0; j < tree_basic.size_x; j++)
        {
            RUNTIME_ASSERT(tok != NULL, "Tree data must not be empty");
            body_data[i][j] = strtoul(tok, NULL, 10);
            tok = strtok(NULL, " ");
        }
    }

    for (size_t i = 0; i < body_len; i++)
    {
        for (int32_t j = 0; j < tree_basic.size_x; j++)
            PUT_WARN("%hhu ", body_data[i][j]);
        PUT_WARN("\n", NULL);
    }

    tree_basic.types = malloc(tree_basic.size_x * sizeof(cube_type_e *));
    for (int32_t x = 0; x < tree_basic.size_x; x++)
        tree_basic.types[x] = malloc(tree_basic.size_y * sizeof(cube_type_e *));

    for (int32_t x = 0; x < tree_basic.size_x; x++)
        for (int32_t y = 0; y < tree_basic.size_y; y++)
            tree_basic.types[x][y] = malloc(tree_basic.size_z * sizeof(cube_type_e));

    for (int32_t x = 0; x < tree_basic.size_x; x++)
    {
    for (int32_t y = 0; y < tree_basic.size_y; y++)
    {
    for (int32_t z = 0; z < tree_basic.size_z; z++)
    {
        int32_t i = x;
        int32_t j = z + (tree_basic.size_x * y);
        APP_LOG_OK("%i, %i => %i, %i, %i", i, j, x, y, z);
        tree_basic.types[x][y][z] = body_data[j][i];
    }
    }
    }

    for (int32_t y = 0; y < tree_basic.size_y; y++)
    {
        for (int32_t x = 0; x < tree_basic.size_x; x++)
        {
            for (int32_t z = 0; z < tree_basic.size_z; z++)
                PUT_WARN("%hhu ", tree_basic.types[x][y][z]);
            PUT_WARN("\n", NULL);
        }
        PUT_WARN("\n\n", NULL);
    }

    return true;
}

void unload_model_files()
{
    for (int32_t x = 0; x < tree_basic.size_x; x++)
        for (int32_t y = 0; y < tree_basic.size_y; y++)
            free(tree_basic.types[x][y]);

    for (int32_t x = 0; x < tree_basic.size_x; x++)
        free(tree_basic.types[x]);

    free(tree_basic.types);
}

chunk_data_t *generate_chunk_data(ivec2 pos, uint32_t seed) 
{
    chunk_data_t *data = malloc(sizeof(chunk_data_t));
    memset(&data->types[0][0][0], 0, sizeof(data->types));

    for (int32_t xx = -2; xx < CHUNK_SIZE + 2; xx++)
    {
        for (int32_t zz = -2; zz < CHUNK_SIZE + 2; zz++)
        {
            ivec2 xz = em_add_ivec2(pos, IVEC2(xx, zz));
            uint8_t h = _get_height(xz, seed);
            ivec3 block_pos = IVEC3(xz.x, h, xz.y);

            if (interval_contains_i(ALLOWED_CRD, xx) && 
                interval_contains_i(ALLOWED_CRD, zz))
            {
                data->types[xx][h][zz] = CUBETYPE_GRASS;

                for (uint8_t y = 0; y < h - 4; y++) 
                    data->types[xx][y][zz] = CUBETYPE_STONE;

                for (uint8_t y = h - 4; y < h; y++) 
                    data->types[xx][y][zz] = CUBETYPE_DIRT;
            }

            if (_is_tree(block_pos, seed))
                _place_tree(block_pos, pos, data, tree_basic);
        }
    }

    data->edited = false;
    return data;
}
