#include "load_system.h"
#include "chunk_system.h"
#include "constants.h"

void load_sys_init(load_system_t *ls, const load_system_desc_t *desc)
{
    ls->curr_pos = desc->start_pos;
    ls->load_dist = desc->render_dist + 1;
    ls->base_chunk_coords = gen_get_coords((ivec2) {0, 0}, desc->render_dist, &ls->base_chunk_count);
}

void load_sys_cleanup(load_system_t *lm)
{
    (void) lm;
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
    shell_t *shells = gen_get_shells(ls->curr_pos, ls->load_dist, NULL);

    for (size_t i = 0; i < ls->load_dist; i++)
    {
        shell_t s = shells[i];

        /* Generate all chunks within render distance. */
        for (size_t j = 0; j < s.cnt; j++)
            CS_REQUEST(cs, CSREQ_GEN, s.crds[j]);

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

    free(shells);
}

static void _get_direction(int8_t direction, ivec2 start_crd, ivec2 test_crd, 
                           bool *forward, bool *backward)
{
    /* NOTE: direction checks are both inclusive of equality for this to work. */
    switch (direction) {
    case NORTH:
        *forward = test_crd.y >= start_crd.y;
        *backward = test_crd.y <= start_crd.y;
        break;
    case EAST:
        *forward = test_crd.x >= start_crd.x;
        *backward = test_crd.x <= start_crd.x;
        break;
    case SOUTH:
        *forward = test_crd.y <= start_crd.y;
        *backward = test_crd.y >= start_crd.y;
        break;
    case WEST:
        *forward = test_crd.x <= start_crd.x;
        *backward = test_crd.x >= start_crd.x;
        break;
    };
}

static void _dispatch_loads(load_system_t *ls, chunk_system_t *cs, update_system_t *us, 
                            ivec2 delta)
{
    INSTRUMENT_FUNC_BEGIN();

    int8_t direction = 0;
    if (delta.x > 0) direction      = EAST;
    else if (delta.x < 0) direction = WEST;
    else if (delta.y > 0) direction = NORTH;
    else if (delta.y < 0) direction = SOUTH;

    ENGINE_LOG_OK("Direction %hhi\n", direction);

    size_t out_cnt = ls->shells[SHELL_OUT].cnt;
    size_t rim_cnt = ls->shells[SHELL_RIM].cnt;

    ivec2 *new_out = malloc(out_cnt * sizeof(ivec2));
    ivec2 *new_rim = malloc(rim_cnt * sizeof(ivec2));

    /* Handle outermost shell first as the inner rim depends on them for meshing. */
    for (size_t i = 0; i < out_cnt; i++)
    {
        ivec2 old_crd = ls->shells[SHELL_OUT].crds[i];
        new_out[i] = em_add_ivec2(old_crd, delta);

        bool forward;
        bool backward;

        _get_direction(direction, ls->curr_pos, old_crd, &forward, &backward);

        if (forward) // Load new.
            CS_REQUEST(cs, CSREQ_GEN, new_out[i]);

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

    free(ls->shells[SHELL_OUT].crds);
    free(ls->shells[SHELL_RIM].crds);

    ls->shells[SHELL_OUT].crds = new_out;
    ls->shells[SHELL_RIM].crds = new_rim;
    ls->curr_pos = em_add_ivec2(ls->curr_pos, delta);

    INSTRUMENT_FUNC_END();
}

bool load_sys_update(load_system_t *ls, chunk_system_t *cs, update_system_t *us, ivec2 new_pos)
{
    ivec2 delta = em_sub_ivec2(new_pos, ls->curr_pos);
    if (em_equals_ivec2(delta, (ivec2) {0, 0}))
        return false;

    if (em_abs(delta.x) > 0 && em_abs(delta.y) > 0)
    {
        ENGINE_LOG_ERROR("Diagonal move needs fix.\n", NULL);
        // exit(1);
        _dispatch_loads(ls, cs, us, (ivec2) {delta.x, 0});
        _dispatch_loads(ls, cs, us, (ivec2) {0, delta.y});
    }
    else 
    {
        _dispatch_loads(ls, cs, us, delta);
    }

    return true;
}
