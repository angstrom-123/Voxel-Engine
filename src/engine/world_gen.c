#include "world_gen.h"

shell_t _get_next_shell(shell_t *prev, ivec2 c, uint8_t **mat, size_t mp)
{
    shell_t res;
    size_t cnt;
    size_t idx = 0;

    if (prev)
    {
        cnt = (prev->lvl + 1) * 4;
        res = (shell_t) {
            .cnt = cnt,
            .lvl = prev->lvl + 1,
            .crds = malloc(cnt * sizeof(ivec2))
        };
    }
    else 
    {
        cnt = 1;
        res = (shell_t) {
            .cnt = cnt,
            .lvl = 0,
            .crds = malloc(cnt * sizeof(ivec2))
        };
        res.crds[0] = c;

        return res;
    }

    const ivec2 moves[4] = {{0, CHUNK_SIZE}, {CHUNK_SIZE, 0}, {0, -CHUNK_SIZE}, {-CHUNK_SIZE, 0}};

    for (size_t i = 0; i < prev->cnt; i++)
    {
        ivec2 prev_crd = prev->crds[i];

        const bool bad_cardinals[4] = {
            prev_crd.y < c.y,
            prev_crd.x < c.x,
            prev_crd.y > c.y,
            prev_crd.x > c.x
        };

        for (size_t j = 0; j < 4; j++)
        {
            if (bad_cardinals[j])
                continue;

            ivec2 next_crd = em_add_ivec2(prev_crd, moves[j]);
            uint8_t *mat_val = &mat[mp - (c.y - next_crd.y) / CHUNK_SIZE]
                                   [mp - (c.x - next_crd.x) / CHUNK_SIZE];

            if (*mat_val == UINT8_MAX)
            {
                res.crds[idx++] = next_crd;
                *mat_val = res.lvl;
            }
        }

        if (idx == cnt)
            break;
    }

    return res;
}

ivec2 *gen_get_coords(ivec2 c, size_t num, size_t *cnt)
{
    *cnt = (2 * em_sqr(num)) + (2 * num) + 1;
    ivec2 *res = malloc(*cnt * sizeof(ivec2));
    size_t idx = 0;

    int32_t rd = num;
    int32_t x0 = c.x / CHUNK_SIZE;
    int32_t z0 = c.y / CHUNK_SIZE;

    for (int32_t x = x0 - rd - 1; x < x0 + rd + 2; x++)
    {
        for (int32_t z = z0 - rd - 1; z < z0 + rd + 2; z++)
        {
            ivec2 crd = {
                .x = x * CHUNK_SIZE, 
                .y = z * CHUNK_SIZE
            };

            /* Calculate manhattan distance. */
            int32_t m = em_abs(z - z0) + em_abs(x - x0);
            if (m <= rd)
            {
                res[idx++] = crd;
            }
        }
    }

    return res;
}

shell_t *gen_get_shells(ivec2 c, size_t num, shell_t *start_shell)
{
    shell_t *res = malloc(num * sizeof(shell_t));

    /* Matrix representing chunk grid, value stores distance from centre. */
    size_t mp = (start_shell) 
              ? start_shell->lvl + num 
              : num;
    size_t dim = 2 * mp + 1;
    uint8_t **matrix = malloc(dim * sizeof(uint8_t *));
    for (size_t i = 0; i < dim; i++)
    {
        matrix[i] = malloc(dim * sizeof(uint8_t));
        memset(matrix[i], UINT8_MAX, dim * sizeof(uint8_t));
    }

    shell_t *prev_shell = start_shell;
    for (size_t i = 0; i < num; i++)
    {
        res[i] = _get_next_shell(prev_shell, c, matrix, mp);
        prev_shell = &res[i];
    }

    for (size_t i = 0; i < dim; i++)
        free(matrix[i]);
    free(matrix);

    return res;
}

chunk_data_t *gen_generate_chunk_data(ivec2 pos, uint32_t seed) 
{
    chunk_data_t *data = malloc(sizeof(chunk_data_t));
    memset(&data->types[0][0][0], 0, sizeof(data->types));

    for (int32_t x1 = 0; x1 < CHUNK_SIZE; x1++)
    {
        for (int32_t z1 = 0; z1 < CHUNK_SIZE; z1++)
        {
            float p = perlin_octave_2d(seed, pos.x + x1, pos.y + z1, 0.004, 6);
            float n = (p + 1.0) / 2.0;
            uint8_t h = floorf(n * (CHUNK_HEIGHT - 1));
            data->types[x1][h][z1] = CUBETYPE_GRASS;

            for (uint8_t y = 0; y < h - 4; y++) 
                data->types[x1][y][z1] = CUBETYPE_STONE;

            for (uint8_t y = h - 4; y < h; y++) 
                data->types[x1][y][z1] = CUBETYPE_DIRT;
        }
    }

    return data;
}
