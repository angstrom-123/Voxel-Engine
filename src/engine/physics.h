#ifndef PHYSICS_H
#define PHYSICS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <math.h>

#include <libem/em_math.h>

#include "logger.h"

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

typedef struct ray {
    vec3 origin;
    vec3 direction;
    float length;
} ray_t;

extern bool aabb_ray_intersecting(aabb_t box, ray_t ray);
extern bool aabb_intersecting_depth(aabb_t box_1, aabb_t box_2, vec3 *axis, float *depth);
extern bool aabb_intersecting(aabb_t box_1, aabb_t box_2);
extern aabb_t aabb_from_voxel_coord(ivec3 coord);
extern aabb_t aabb_with_offset(aabb_t box, vec3 offset);
extern float interval_size(interval_t interval);
extern interval_t interval_around(float value, float size);
extern bool interval_contains(interval_t interval, float value);

#endif
