#ifndef AABB_H
#define AABB_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <libem/em_math.h>

typedef struct interval {
    float min;
    float max;
} interval_t;

typedef union aabb {
    struct {
        interval_t x, y, z;
    };
    interval_t axis[3];
} aabb_t;

extern bool aabb_intersecting(aabb_t a, aabb_t b);
extern aabb_t aabb_from_voxel_coord(ivec3 coord);
extern interval_t interval_around(float val, float size);

#endif
