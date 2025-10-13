#ifndef HASHMAP_H
#define HASHMAP_H 

#include <libem/em_hashmap.h>
#include <libem/em_math.h> // ivec2

#include "geometry_types.h" // chunk_t
#include "chunk_system_types.h"

DECLARE_HASHMAP(ivec2, chunk_render_info_t, ivec2_render_chunk)
DECLARE_HASHMAP(ivec2, chunk_data_t, ivec2_chunk_data)
DECLARE_HASHMAP_CMP(ivec2, ivec2)
DECLARE_HASHMAP_HSH(ivec2, ivec2)
DECLARE_HASHMAP_CLN_K(ivec2, ivec2)
DECLARE_HASHMAP_CLN_V(chunk_render_info_t, front_chunk)
DECLARE_HASHMAP_CLN_V(chunk_data_t, chunk_data)

#ifdef MY_HASHMAP_IMPL

DEFINE_HASHMAP(ivec2, chunk_render_info_t, ivec2_render_chunk)
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
DEFINE_HASHMAP_CLN_V(chunk_render_info_t, front_chunk)
{
    free(val);
}
DEFINE_HASHMAP_CLN_V(chunk_data_t, chunk_data)
{
    free(val);
}

#endif // MY_HASHMAP_IMPL

#endif
