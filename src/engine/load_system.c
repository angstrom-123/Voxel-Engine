#include "load_system.h"

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

ivec2 *_gen_get_coords(ivec2 c, size_t num, size_t *cnt)
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

shell_t *_gen_get_shells(ivec2 c, size_t num, shell_t *start_shell)
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


void load_sys_init(load_system_t *ls, const load_system_desc_t *desc)
{
    ls->curr_pos = desc->start_pos;
    ls->load_dist = desc->render_dist + 1;
    ls->base_chunk_coords = _gen_get_coords((ivec2) {0, 0}, desc->render_dist, &ls->base_chunk_count);
}

void load_sys_cleanup(load_system_t *ls)
{
    free(ls->base_chunk_coords);
    for (size_t i = 0; i < SHELL_NUM; i++)
        free(ls->shells[i].crds);
}

render_coords_t load_sys_get_render_coords(load_system_t *ls)
{
    return (render_coords_t) {
        .coords = ls->base_chunk_coords,
        .num = ls->base_chunk_count,
        .offset = ls->curr_pos
    };
}

void load_sys_load_initial(load_system_t *ls, chunk_system_t *cs)
{
    shell_t *shells = _gen_get_shells(ls->curr_pos, ls->load_dist, NULL);

    for (size_t i = 0; i < ls->load_dist; i++)
    {
        shell_t s = shells[i];

        /* Generate all chunks within render distance. */
        for (size_t j = 0; j < s.cnt; j++)
            CS_REQUEST(cs, CSREQ_LOAD, s.crds[j]);

        /* After a shell is generated, we can fully mesh all chunks in the previous shell. */
        if (i > 0) 
        {
            shell_t prev_s = shells[i - 1];
            for (size_t j = 0; j < prev_s.cnt; j++)
                CS_REQUEST(cs, CSREQ_MESH, prev_s.crds[j]);
        }
    }

    ls->shells[SHELL_RIM] = shells[ls->load_dist - 2];
    ls->shells[SHELL_OUT] = shells[ls->load_dist - 1];

    for (size_t i = 0; i < ls->load_dist - 2; i++)
        free(shells[i].crds);
    free(shells);

    CS_REQUEST(cs, CSREQ_INITIAL_LOAD_COMPLETE, 0);
}

static void _get_direction(uint8_t direction, ivec2 start_crd, ivec2 test_crd, 
                           bool *forward, bool *backward)
{
    /* NOTE: North, East, South, West all use >= AND <= for forward and backward. */
    switch (direction) {
    case DIR_NORTH:
        *forward = test_crd.y >= start_crd.y;
        *backward = test_crd.y <= start_crd.y;
        break;
    case DIR_EAST:
        *forward = test_crd.x >= start_crd.x;
        *backward = test_crd.x <= start_crd.x;
        break;
    case DIR_SOUTH:
        *forward = test_crd.y <= start_crd.y;
        *backward = test_crd.y >= start_crd.y;
        break;
    case DIR_WEST:
        *forward = test_crd.x <= start_crd.x;
        *backward = test_crd.x >= start_crd.x;
        break;
    case (DIR_NORTH | DIR_EAST):
        *forward = test_crd.x >= start_crd.x && test_crd.y >= start_crd.y;
        *backward = test_crd.x <= start_crd.x && test_crd.y <= start_crd.y;
        break;
    case (DIR_NORTH | DIR_WEST):
        *forward = test_crd.x <= start_crd.x && test_crd.y >= start_crd.y;
        *backward = test_crd.x >= start_crd.x && test_crd.y <= start_crd.y;
        break;
    case (DIR_SOUTH | DIR_EAST):
        *forward = test_crd.x >= start_crd.x && test_crd.y <= start_crd.y;
        *backward = test_crd.x <= start_crd.x && test_crd.y >= start_crd.y;
        break;
    case (DIR_SOUTH | DIR_WEST):
        *forward = test_crd.x <= start_crd.x && test_crd.y <= start_crd.y;
        *backward = test_crd.x >= start_crd.x && test_crd.y >= start_crd.y;
        break;
    };
}

static void _dispatch_diagonal_loads(load_system_t *ls, chunk_system_t *cs, update_system_t *us, 
                                     ivec2 delta)
{
    INSTRUMENT_FUNC_BEGIN();

    uint8_t direction = 0;
    if (delta.x > 0) direction |= DIR_EAST;
    else if (delta.x < 0) direction |= DIR_WEST;

    if (delta.y > 0) direction |= DIR_NORTH;
    else if (delta.y < 0) direction |= DIR_SOUTH;

    size_t out_cnt = ls->shells[SHELL_OUT].cnt;
    size_t rim_cnt = ls->shells[SHELL_RIM].cnt;
    size_t mesh_after_rim_idx = 0;

    ivec2 new_out[out_cnt];
    ivec2 new_rim[rim_cnt];
    ivec2 mesh_after_rim[ls->load_dist - 1];

    /* Handle outermost shell first as the inner rim depends on them for meshing. */
    for (size_t i = 0; i < out_cnt; i++)
    {
        ivec2 old_crd = ls->shells[SHELL_OUT].crds[i];
        new_out[i] = em_add_ivec2(old_crd, delta);

        bool forward;
        bool backward;
        _get_direction(direction, ls->curr_pos, old_crd, &forward, &backward);

        if (forward) // Load new.
        {
            CS_REQUEST(cs, CSREQ_LOAD, new_out[i]);
            if (old_crd.x != ls->curr_pos.x && old_crd.y != ls->curr_pos.y)
                mesh_after_rim[mesh_after_rim_idx++] = old_crd;
        }

        if (backward) // Unload old.
        {
            CS_REQUEST(cs, CSREQ_UNLOAD, old_crd);
            US_REQUEST(us, USREQ_UNSTAGE, new_out[i], NULL);
        }
    }

    /* Handle inner rim chunks now that outermost shell requests are dispatched. */
    for (size_t i = 0; i < rim_cnt; i++)
    {
        ivec2 old_crd = ls->shells[SHELL_RIM].crds[i];
        new_rim[i] = em_add_ivec2(old_crd, delta);

        bool forward;
        bool backward;
        _get_direction(direction, ls->curr_pos, old_crd, &forward, &backward);

        if (forward) // Mesh new.
        {
            CS_REQUEST(cs, CSREQ_LOAD, new_rim[i]);
            CS_REQUEST(cs, CSREQ_MESH, new_rim[i]);
        }

        if (backward) // Unstage old.
        {
            CS_REQUEST(cs, CSREQ_UNLOAD, old_crd);
            US_REQUEST(us, USREQ_UNSTAGE, old_crd, NULL);
        }
    }

    for (size_t i = 0; i < mesh_after_rim_idx; i++)
        CS_REQUEST(cs, CSREQ_MESH, mesh_after_rim[i]);

    memcpy(&ls->shells[SHELL_OUT].crds[0], &new_out[0], out_cnt * sizeof(ivec2));
    memcpy(&ls->shells[SHELL_RIM].crds[0], &new_rim[0], rim_cnt * sizeof(ivec2));
    ls->curr_pos = em_add_ivec2(ls->curr_pos, delta);

    INSTRUMENT_FUNC_END();
}

static void _dispatch_loads(load_system_t *ls, chunk_system_t *cs, update_system_t *us, 
                            ivec2 delta)
{
    INSTRUMENT_FUNC_BEGIN();

    uint8_t direction = 0;
    if (delta.x > 0) direction = DIR_EAST;
    else if (delta.x < 0) direction = DIR_WEST;
    else if (delta.y > 0) direction = DIR_NORTH;
    else if (delta.y < 0) direction = DIR_SOUTH;

    size_t out_cnt = ls->shells[SHELL_OUT].cnt;
    size_t rim_cnt = ls->shells[SHELL_RIM].cnt;

    ivec2 new_out[out_cnt];
    ivec2 new_rim[rim_cnt];

    /* Handle outermost shell first as the inner rim depends on them for meshing. */
    for (size_t i = 0; i < out_cnt; i++)
    {
        ivec2 old_crd = ls->shells[SHELL_OUT].crds[i];
        new_out[i] = em_add_ivec2(old_crd, delta);

        bool forward;
        bool backward;
        _get_direction(direction, ls->curr_pos, old_crd, &forward, &backward);

        if (forward) // Load new.
            CS_REQUEST(cs, CSREQ_LOAD, new_out[i]);

        if (backward) // Unload old.
            CS_REQUEST(cs, CSREQ_UNLOAD, old_crd);
    }

    /* Handle inner rim chunks now that outermost shell requests are dispatched. */
    for (size_t i = 0; i < rim_cnt; i++)
    {
        ivec2 old_crd = ls->shells[SHELL_RIM].crds[i];
        new_rim[i] = em_add_ivec2(old_crd, delta);

        bool forward;
        bool backward;
        _get_direction(direction, ls->curr_pos, old_crd, &forward, &backward);

        if (forward) // Mesh new.
            CS_REQUEST(cs, CSREQ_MESH, new_rim[i]);

        if (backward) // Unstage old.
            US_REQUEST(us, USREQ_UNSTAGE, old_crd, NULL);
    }

    memcpy(&ls->shells[SHELL_OUT].crds[0], &new_out[0], out_cnt * sizeof(ivec2));
    memcpy(&ls->shells[SHELL_RIM].crds[0], &new_rim[0], rim_cnt * sizeof(ivec2));
    ls->curr_pos = em_add_ivec2(ls->curr_pos, delta);

    INSTRUMENT_FUNC_END();
}

bool load_sys_update(load_system_t *ls, chunk_system_t *cs, update_system_t *us, ivec2 new_pos)
{
    ivec2 delta = em_sub_ivec2(new_pos, ls->curr_pos);
    if (em_equals_ivec2(delta, IVEC2(0, 0)))
        return false;

    if (delta.x == 0 || delta.y == 0)
        _dispatch_loads(ls, cs, us, delta);
    else 
        _dispatch_diagonal_loads(ls, cs, us, delta);

    return true;
}
