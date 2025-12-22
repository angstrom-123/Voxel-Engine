#ifndef EM_PERLIN_H
#define EM_PERLIN_H

#ifndef EM_PERLIN_INCLUDED
#define EM_PERLIN_INCLUDED

#include "em_global.h"

#include <stddef.h> // SIZE
#include <stdint.h>
#include <math.h> // floorf

extern INT perlin_hash(INT x);
extern INT perlin_pair(INT a, INT b);

extern FLOAT perlin_2d(UINT seed, FLOAT x, FLOAT y, FLOAT frequency);
extern FLOAT perlin_octave_2d(UINT seed, FLOAT x, FLOAT y, FLOAT frequency, UBYTE num_octaves);

extern FLOAT perlin_3d(UINT seed, FLOAT x, FLOAT y, FLOAT z, FLOAT frequency);
extern FLOAT perlin_octave_3d(UINT seed, FLOAT x, FLOAT y, FLOAT z, FLOAT frequency, UBYTE num_octaves);

#endif // EM_PERLIN_INCLUDED

/*     ______  _______  __    ________  __________   ___________  ______________  _   __
 *    /   /  |/  / __ \/ /   / ____/  |/  / ____/ | / /_  __/   |/_  __/   / __ \/ | / /
 *    / // /|_/ / /_/ / /   / __/ / /|_/ / __/ /  |/ / / / / /| | / /  / // / / /  |/ /
 *   / // /  / / ____/ /___/ /___/ /  / / /___/ /|  / / / / ___ |/ /  / // /_/ / /|  /
 * /___/_/  /_/_/   /_____/_____/_/  /_/_____/_/ |_/ /_/ /_/  |_/_/ /___/\____/_/ |_/
 *
 */

#ifdef EM_PERLIN_IMPL

#include "em_math.h" // vectors
#include "em_random.h" // fast random

vec2 _grad_2d(UINT seed, INT x, INT y)
{
    em_romu_mono32_init(seed + em_abs(perlin_pair(x, y)));

    FLOAT x_01 = (FLOAT) em_romu_mono32_random() / (FLOAT) MONO32_MAX;
    FLOAT y_01 = (FLOAT) em_romu_mono32_random() / (FLOAT) MONO32_MAX;

    FLOAT x_cmp = (2 * x_01) - 1.0;
    FLOAT y_cmp = (2 * y_01) - 1.0;

    return em_normalize_vec2((vec2) {x_cmp, y_cmp});
}

vec3 _grad_3d(UINT seed, INT x, INT y, INT z)
{
    em_romu_mono32_init(seed + em_abs(perlin_pair(x, perlin_pair(y, z))));

    FLOAT x_01 = (FLOAT) em_romu_mono32_random() / (FLOAT) MONO32_MAX;
    FLOAT y_01 = (FLOAT) em_romu_mono32_random() / (FLOAT) MONO32_MAX;
    FLOAT z_01 = (FLOAT) em_romu_mono32_random() / (FLOAT) MONO32_MAX;

    FLOAT x_cmp = (2 * x_01) - 1.0;
    FLOAT y_cmp = (2 * y_01) - 1.0;
    FLOAT z_cmp = (2 * z_01) - 1.0;

    return em_normalize_vec3((vec3) {x_cmp, y_cmp, z_cmp});
}

FLOAT _fade(FLOAT t)
{
    return t * t * t * (t * (t * 6 - 15.0) + 10.0);
}

FLOAT _lerp(FLOAT t, FLOAT a, FLOAT b)
{
    return a + t * (b - a);
}

INT perlin_hash(INT x)
{
    x = ~x + (x << 15);
    x = x ^ (x >> 12u);
    x = x + (x << 2);
    x = x ^ (x >> 4u);
    x = x * 2057;
    x = x ^ (x >> 16u);
    return x;
}

INT perlin_pair(INT a, INT b)
{
    if (a >= 0) a = 2 * a;
    else a = -2 * a - 1;

    if (b >= 0) b = 2 * b;
    else b = -2 * b - 1;

    return (a >= b) 
        ? a * a + b 
        : b * b + a;
}

FLOAT perlin_2d(UINT seed, FLOAT x, FLOAT y, FLOAT freq)
{
    x *= freq;
    y *= freq;

    /* Decimal part within grid cell. */
    const FLOAT x_f = x - floorf(x);
    const FLOAT y_f = y - floorf(y);

    /* Integer part describing the grid cell. */
    const INT x_i = (INT) floorf(x);
    const INT y_i = (INT) floorf(y);

    /* Position vectors of grid cell corners. */
    const ivec2 p0 = {x_i, y_i};
    const ivec2 p1 = {x_i + 1, y_i};
    const ivec2 p2 = {x_i, y_i + 1};
    const ivec2 p3 = {x_i + 1, y_i + 1};

    /* Gradients for each corner. */
    const vec2 g0 = _grad_2d(seed, p0.x, p0.y);
    const vec2 g1 = _grad_2d(seed, p1.x, p1.y);
    const vec2 g2 = _grad_2d(seed, p2.x, p2.y);
    const vec2 g3 = _grad_2d(seed, p3.x, p3.y);

    /* Offset vectors from each corner to the candidate point. */
    const vec2 o0 = em_sub_vec2((vec2) {x, y}, (vec2) {p0.x, p0.y});
    const vec2 o1 = em_sub_vec2((vec2) {x, y}, (vec2) {p1.x, p1.y});
    const vec2 o2 = em_sub_vec2((vec2) {x, y}, (vec2) {p2.x, p2.y});
    const vec2 o3 = em_sub_vec2((vec2) {x, y}, (vec2) {p3.x, p3.y});

    /* Gradient weights. */
    const FLOAT d0 = em_dot_vec2(g0, o0);
    const FLOAT d1 = em_dot_vec2(g1, o1);
    const FLOAT d2 = em_dot_vec2(g2, o2);
    const FLOAT d3 = em_dot_vec2(g3, o3);

    /* Lerp constants using smoothstep. */
    const FLOAT u = _fade(x_f);
    const FLOAT v = _fade(y_f);

    /* Interpolate gradients. */
    const FLOAT x0 = _lerp(u, d0, d1);
    const FLOAT x1 = _lerp(u, d2, d3);

    const FLOAT y0 = _lerp(v, x0, x1);

    return y0;
}

FLOAT perlin_octave_2d(UINT seed, FLOAT x, FLOAT y, FLOAT freq, UBYTE num)
{
    FLOAT res = 0.0;
    FLOAT amp = 1.0;
    FLOAT f = freq;

    for (SIZE i = 0; i < num; i++)
    {
        res += amp * perlin_2d(seed, x, y, f);
        amp /= 2.0;
        f *= 2.0;
    }

    return res;
}

FLOAT perlin_3d(UINT seed, FLOAT x, FLOAT y, FLOAT z, FLOAT freq)
{
    x *= freq;
    y *= freq;
    z *= freq;

    /* Decimal part within grid cell. */
    const FLOAT x_f = x - floorf(x);
    const FLOAT y_f = y - floorf(y);
    const FLOAT z_f = z - floorf(z);

    /* Integer part describing the grid cell. */
    const INT x_i = (INT) floorf(x);
    const INT y_i = (INT) floorf(y);
    const INT z_i = (INT) floorf(z);

    /* Position vectors of grid cell corners. */
    const ivec3 p0 = {x_i    , y_i    , z_i    };
    const ivec3 p1 = {x_i + 1, y_i    , z_i    };
    const ivec3 p2 = {x_i    , y_i + 1, z_i    };
    const ivec3 p3 = {x_i + 1, y_i + 1, z_i    };
    const ivec3 p4 = {x_i    , y_i    , z_i + 1};
    const ivec3 p5 = {x_i + 1, y_i    , z_i + 1};
    const ivec3 p6 = {x_i    , y_i + 1, z_i + 1};
    const ivec3 p7 = {x_i + 1, y_i + 1, z_i + 1};

    /* Gradients for each corner. */
    const vec3 g0 = _grad_3d(seed, p0.x, p0.y, p0.z);
    const vec3 g1 = _grad_3d(seed, p1.x, p1.y, p1.z);
    const vec3 g2 = _grad_3d(seed, p2.x, p2.y, p2.z);
    const vec3 g3 = _grad_3d(seed, p3.x, p3.y, p3.z);
    const vec3 g4 = _grad_3d(seed, p4.x, p4.y, p4.z);
    const vec3 g5 = _grad_3d(seed, p5.x, p5.y, p5.z);
    const vec3 g6 = _grad_3d(seed, p6.x, p6.y, p6.z);
    const vec3 g7 = _grad_3d(seed, p7.x, p7.y, p7.z);

    /* Offset vectors from each corner to the candidate point. */
    const vec3 o0 = em_sub_vec3((vec3) {x, y, z}, (vec3) {p0.x, p0.y, p0.z});
    const vec3 o1 = em_sub_vec3((vec3) {x, y, z}, (vec3) {p1.x, p1.y, p1.z});
    const vec3 o2 = em_sub_vec3((vec3) {x, y, z}, (vec3) {p2.x, p2.y, p2.z});
    const vec3 o3 = em_sub_vec3((vec3) {x, y, z}, (vec3) {p3.x, p3.y, p3.z});
    const vec3 o4 = em_sub_vec3((vec3) {x, y, z}, (vec3) {p4.x, p4.y, p4.z});
    const vec3 o5 = em_sub_vec3((vec3) {x, y, z}, (vec3) {p5.x, p5.y, p5.z});
    const vec3 o6 = em_sub_vec3((vec3) {x, y, z}, (vec3) {p6.x, p6.y, p6.z});
    const vec3 o7 = em_sub_vec3((vec3) {x, y, z}, (vec3) {p7.x, p7.y, p7.z});

    /* Gradient weights. */
    const FLOAT d0 = em_dot_vec3(g0, o0);
    const FLOAT d1 = em_dot_vec3(g1, o1);
    const FLOAT d2 = em_dot_vec3(g2, o2);
    const FLOAT d3 = em_dot_vec3(g3, o3);
    const FLOAT d4 = em_dot_vec3(g4, o4);
    const FLOAT d5 = em_dot_vec3(g5, o5);
    const FLOAT d6 = em_dot_vec3(g6, o6);
    const FLOAT d7 = em_dot_vec3(g7, o7);

    /* Lerp constants using smoothstep. */
    const FLOAT u = _fade(x_f);
    const FLOAT v = _fade(y_f);
    const FLOAT w = _fade(z_f);

    /* Interpolate gradients. */
    const FLOAT x0 = _lerp(u, d0, d1);
    const FLOAT x1 = _lerp(u, d2, d3);
    const FLOAT x2 = _lerp(u, d4, d5);
    const FLOAT x3 = _lerp(u, d6, d7);

    const FLOAT y0 = _lerp(v, x0, x1);
    const FLOAT y1 = _lerp(v, x2, x3);

    const FLOAT w1 = _lerp(w, y0, y1);

    return w1;
}

FLOAT perlin_octave_3d(UINT seed, FLOAT x, FLOAT y, FLOAT z, FLOAT freq, UBYTE num)
{
    FLOAT res = 0.0;
    FLOAT amp = 1.0;
    FLOAT f = freq;

    for (SIZE i = 0; i < num; i++)
    {
        res += amp * perlin_3d(seed, x, y, z, f);
        amp /= 2.0;
        f *= 2.0;
    }

    return res;
}
#endif // EM_PERLIN_IMPL

#endif
