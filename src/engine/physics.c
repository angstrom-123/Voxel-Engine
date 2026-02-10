#include "physics.h"

static const float INF = 10000000000.0;

bool aabb_ray_intersecting(aabb_t a, ray_t r)
{
    interval_t t_interval = { -INF, INF };
    for (size_t i = 0; i < 3; i++)
    {
        interval_t axis = a.axis[i];
        float axis_dir_inv = 1.0 / r.direction.elements[i];
        float t0 = (axis.min - r.origin.elements[i]) * axis_dir_inv;
        float t1 = (axis.max - r.origin.elements[i]) * axis_dir_inv;
        if (t0 < t1)
        {
            if (t0 > t_interval.min) t_interval.min = t0;
            if (t1 < t_interval.max) t_interval.max = t1;
        }
        else 
        {
            if (t1 > t_interval.min) t_interval.min = t1;
            if (t0 < t_interval.max) t_interval.max = t0;
        }

        if (t_interval.max <= t_interval.min) 
            return false;
    }

    // return t_interval.max <= r.length;
    return t_interval.min <= r.length;
}

bool aabb_intersecting(aabb_t a, aabb_t b)
{
    return a.x.min <= b.x.max && a.x.max >= b.x.min &&
           a.y.min <= b.y.max && a.y.max >= b.y.min &&
           a.z.min <= b.z.max && a.z.max >= b.z.min;
}

bool aabb_axis_overlap(vec3 ax, float ami, float ama, float bmi, float bma, 
                       vec3 *mtv_ax, float *mtv_dist, bool *neg)
{
    float lensq = em_length_squared_vec3(ax);

    float d0 = bma - ami;
    float d1 = ama - bmi;
    if (d0 <= 0.0 || d1 <= 0.0) 
        return false;

    float o = (d0 < d1) ? d0 : -d1;
    vec3 sep = em_mul_vec3_f(ax, o / lensq);
    float sep_lensq = em_length_squared_vec3(sep);
    if (sep_lensq < *mtv_dist)
    {
        *mtv_dist = sep_lensq;
        *mtv_ax = ax;
        *neg = o < 0.0;
    }

    return true;
}

bool aabb_intersecting_depth(aabb_t a, aabb_t b, vec3 *ax, float *d)
{
    vec3 axis;
    float dist = INF;
    bool neg;
    if (!aabb_axis_overlap(VEC3(1.0, 0.0, 0.0), a.x.min, a.x.max, b.x.min, b.x.max, 
                           &axis, &dist, &neg))
        return false;

    if (!aabb_axis_overlap(VEC3(0.0, 1.0, 0.0), a.y.min, a.y.max, b.y.min, b.y.max, 
                           &axis, &dist, &neg))
        return false;

    if (!aabb_axis_overlap(VEC3(0.0, 0.0, 1.0), a.z.min, a.z.max, b.z.min, b.z.max, 
                           &axis, &dist, &neg))
        return false;

    // _axis_overlap returns dist as a squared length.
    *d = sqrtf(dist) * (neg ? -1.0 : 1.0);
    *ax = axis;

    return true;
}

aabb_t aabb_from_voxel_coord(ivec3 coord)
{
    return (aabb_t) {
        .x = { coord.x, coord.x + 1.0 },
        .y = { coord.y, coord.y + 1.0 },
        .z = { coord.z, coord.z + 1.0 }
    };
}

aabb_t aabb_with_offset(aabb_t a, vec3 o)
{
    return (aabb_t) {
        .x = { a.x.min + o.x, a.x.max + o.x },
        .y = { a.y.min + o.y, a.y.max + o.y },
        .z = { a.z.min + o.z, a.z.max + o.z }
    };
}

float interval_size(interval_t interval)
{
    return interval.max - interval.min;
}

interval_t interval_around(float val, float size)
{
    return (interval_t) { val - size / 2.0, val + size / 2.0 };
}

bool interval_contains(interval_t interval, float val)
{
    return (interval.min <= val && interval.max >= val);
}

bool interval_contains_i(i_interval_t interval, int32_t val)
{
    return (interval.min <= val && interval.max >= val);
}
