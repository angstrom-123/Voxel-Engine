#ifndef PHYSICS_H
#define PHYSICS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <math.h>

#include <libem/em_math.h>

#include "logger.h"

typedef union aabb {
    struct {
        itvl x, y, z;
    };
    itvl axis[3];
} aabb_t;

typedef struct ray {
    vec3 origin;
    vec3 direction;
    float length;
} ray_t;

extern bool aabb_ray_intersecting(aabb_t box, ray_t ray);
extern bool aabb_intersecting_depth(aabb_t box_1, aabb_t box_2, vec3 *axis, float *depth);
extern bool aabb_intersecting(aabb_t box_1, aabb_t box_2);
extern bool aabb_axis_overlap(vec3 axis, float min_1, float max_1, float min_2, float max_2,
                              vec3 *min_axis, float *overlap, bool *negative);
extern aabb_t aabb_from_voxel_coord(ivec3 coord);
extern aabb_t aabb_with_offset(aabb_t box, vec3 offset);

#endif
