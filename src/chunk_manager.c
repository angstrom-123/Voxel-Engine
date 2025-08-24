#include "chunk_manager.h"

mesh_t *_mesh_chunk(HASHMAP(ivec2_chunk) *c_map, ivec2 crd, chunk_data_t *b)
{
    ivec2 crd_n = em_add_ivec2(crd, (ivec2) {  0,  16});
    ivec2 crd_e = em_add_ivec2(crd, (ivec2) { 16,   0});
    ivec2 crd_s = em_add_ivec2(crd, (ivec2) {  0, -16});
    ivec2 crd_w = em_add_ivec2(crd, (ivec2) {-16,   0});

    chunk_t *c_n = c_map->get_or_default(c_map, crd_n, NULL);
    chunk_t *c_e = c_map->get_or_default(c_map, crd_e, NULL);
    chunk_t *c_s = c_map->get_or_default(c_map, crd_s, NULL);
    chunk_t *c_w = c_map->get_or_default(c_map, crd_w, NULL);

    chunk_data_t *n_d = c_n ? c_n->blocks : NULL;
    chunk_data_t *e_d = c_e ? c_e->blocks : NULL;
    chunk_data_t *s_d = c_s ? c_s->blocks : NULL;
    chunk_data_t *w_d = c_w ? c_w->blocks : NULL;

    return geom_generate_full_mesh(b, n_d, e_d, s_d, w_d);
}

void _restage_chunk(state_t *s, chunk_t *c, mesh_t *m) 
{
    memcpy(&s->cb.v_stg[c->offsets.v_ofst], m->v_buf, m->v_cnt * sizeof(vertex_t));
    memcpy(&s->cb.i_stg[c->offsets.i_ofst], m->i_buf, m->i_cnt * sizeof(uint32_t));

    c->creation_frame = s->frame;
    c->index_cnt = m->i_cnt;

    free(m->v_buf);
    free(m->i_buf);
    free(m);
}

void _stage_chunk(state_t *s, chunk_t *c, mesh_t *m)
{
    offset_t o = s->free_list->pop_first(s->free_list);

    memcpy(&s->cb.v_stg[o.v_ofst], m->v_buf, m->v_cnt * sizeof(vertex_t));
    memcpy(&s->cb.i_stg[o.i_ofst], m->i_buf, m->i_cnt * sizeof(uint32_t));

    c->offsets = o;
    c->creation_frame = s->frame;
    c->index_cnt = m->i_cnt;

    free(m->v_buf);
    free(m->i_buf);
    free(m);
}

void manager_generate_chunks_init(state_t *s)
{
    ivec2_tuple_t coords = gen_get_coords(s->cam.pos, s->cam.rndr_dist);

    HASHMAP(ivec2_chunk) *m_hot = s->buckets[BUCKET_HOT].chunks;

    ivec2 *inner = coords.arrays[0];
    ivec2 *edge = coords.arrays[1];
    ivec2 *fringe = coords.arrays[2];

    size_t i_len = coords.sizes[0];
    size_t e_len = coords.sizes[1];
    size_t f_len = coords.sizes[2];

    /* Generate all chunks within render distance (inner, edge, fringe). */
    for (size_t i = 0; i < i_len; i++)
    {
        ivec2 crd = inner[i];
        chunk_t *c = gen_new_chunk(crd.x, crd.y);
        m_hot->put_ptr(m_hot, crd, c);
    }

    for (size_t i = 0; i < e_len; i++)
    {
        ivec2 crd = edge[i];
        chunk_t *c = gen_new_chunk(crd.x, crd.y);
        m_hot->put_ptr(m_hot, crd, c);
    }

    for (size_t i = 0; i < f_len; i++)
    {
        ivec2 crd = fringe[i];
        chunk_t *c = gen_new_chunk(crd.x, crd.y);
        m_hot->put_ptr(m_hot, crd, c);
    }

    /* Use complete chunk data to generate full meshes for each chunk. */
    for (size_t i = 0; i < i_len; i++)
    {
        ivec2 crd = inner[i];
        chunk_t *c = m_hot->get_ptr(m_hot, crd);

        mesh_t *m = _mesh_chunk(m_hot, crd, c->blocks);
        c->full_mesh = true;
        _stage_chunk(s, c, m);
    }

    for (size_t i = 0; i < e_len; i++)
    {
        ivec2 crd = edge[i];
        chunk_t *c = m_hot->get_ptr(m_hot, crd);

        mesh_t *m = _mesh_chunk(m_hot, crd, c->blocks);
        c->full_mesh = true;
        _stage_chunk(s, c, m);
    }

    /* Fringe chunks border void so are not fully meshed yet. */
    for (size_t i = 0; i < f_len; i++)
    {
        ivec2 crd = fringe[i];
        chunk_t *c = m_hot->get_ptr(m_hot, crd);
        
        mesh_t *m = _mesh_chunk(m_hot, crd, c->blocks);
        c->full_mesh = false;
        _stage_chunk(s, c, m);
    }
}

void manager_generate_chunks(state_t *s)
{
    HASHMAP(ivec2_chunk) *m_hot = s->buckets[BUCKET_HOT].chunks;
    HASHMAP(ivec2_chunk) *m_cold = s->buckets[BUCKET_COLD].chunks;

    ivec2_tuple_t coords = gen_get_coords(s->cam.pos, s->cam.rndr_dist);

    ivec2 *edge = coords.arrays[1];
    ivec2 *fringe = coords.arrays[2];
    ivec2 *outer = coords.arrays[3];

    size_t e_len = coords.sizes[1];
    size_t f_len = coords.sizes[2];
    size_t o_len = coords.sizes[3];

    for (size_t i = 0; i < o_len; i++)
    {
        ivec2 crd = outer[i];

        if (m_hot->contains_key(m_hot, crd))
        {
            chunk_t *c = m_hot->pop_ptr(m_hot, crd);
            m_cold->put_ptr(m_cold, crd, c);
        }
    }

    for (size_t i = 0; i < f_len; i++)
    {
        ivec2 crd = fringe[i];

        if (!m_hot->contains_key(m_hot, crd))
        {
            chunk_t *c;

            if (m_cold->contains_key(m_cold, crd))
            {
                c = m_cold->pop_ptr(m_cold, crd);
            }
            else 
            {
                c = gen_new_chunk(crd.x, crd.y);
                mesh_t *m = _mesh_chunk(m_hot, crd, c->blocks);
                _stage_chunk(s, c, m);

                c->full_mesh = false;
            }

            m_hot->put_ptr(m_hot, crd, c);
        }
    }

    for (size_t i = 0; i < e_len; i++)
    {
        ivec2 crd = edge[i];

        chunk_t *c = m_hot->get_ptr(m_hot, crd);
        if (!c->full_mesh)
        {
            mesh_t *m = _mesh_chunk(m_hot, crd, c->blocks);
            _restage_chunk(s, c, m);

            c->full_mesh = true;
        }
    }
}

void manager_demote_chunks(state_t *s)
{
    bucket_t cold = s->buckets[BUCKET_COLD];
    bucket_t stale = s->buckets[BUCKET_STALE];
    pred_c_age_args_t args = {
        .current_frame = s->frame,
        .max_age = cold.upper
    };
    em_hashmap_entry_t **rmv = cold.chunks->filter_get(cold.chunks, predicate_chunk_age, &args);
    if (rmv)
    {
        em_hashmap_entry_t *e;
        size_t idx = 0;
        while ((e = rmv[idx++]) != NULL)
            stale.chunks->put_ptr(stale.chunks, *(ivec2 *) e->key, e->val);

        free(rmv);
    }
}

void manager_cleanup_chunks(state_t *s)
{
    bucket_t stale = s->buckets[BUCKET_STALE];
    pred_c_age_args_t args = {
        .current_frame = s->frame,
        .max_age = stale.upper
    };
    em_hashmap_entry_t **rmv = stale.chunks->filter_get(stale.chunks, predicate_chunk_age, &args);
    if (rmv != NULL)
    {
        for (size_t i = 0; rmv[i]; i++)
        {
            em_hashmap_entry_t *e = rmv[i];
            chunk_t *c = e->val;
            s->free_list->append(s->free_list, c->offsets);

            geom_destroy_chunk(c);
            free(e->key);
            free(e);
        }

        free(rmv);
    }
}
