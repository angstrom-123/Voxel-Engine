#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#ifdef DATA_STRUCTURES_IMPL
    #define EM_ARRAY_LIST_IMPL
    #define EM_DOUBLE_LIST_IMPL
    #define EM_CIRCULAR_QUEUE_IMPL
    #define EM_HASHMAP_IMPL
#endif 

#include <libem/em_array_list.h>
#include <libem/em_double_list.h>
#include <libem/em_circular_queue.h>
#include <libem/em_hashmap.h>

/* Types for the datastructures. */
#include <libem/em_math.h>
#include "update_system_types.h"
#include "chunk_system_types.h"

/* Array List. */

/* Doubly Linked List. */
DECLARE_DOUBLE_LIST(ivec2, ivec2)
DECLARE_DOUBLE_LIST_CLN(ivec2, ivec2)

/* Circular Queue. */
DECLARE_CIRCULAR_QUEUE(cs_request_t, cs_request)
DECLARE_CIRCULAR_QUEUE_CLN(cs_request_t, cs_request)

DECLARE_CIRCULAR_QUEUE(us_request_t, us_request)
DECLARE_CIRCULAR_QUEUE_CLN(us_request_t, us_request)

DECLARE_CIRCULAR_QUEUE(offset_t, offset)
DECLARE_CIRCULAR_QUEUE_CLN(offset_t, offset)

DECLARE_CIRCULAR_QUEUE(buffer_pair_t, sokol_buffers);
DECLARE_CIRCULAR_QUEUE_CLN(buffer_pair_t, sokol_buffers);

/* Hashmap. */
DECLARE_HASHMAP(ivec2, chunk_render_info_t, ivec2_cri);
DECLARE_HASHMAP_CLN_V(chunk_render_info_t, cri);

DECLARE_HASHMAP(ivec2, chunk_data_t, ivec2_chunk_data);
DECLARE_HASHMAP_CLN_V(chunk_data_t, chunk_data);

DECLARE_HASHMAP_CLN_K(ivec2, ivec2);
DECLARE_HASHMAP_CMP(ivec2, ivec2);
DECLARE_HASHMAP_HSH(ivec2, ivec2);

#ifdef DATA_STRUCTURES_IMPL

/* Array List. */

/* Doubly Linked List. */
DEFINE_DOUBLE_LIST(ivec2, ivec2)
DEFINE_DOUBLE_LIST_CLN(ivec2, ivec2)
{
    free(val);
}

/* Circular Queue. */
DEFINE_CIRCULAR_QUEUE(us_request_t, us_request)
DEFINE_CIRCULAR_QUEUE_CLN(us_request_t, us_request)
{
    free(val);
}

DEFINE_CIRCULAR_QUEUE(cs_request_t, cs_request)
DEFINE_CIRCULAR_QUEUE_CLN(cs_request_t, cs_request)
{
    free(val);
}

DEFINE_CIRCULAR_QUEUE(offset_t, offset)
DEFINE_CIRCULAR_QUEUE_CLN(offset_t, offset)
{
    free(val);
}

DEFINE_CIRCULAR_QUEUE(buffer_pair_t, sokol_buffers)
DEFINE_CIRCULAR_QUEUE_CLN(buffer_pair_t, sokol_buffers)
{
    sg_destroy_buffer(val->vertex);
    sg_destroy_buffer(val->index);
    free(val);
}

/* Hashmap. */
DEFINE_HASHMAP(ivec2, chunk_render_info_t, ivec2_cri)
DEFINE_HASHMAP(ivec2, chunk_data_t, ivec2_chunk_data)
DEFINE_HASHMAP_CMP(ivec2, ivec2)
{
    return lhs->x == rhs->x && lhs->y == rhs->y;
}
DEFINE_HASHMAP_HSH(ivec2, ivec2)
{
    /* Combine x and y components into one value. */
    int32_t x;
    if (key->x >= 0) x = 2 * key->x;
    else x = -2 * key->x - 1;

    int32_t y;
    if (key->y >= 0) y = 2 * key->y;
    else y = -2 * key->y - 1;

    int32_t pair = (x >= y) 
        ? em_sqr(x) + y 
        : em_sqr(y) + x;

    return em_hash(pair);
}
DEFINE_HASHMAP_CLN_K(ivec2, ivec2)
{
    free(key);
}
DEFINE_HASHMAP_CLN_V(chunk_render_info_t, cri)
{
    free(val);
}
DEFINE_HASHMAP_CLN_V(chunk_data_t, chunk_data)
{
    free(val);
}

#endif // DATA_STRUCTURES_IMPL

#endif
