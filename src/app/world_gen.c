#include "world_gen.h"

static const iitvl ALLOWED_CRD = { 0, CHUNK_SIZE - 1 };

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
    INSTRUMENT_FUNC_BEGIN();
    const itvl ALLOWED_Y = { CHUNK_HEIGHT * 0.2, CHUNK_HEIGHT * 0.75 };

    if (!em_interval_contains(ALLOWED_Y, pos.y)) return;

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

        if (!em_iinterval_contains(ALLOWED_CRD, block_crd.x)) continue;
        if (!em_iinterval_contains(ALLOWED_CRD, block_crd.z)) continue;

        data->types[INDEX_3(block_crd)] = type;
    }
    }
    }
    INSTRUMENT_FUNC_END();
}

static bool load_model_file(file_t *f, gen_tree_t *out)
{
    RUNTIME_ASSERT(file_open(f, USAGE_READ), "Failed to open tree file for reading");
    size_t hdr_len = 3;
    char hdr_buf[hdr_len][STD_BUFLEN];
    RUNTIME_ASSERT(file_read_lines(f, &hdr_len, STD_BUFLEN, hdr_buf),
                   "Failed to read tree file header");
    out->size_x = strtoul(hdr_buf[0], NULL, 10);
    out->size_y = strtoul(hdr_buf[1], NULL, 10);
    out->size_z = strtoul(hdr_buf[2], NULL, 10);

    size_t body_len = out->size_z * out->size_y;
    char body_buf[body_len][STD_BUFLEN];
    RUNTIME_ASSERT(file_read_lines(f, &body_len, STD_BUFLEN, body_buf), 
                   "Failed to read tree file body");
    cube_type_e body_data[body_len][out->size_x];
    for (size_t i = 0; i < body_len; i++)
    {
        char tmp[STD_BUFLEN];
        strcpy(tmp, body_buf[i]);
        char *tok = strtok(body_buf[i], " ");
        for (int32_t j = 0; j < out->size_x; j++)
        {
            RUNTIME_ASSERT(tok != NULL, "Tree data must not be empty");
            body_data[i][j] = strtoul(tok, NULL, 10);
            tok = strtok(NULL, " ");
        }
    }

    RUNTIME_ASSERT(file_close(f), "Failed to close tree file");

    out->types = malloc(out->size_x * sizeof(cube_type_e *));
    for (int32_t x = 0; x < out->size_x; x++)
        out->types[x] = malloc(out->size_y * sizeof(cube_type_e *));

    for (int32_t x = 0; x < out->size_x; x++)
        for (int32_t y = 0; y < out->size_y; y++)
            out->types[x][y] = malloc(out->size_z * sizeof(cube_type_e));

    for (int32_t x = 0; x < out->size_x; x++)
    {
    for (int32_t y = 0; y < out->size_y; y++)
    {
    for (int32_t z = 0; z < out->size_z; z++)
    {
        int32_t i = x;
        int32_t j = z + (out->size_x * y);
        out->types[x][y][z] = body_data[j][i];
    }
    }
    }

    return true;
}

void load_model_files()
{
    #define model_file(f_name)\
    {\
        .base = MODEL_DATA_DIR,\
        .path = MODEL_DATA_DIR SEP QUOTE(f_name),\
        .name = QUOTE(f_name)\
    };\

    file_t f;

    f = (file_t) model_file(tree_basic);
    RUNTIME_ASSERT(load_model_file(&f, &tree_basic), "Failed to load tree basic file");

    f = (file_t) model_file(tree_tall);
    RUNTIME_ASSERT(load_model_file(&f, &tree_tall), "Failed to load tree tall file");

    f = (file_t) model_file(tree_pink);
    RUNTIME_ASSERT(load_model_file(&f, &tree_pink), "Failed to load tree tall file");
}

void unload_model_files()
{
    for (int32_t x = 0; x < tree_basic.size_x; x++)
        for (int32_t y = 0; y < tree_basic.size_y; y++)
            free(tree_basic.types[x][y]);
    for (int32_t x = 0; x < tree_basic.size_x; x++)
        free(tree_basic.types[x]);
    free(tree_basic.types);

    for (int32_t x = 0; x < tree_tall.size_x; x++)
        for (int32_t y = 0; y < tree_tall.size_y; y++)
            free(tree_tall.types[x][y]);
    for (int32_t x = 0; x < tree_tall.size_x; x++)
        free(tree_tall.types[x]);
    free(tree_tall.types);

    for (int32_t x = 0; x < tree_pink.size_x; x++)
        for (int32_t y = 0; y < tree_pink.size_y; y++)
            free(tree_pink.types[x][y]);
    for (int32_t x = 0; x < tree_pink.size_x; x++)
        free(tree_pink.types[x]);
    free(tree_pink.types);
}

chunk_data_t *generate_chunk_data(ivec2 pos, uint32_t seed) 
{
    INSTRUMENT_FUNC_BEGIN();
    chunk_data_t *data = malloc(sizeof(chunk_data_t));
    memset(&data->types[0][0][0], 0, sizeof(data->types));

    for (int32_t xx = -2; xx < CHUNK_SIZE + 2; xx++)
    {
        for (int32_t zz = -2; zz < CHUNK_SIZE + 2; zz++)
        {
            ivec2 xz = em_add_ivec2(pos, IVEC2(xx, zz));
            uint8_t h = _get_height(xz, seed);
            ivec3 block_pos = IVEC3(xz.x, h, xz.y);

            if (em_iinterval_contains(ALLOWED_CRD, xx) && em_iinterval_contains(ALLOWED_CRD, zz))
            {
                data->types[xx][h][zz] = CUBETYPE_GRASS;

                for (uint8_t y = 0; y < h - 4; y++) 
                    data->types[xx][y][zz] = CUBETYPE_STONE;

                for (uint8_t y = h - 4; y < h; y++) 
                    data->types[xx][y][zz] = CUBETYPE_DIRT;
            }

            if (_is_tree(block_pos, seed))
            {
                bool x_even = block_pos.x % 2 == 0;
                bool y_even = block_pos.y % 2 == 0;
                bool z_even = block_pos.z % 2 == 0;

                if (x_even && y_even && z_even)
                    _place_tree(block_pos, pos, data, tree_pink);
                else if (y_even) 
                    _place_tree(block_pos, pos, data, tree_tall);
                else
                    _place_tree(block_pos, pos, data, tree_basic);
            }
        }
    }

    INSTRUMENT_FUNC_END();
    data->edited = false;
    return data;
}
