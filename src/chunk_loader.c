#include "chunk_loader.h"

void stage_chunk(state_t *state, chunk_t *c, mesh_t *m)
{
    offset_t o = state->free_list->pop_first(state->free_list);

    memcpy(&state->cb.v_stg[o.v_ofst], m->v_buf, m->v_cnt * sizeof(vertex_t));
    memcpy(&state->cb.i_stg[o.i_ofst], m->i_buf, m->i_cnt * sizeof(uint32_t));

    c->offsets = o;
    c->index_cnt = m->i_cnt;
    c->creation_frame = state->frame;
}

void restage_chunk(state_t *state, chunk_t *c, mesh_t *m)
{
    offset_t o = c->offsets;

    memcpy(&state->cb.v_stg[o.v_ofst], m->v_buf, m->v_cnt * sizeof(vertex_t));
    memcpy(&state->cb.i_stg[o.i_ofst], m->i_buf, m->i_cnt * sizeof(uint32_t));

    c->index_cnt = m->i_cnt;
    c->creation_frame = state->frame;
}

void load_initial_chunks(state_t *state)
{
    ivec2_tuple_t coords = gen_get_coords(state->cam.pos, state->cam.rndr_dist);

    ivec2 *inner = coords.arrays[0];
    size_t i_cnt = coords.sizes[0];

    ivec2 *edge = coords.arrays[1];
    size_t e_cnt = coords.sizes[1];

    ivec2 *fringe = coords.arrays[2];
    size_t f_cnt = coords.sizes[2];

    /* Outer chunks are outside of render distance so are ignored here. */

    HASHMAP(ivec2_chunk) *m_hot = state->buckets[BUCKET_HOT].chunks;

    /* Get block information for all of the inner, edge, and fringe chunks for meshing. */
    /* Inner chunks (distance <= render distance - 2). */
    for (size_t i = 0; i < i_cnt; i++) 
    {
        ivec2 crd = inner[i];
        chunk_t *c = gen_new_chunk(crd.x, crd.y);
        m_hot->put_ptr(m_hot, crd, c);
    }

    /* Edge chunks (distance == render distance - 1). */
    for (size_t i = 0; i < e_cnt; i++)
    {
        ivec2 crd = edge[i];
        chunk_t *c = gen_new_chunk(crd.x, crd.y);
        m_hot->put_ptr(m_hot, crd, c);
    }

    /* Fringe chunks (distance == render distance). */
    for (size_t i = 0; i < f_cnt; i++)
    {
        ivec2 crd = fringe[i];
        chunk_t *c = gen_new_chunk(crd.x, crd.y);
        m_hot->put_ptr(m_hot, crd, c);
    }

    /* Mesh all of the chunks using data from adjacent chunks too. */
    /* Inner chunks (distance <= render distance - 2). */
    for (size_t i = 0; i < i_cnt; i++) 
    {
        ivec2 crd = inner[i];
        chunk_t *c = m_hot->get_ptr(m_hot, crd);

        chunk_t *n = m_hot->get_ptr(m_hot, em_add_ivec2(crd, (ivec2) {  0,  16}));
        chunk_t *e = m_hot->get_ptr(m_hot, em_add_ivec2(crd, (ivec2) { 16,   0}));
        chunk_t *s = m_hot->get_ptr(m_hot, em_add_ivec2(crd, (ivec2) {  0, -16}));
        chunk_t *w = m_hot->get_ptr(m_hot, em_add_ivec2(crd, (ivec2) {-16,   0}));

        mesh_t *m = geom_generate_full_mesh(c->blocks, n->blocks, e->blocks, s->blocks, w->blocks);
        stage_chunk(state, c, m);
        c->dirty = false;

        free(m->v_buf);
        free(m->i_buf);
        free(m);
    }

    /* Edge chunks (distance == render distance - 1). */
    for (size_t i = 0; i < e_cnt; i++)
    {
        ivec2 crd = edge[i];
        chunk_t *c = m_hot->get_ptr(m_hot, crd);

        chunk_t *n = m_hot->get_ptr(m_hot, em_add_ivec2(crd, (ivec2) {  0,  16}));
        chunk_t *e = m_hot->get_ptr(m_hot, em_add_ivec2(crd, (ivec2) { 16,   0}));
        chunk_t *s = m_hot->get_ptr(m_hot, em_add_ivec2(crd, (ivec2) {  0, -16}));
        chunk_t *w = m_hot->get_ptr(m_hot, em_add_ivec2(crd, (ivec2) {-16,   0}));

        mesh_t *m = geom_generate_full_mesh(c->blocks, n->blocks, e->blocks, s->blocks, w->blocks);
        stage_chunk(state, c, m);
        c->dirty = false;

        free(m->v_buf);
        free(m->i_buf);
        free(m);
    }

    /* Fringe chunks (distance == render distance). */
    for (size_t i = 0; i < f_cnt; i++)
    {
        ivec2 crd = fringe[i];
        chunk_t *c = m_hot->get_ptr(m_hot, crd);

        /* Fringe chunks border the void so do not have an adjacent chunk on some sides. */
        chunk_t *n = m_hot->get_or_default(m_hot, em_add_ivec2(crd, (ivec2) {  0,  16}), NULL);
        chunk_t *e = m_hot->get_or_default(m_hot, em_add_ivec2(crd, (ivec2) { 16,   0}), NULL);
        chunk_t *s = m_hot->get_or_default(m_hot, em_add_ivec2(crd, (ivec2) {  0, -16}), NULL);
        chunk_t *w = m_hot->get_or_default(m_hot, em_add_ivec2(crd, (ivec2) {-16,   0}), NULL);

        mesh_t *m = geom_generate_full_mesh(c->blocks, 
                                            (n) ? n->blocks : NULL, 
                                            (e) ? e->blocks : NULL,
                                            (s) ? s->blocks : NULL,
                                            (w) ? w->blocks : NULL);
        stage_chunk(state, c, m);
        c->dirty = true;

        free(m->v_buf);
        free(m->i_buf);
        free(m);
    }
}

void load_chunks(state_t *state)
{
    ivec2_tuple_t coords = gen_get_coords(state->cam.pos, state->cam.rndr_dist);

    /* Inner chunks are guaranteed to be loaded and clean so we can ignore them. */

    ivec2 *edge = coords.arrays[1];
    size_t e_cnt = coords.sizes[1];

    ivec2 *fringe = coords.arrays[2];
    size_t f_cnt = coords.sizes[2];

    ivec2 *outer = coords.arrays[3];
    size_t o_cnt = coords.sizes[3];

    HASHMAP(ivec2_chunk) *m_hot = state->buckets[BUCKET_HOT].chunks;
    HASHMAP(ivec2_chunk) *m_cold = state->buckets[BUCKET_COLD].chunks;

    /* Demote outer chunks to cold. */
    for (size_t i = 0; i < o_cnt; i++)
    {
        ivec2 crd = outer[i];
        if (m_hot->contains_key(m_hot, crd))
        {
            chunk_t *c = m_hot->pop_ptr(m_hot, crd);
            m_cold->put_ptr(m_cold, crd, c);
        }
    }

    /* Load any fringe chunks that we don't have loaded yet. */
    for (size_t i = 0; i < f_cnt; i++)
    {
        ivec2 crd = fringe[i];

        /* If a chunk at this coordinate is already active, we can ignore it. */
        if (m_hot->contains_key(m_hot, crd))
            continue;

        /* Cold chunks are already staged but not active, can promote if found. */
        if (m_cold->contains_key(m_cold, crd))
        {
            chunk_t *c = m_cold->pop_ptr(m_cold, crd);
            m_hot->put_ptr(m_hot, crd, c);
            continue;
        }

        /* Did not find a pre-loaded chunk at these coords, generate a new one. */
        chunk_t *c = gen_new_chunk(crd.x, crd.y);
        m_hot->put_ptr(m_hot, crd, c);

        /* Fringe chunks border the void so do not have an adjacent chunk on some sides. */
        chunk_t *n = m_hot->get_or_default(m_hot, em_add_ivec2(crd, (ivec2) {  0,  16}), NULL);
        chunk_t *e = m_hot->get_or_default(m_hot, em_add_ivec2(crd, (ivec2) { 16,   0}), NULL);
        chunk_t *s = m_hot->get_or_default(m_hot, em_add_ivec2(crd, (ivec2) {  0, -16}), NULL);
        chunk_t *w = m_hot->get_or_default(m_hot, em_add_ivec2(crd, (ivec2) {-16,   0}), NULL);

        mesh_t *m = geom_generate_full_mesh(c->blocks, 
                                            (n) ? n->blocks : NULL, 
                                            (e) ? e->blocks : NULL,
                                            (s) ? s->blocks : NULL,
                                            (w) ? w->blocks : NULL);
        stage_chunk(state, c, m);
        c->dirty = true;

        free(m->v_buf);
        free(m->i_buf);
        free(m);
    }

    /* Edge chunks may need a remesh if they are marked as dirty. */
    for (size_t i = 0; i < e_cnt; i++)
    {
        ivec2 crd = edge[i];
        chunk_t *c = m_hot->get_ptr(m_hot, crd);

        /* Clean chunks do not need to be remeshed. */
        if (!c->dirty)
            continue;

        chunk_t *n = m_hot->get_ptr(m_hot, em_add_ivec2(crd, (ivec2) {  0,  16}));
        chunk_t *e = m_hot->get_ptr(m_hot, em_add_ivec2(crd, (ivec2) { 16,   0}));
        chunk_t *s = m_hot->get_ptr(m_hot, em_add_ivec2(crd, (ivec2) {  0, -16}));
        chunk_t *w = m_hot->get_ptr(m_hot, em_add_ivec2(crd, (ivec2) {-16,   0}));

        mesh_t *m = geom_generate_full_mesh(c->blocks, n->blocks, e->blocks, s->blocks, w->blocks);
        restage_chunk(state, c, m); // Chunk is already staged, can reuse its buffer slot.
        c->dirty = false;

        free(m->v_buf);
        free(m->i_buf);
        free(m);
    }
}

void cleanup_old_chunks(state_t *state)
{
    HASHMAP(ivec2_chunk) *m_cold = state->buckets[BUCKET_COLD].chunks;

    pred_c_age_args_t predicate_args = {
        .current_frame = state->frame,
        .max_age = state->buckets[BUCKET_COLD].upper
    };
    em_hashmap_entry_t **rmv = m_cold->filter_get(m_cold, predicate_chunk_age, &predicate_args);

    /* NULL output for filter_get means no entries were removed. */
    if (!rmv)
        return;

    for (size_t i = 0; rmv[i] != NULL; i++)
    {
        offset_t o = ((chunk_t *) rmv[i]->val)->offsets;
        state->free_list->append(state->free_list, o);

        free(rmv[i]->val);
        free(rmv[i]->key);
        free(rmv[i]);
    }

    free(rmv);
}
