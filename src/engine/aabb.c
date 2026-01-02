#include "aabb.h"

bool aabb_intersecting(aabb_t a, aabb_t b)
{
    return a.x.min <= b.x.max && a.x.max >= b.x.min &&
           a.y.min <= b.y.max && a.y.max >= b.y.min &&
           a.z.min <= b.z.max && a.z.max >= b.z.min;
}

aabb_t aabb_from_voxel_coord(ivec3 coord)
{
    return (aabb_t) {
        .x = { coord.x, coord.x + 1.0 },
        .y = { coord.y, coord.y + 1.0 },
        .z = { coord.z, coord.z + 1.0 }
    };
}

interval_t interval_around(float val, float size)
{
    return (interval_t) { val - size / 2.0, val + size / 2.0 };
}
