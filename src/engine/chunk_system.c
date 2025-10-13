#include "chunk_system.h"

static void _handle_request(chunk_system_t *cs, cs_request_t *r)
{
    switch (r->type) {
    case CSREQ_GEN:
    {
        chunk_data_t *data = gen_generate_chunk_data(r->pos, cs->seed);
        ENGINE_LOG_OK("Adding %i %i inner map.\n", r->pos.x, r->pos.y);
        cs->genned->put_ptr(cs->genned, r->pos, data);

        break;
    }
    case CSREQ_MESH:
    {
        chunk_data_t *c = cs->genned->get_or_default(cs->genned, r->pos, NULL);
        chunk_data_t *n = cs->genned->get_or_default(cs->genned, REL_N(r->pos), NULL);
        chunk_data_t *e = cs->genned->get_or_default(cs->genned, REL_E(r->pos), NULL);
        chunk_data_t *s = cs->genned->get_or_default(cs->genned, REL_S(r->pos), NULL);
        chunk_data_t *w = cs->genned->get_or_default(cs->genned, REL_W(r->pos), NULL);

        if (c && n && e && s && w)
        {
            mesh_t *mesh = geom_generate_full_mesh(c, n, e, s, w);

            mtx_lock(&cs->accumulator_lock);

            ENGINE_LOG_OK("Adding mesh to accumulator at %i %i.\n", r->pos.x, r->pos.y);
            cs->accumulator->enqueue(cs->accumulator, (cs_result_t) {
                .pos = r->pos,
                .mesh = mesh
            });

            mtx_unlock(&cs->accumulator_lock);
        }
        else 
        {
            ENGINE_LOG_ERROR("Failed to mesh chunk at %i %i because required chunks are not genned.\n",
                      r->pos.x, r->pos.y);
            if (!c) 
                ENGINE_LOG_ERROR("Found current: %i %i\n", 
                          r->pos.x, r->pos.y);
            if (!n) 
                ENGINE_LOG_ERROR("Not found north: %i %i\n", 
                          r->pos.x, r->pos.y + CHUNK_SIZE);
            if (!e) 
                ENGINE_LOG_ERROR("Not found east: %i %i\n", 
                          r->pos.x + CHUNK_SIZE, r->pos.y);
            if (!s) 
                ENGINE_LOG_ERROR("Not found south: %i %i\n", 
                          r->pos.x, r->pos.y - CHUNK_SIZE);
            if (!w) 
                ENGINE_LOG_ERROR("Not found west: %i %i\n", 
                          r->pos.x - CHUNK_SIZE, r->pos.y);
            exit(1);
        }

        break;
    }
    case CSREQ_UNLOAD:
        cs->genned->remove(cs->genned, r->pos);
        break;
    }
}

int _thread_func(void *args)
{
    chunk_system_t *cs = args;
    cnd_signal(&cs->thread_initialized);
    ENGINE_LOG_OK("Thread init.\n", NULL);

    const size_t BATCH_CNT = 100000;

    while (atomic_load(&cs->running))
    {
        mtx_lock(&cs->requests_lock);

        cnd_wait(&cs->needs_update, &cs->requests_lock);

        size_t handled_cnt = 0;
        while (cs->requests->count > 0 && handled_cnt++ < BATCH_CNT)
        {
            cs_request_t *r = cs->requests->dequeue_ptr(cs->requests);
            _handle_request(cs, r);
        }

        mtx_unlock(&cs->requests_lock);
    }

    ENGINE_LOG_WARN("Thread terminating.\n", NULL);

    return 0;
}

// chunk_system_front_t *_get_front(const chunk_system_desc_t *desc)
// {
//     chunk_system_front_t *res = malloc(sizeof(chunk_system_front_t));
//
//     /* Size of stages and buffers for vertex and index data. */
//     const size_t V_SIZE = desc->free_capacity * V_MAX * sizeof(vertex_t);
//     const size_t I_SIZE = desc->free_capacity * I_MAX * sizeof(uint32_t);
//
//     res->chunks = HASHMAP_NEW(ivec2_render_chunk)(&(em_hashmap_desc_t) {
//         .capacity = desc->chunk_capacity,
//         .cmp_func = (void_cmp_func) HASHMAP_CMP(ivec2),
//         .hsh_func = (void_hsh_func) HASHMAP_HSH(ivec2),
//         .cln_k_func = (void_cln_func) HASHMAP_CLN_K(ivec2),
//         .cln_v_func = (void_cln_func) HASHMAP_CLN_V(front_chunk),
//         .flags = EM_FLAG_NO_RESIZE
//     });
//
//     res->visible = DOUBLE_LIST_NEW(ivec2)(&(em_double_list_desc_t) {
//         .cln_func = (void_cln_func) DOUBLE_LIST_CLN(ivec2),
//         .flags = EM_FLAG_NONE
//     });
//
//     /* Initializing list of free offsets to include all possible offsets. */
//     res->free = CIRCULAR_QUEUE_NEW(offset)(&(em_circular_queue_desc_t) {
//         .capacity = desc->free_capacity,
//         .cln_func = (void_cln_func) CIRCULAR_QUEUE_CLN(offset),
//         .flags = EM_FLAG_NO_RESIZE
//     });
//     for (size_t i = 0; i < desc->free_capacity; i++)
//     {
//         res->free->enqueue(res->free, (offset_t) {
//             .v_ofst = V_MAX * i,
//             .i_ofst = I_MAX * i
//         });
//     }
//
//     res->buffers = (cs_buffers_t) {
//         .v_size = V_SIZE,
//         .v_stage = malloc(V_SIZE),
//         .vbo = sg_make_buffer(&(sg_buffer_desc) {
//             .size = V_SIZE,
//             .usage = {
//                 .vertex_buffer = true,
//                 .dynamic_update = true
//             },
//             .label = "Vertex Buffer"
//         }),
//         .i_size = I_SIZE,
//         .i_stage = malloc(I_SIZE),
//         .ibo = sg_make_buffer(&(sg_buffer_desc) {
//             .size = I_SIZE,
//             .usage = {
//                 .index_buffer = true,
//                 .dynamic_update = true
//             },
//             .label = "Index Buffer"
//         })
//     };
//
//     return res;
// }
//
void cs_init(chunk_system_t *cs, const chunk_system_desc_t *desc)
{
    cs->seed = desc->seed;
    cs->running = true;

    cs->genned = HASHMAP_NEW(ivec2_chunk_data)(&(em_hashmap_desc_t) {
        .capacity = desc->chunk_capacity,
        .cmp_func = (void_cmp_func) HASHMAP_CMP(ivec2),
        .hsh_func = (void_hsh_func) HASHMAP_HSH(ivec2),
        .cln_k_func = (void_cln_func) HASHMAP_CLN_K(ivec2),
        .cln_v_func = (void_cln_func) HASHMAP_CLN_V(chunk_data),
        .flags = EM_FLAG_NO_RESIZE
    });

    cs->accumulator = CIRCULAR_QUEUE_NEW(cs_result)(&(em_circular_queue_desc_t) {
        .capacity = desc->accumulator_capacity,
        .cln_func = (void_cln_func) CIRCULAR_QUEUE_CLN(cs_result),
        .flags = EM_FLAG_NONE // Resizable.
    });

    cs->requests = CIRCULAR_QUEUE_NEW(cs_request)(&(em_circular_queue_desc_t) {
        .capacity = desc->request_capacity,
        .cln_func = (void_cln_func) CIRCULAR_QUEUE_CLN(cs_request),
        .flags = EM_FLAG_NONE // Resizable.
    });

    mtx_init(&cs->requests_lock, mtx_plain);
    mtx_init(&cs->accumulator_lock, mtx_plain);
    mtx_init(&cs->init_lock, mtx_plain);
    cnd_init(&cs->needs_update);
    cnd_init(&cs->thread_initialized);

    int res = thrd_create(&cs->worker, _thread_func, cs);
    if (res != thrd_success)
    {
        ENGINE_LOG_ERROR("Failed to initialize worker thread. Aborting.\n", NULL);
        exit(1);
    }
}

void cs_cleanup(chunk_system_t *cs)
{
    atomic_store(&cs->running, false);
    thrd_join(cs->worker, NULL);

    cs->genned->destroy(cs->genned);
    cs->accumulator->destroy(cs->accumulator);
    cs->requests->destroy(cs->requests);

    cnd_destroy(&cs->needs_update);
    mtx_destroy(&cs->requests_lock);
    mtx_destroy(&cs->accumulator_lock);
}

void cs_make_request(chunk_system_t *cs, cs_request_t r)
{
    mtx_lock(&cs->requests_lock);

    cs->requests->enqueue(cs->requests, r);
    cnd_signal(&cs->needs_update);

    mtx_unlock(&cs->requests_lock);
}

// bool cs_pull_n_changes(chunk_system_front_t *csf, chunk_system_back_t *csb, size_t cnt)
// {
//     mtx_lock(&csb->accumulator_lock);
//     for (size_t i = 0; i < cnt; i++)
//     {
//         if (csb->accumulator->count <= 0)
//         {
//             mtx_unlock(&csb->accumulator_lock);
//             return i > 0;
//         }
//
//         cs_result_t *r = csb->accumulator->dequeue_ptr(csb->accumulator);
//
//         chunk_render_info_t *cri = malloc(sizeof(chunk_render_info_t));
//         cri->pos = r->pos;
//         cri->offset = csf->free->dequeue(csf->free);
//         cri->index_cnt = r->mesh->i_cnt;
//         cri->visible = true;
//
//         csf->visible->append(csf->visible, r->pos);
//         csf->chunks->put_ptr(csf->chunks, r->pos, cri);
//
//         if (!r->mesh)
//         {
//             LOG_ERROR("CHUNK SYSTEM", "No mesh in pull changes.\n", NULL);
//             exit(1);
//         }
//         memcpy(&csf->buffers.v_stage[cri->offset.v_ofst], &r->mesh->v_buf[0], 
//                r->mesh->v_cnt * sizeof(vertex_t));
//         memcpy(&csf->buffers.i_stage[cri->offset.i_ofst], &r->mesh->i_buf[0], 
//                r->mesh->i_cnt * sizeof(uint32_t));
//
//         free(r->mesh->v_buf);
//         free(r->mesh->i_buf);
//         free(r->mesh);
//         free(r);
//     }
//     mtx_unlock(&csb->accumulator_lock);
//
//     return true;
// }
//
// void cs_upload_stages(chunk_system_front_t *csf)
// {
//     sg_update_buffer(csf->buffers.vbo, &(sg_range) {
//         .ptr = csf->buffers.v_stage,
//         .size = csf->buffers.v_size
//     });
//
//     sg_update_buffer(csf->buffers.ibo, &(sg_range) {
//         .ptr = csf->buffers.i_stage,
//         .size = csf->buffers.i_size
//     });
// }
