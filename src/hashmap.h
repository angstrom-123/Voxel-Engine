#ifndef HASHMAP_H
#define HASHMAP_H 

#include <libem/em_hashmap.h>
#include <libem/em_math.h> // ivec2

#include "geometry_types.h" // chunk_t

/* Declare new hashmap struct. */
DECLARE_HASHMAP(ivec2, chunk_t, ivec2_chunk)
DECLARE_HASHMAP_CMP(ivec2, ivec2)
DECLARE_HASHMAP_HSH(ivec2, ivec2)

/* Provide function definitions in ONE source file. */
#ifdef MY_HASHMAP_IMPL

DEFINE_HASHMAP(ivec2, chunk_t, ivec2_chunk)
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

#endif // MY_HASHMAP_IMPL

#endif
