#include "load_system.h"
#include "world_gen.h"

void load_sys_init(load_system_t *ls, const load_system_desc_t *desc)
{
    ls->curr_pos = desc->start_pos;
    ls->load_dist = desc->render_dist + 2;
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
        {
            // ENGINE_LOG_OK("Generating %i %i\n", s.crds[j].x, s.crds[j].y);
            chunk_sys_make_request(cs, (cs_request_t) {
                .type = CSREQ_GEN,
                .pos = s.crds[j]
            });
        }

        /* After a shell is generated, we can fully mesh all chunks in the previous shell. */
        if (i > 0) 
        {
            shell_t prev_s = shells[i - 1];
            for (size_t j = 0; j < prev_s.cnt; j++)
            {
                // ENGINE_LOG_OK("Meshing %i %i\n", prev_s.crds[j].x, prev_s.crds[j].y);
                chunk_sys_make_request(cs, (cs_request_t) {
                    .type = CSREQ_MESH,
                    .pos = prev_s.crds[j]
                });
            }
        }
    }

    ls->shells[SHELL_RIM] = shells[ls->load_dist - 2];
    ls->shells[SHELL_OUT] = shells[ls->load_dist - 1];

    free(shells);
}

bool load_sys_update(load_system_t *ls, chunk_system_t *cs, ivec2 new_pos)
{
    ivec2 delta = em_sub_ivec2(new_pos, ls->curr_pos);
    if (em_equals_ivec2(delta, (ivec2) {0, 0}))
        return false;

    size_t out_cnt = ls->shells[SHELL_OUT].cnt;
    size_t rim_cnt = ls->shells[SHELL_RIM].cnt;

    ivec2 *new_out_crds = malloc(out_cnt * sizeof(ivec2));
    ivec2 *new_rim_crds = malloc(rim_cnt * sizeof(ivec2));

    /* 
     * Outermost chunks are either loaded or unloaded. Need to be handled before 
     * rim chunks as their data will be needed for meshing. 
     */
    for (size_t i = 0; i < out_cnt; i++)
    {
        ivec2 old_out_crd = ls->shells[SHELL_OUT].crds[i];
        new_out_crds[i] = em_add_ivec2(old_out_crd, delta);

        ivec2 old_diff = em_sub_ivec2(old_out_crd, ls->curr_pos);
        ivec2 new_diff = em_sub_ivec2(new_out_crds[i], ls->curr_pos);

        /* If the distance is increasing (new distance greater than old). */
        if (em_abs(old_diff.x) + em_abs(old_diff.y) < 
            em_abs(new_diff.x) + em_abs(new_diff.y))
        {
            // ENGINE_LOG_OK("Genning %i %i\n", new_out_crds[i].x, new_out_crds[i].y);
            chunk_sys_make_request(cs, (cs_request_t) {
                .type = CSREQ_GEN,
                .pos = new_out_crds[i]
            });
        }
        else 
        {
            // TODO
            // ENGINE_LOG_OK("Unloading %i %i\n", old_out_crd.x, old_out_crd.y);
            // if (cs->front->chunks->contains_key(cs->front->chunks, old_out_crd))
            //     cs->front->chunks->remove(cs->front->chunks, old_out_crd);
            // cs_make_request(cs->back, (cs_request_t) {
            //     .type = CSREQ_UNLOAD,
            //     .pos = old_out_crd
            // });
        }
    }

    /* Chunks at the rim are either meshed if new, or made invisible if old. */
    for (size_t i = 0; i < rim_cnt; i++)
    {
        ivec2 old_rim_crd = ls->shells[SHELL_RIM].crds[i];
        new_rim_crds[i] = em_add_ivec2(old_rim_crd, delta);

        ivec2 old_diff = em_sub_ivec2(old_rim_crd, ls->curr_pos);
        ivec2 new_diff = em_sub_ivec2(new_rim_crds[i], ls->curr_pos);

        /* If the distance is increasing (new distance greater than old). */
        if (em_abs(old_diff.x) + em_abs(old_diff.y) < 
            em_abs(new_diff.x) + em_abs(new_diff.y))
        {
            // ENGINE_LOG_OK("Meshing %i %i\n", new_rim_crds[i].x, new_rim_crds[i].y);
            chunk_sys_make_request(cs, (cs_request_t) {
                .type = CSREQ_MESH,
                .pos = new_rim_crds[i]
            });
        }
        else 
        {
            // TODO
            // LOG_OK("MANAGER", "Invising %i %i\n", old_rim_crd.x, old_rim_crd.y);
            // chunk_render_info_t *cri = cs->front->chunks->get_ptr(cs->front->chunks, 
            //                                                       old_rim_crd);
            // cri->visible = false;
        }
    }


    free(ls->shells[SHELL_OUT].crds);
    free(ls->shells[SHELL_RIM].crds);

    ls->shells[SHELL_OUT].crds = new_out_crds;
    ls->shells[SHELL_RIM].crds = new_rim_crds;
    ls->curr_pos = new_pos;

    return true;
}
