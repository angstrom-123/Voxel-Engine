#ifndef EM_MATH_H
#define EM_MATH_H

#ifndef EM_MATH_INCLUDED
#define EM_MATH_INCLUDED

#include "em_global.h"

#define em_PI 3.14159265359
#define em_deg_to_rad(t) (t * (em_PI / 180.0))
#define em_rad_to_deg(t) (t * (180.0 / em_PI))
#define em_clamp(x, mi, ma) (((x) < (mi)) ? (mi) : (((x) > (ma)) ? (ma) : (x)))
#define em_min(x, y) ((x) > (y) ? (y) : (x))
#define em_max(x, y) ((x) < (y) ? (y) : (x))
#define em_absf(x) ((x) < 0.0 ? -(x) : (x))
#define em_abs(x) ((x) < 0 ? -(x) : (x))
#define em_sqr(x) ((x) * (x))
#define em_sign(x) (((x) < 0) ? -1 : (((x) > 0) ? 1 : 0))
#define em_signf(x) (((x) < 0.0) ? -1.0 : (((x) > 0.0) ? 1.0 : 0.0))

typedef union em_vec2 {
    struct {
        FLOAT x, y;
    };

    FLOAT elements[2];
} em_vec2;

typedef union em_vec3 {
    struct {
        FLOAT x, y, z;
    };

    FLOAT elements[3];
} em_vec3;

typedef union em_vec4 {
    struct {
        FLOAT x, y, z, w;
    };

    FLOAT elements[4];
} em_vec4;

typedef union em_ivec2 {
    struct {
        INT x, y;
    };

    INT elements[2];
} em_ivec2;

typedef union em_ivec3 {
    struct {
        INT x, y, z;
    };

    INT elements[3];
} em_ivec3;

typedef union em_ivec4 {
    struct {
        INT x, y, z, w;
    };

    INT elements[4];
} em_ivec4;

typedef union em_uvec2 {
    struct {
        UINT x, y;
    };

    UINT elements[2];
} em_uvec2;

typedef union em_uvec3 {
    struct {
        UINT x, y, z;
    };

    UINT elements[3];
} em_uvec3;

typedef union em_uvec4 {
    struct {
        UINT x, y, z, w;
    };

    UINT elements[4];
} em_uvec4;

typedef union em_mat3 {
    struct {
        em_vec4 x, y, z;
    };

    FLOAT elements[3][3];
} em_mat3;

typedef union em_mat4 {
    struct {
        em_vec4 x, y, z, w;
    };

    FLOAT elements[4][4];
} em_mat4;

typedef em_vec4 em_quaternion;

typedef struct em_interval {
    FLOAT min;
    FLOAT max;
} em_interval;

typedef struct em_iinterval {
    INT min;
    INT max;
} em_iinterval;

typedef struct em_uinterval {
    UINT min;
    UINT max;
} em_uinterval;

#if ALIAS
    typedef em_quaternion quat;
    
    typedef em_vec2 vec2;
    typedef em_vec3 vec3;
    typedef em_vec4 vec4;
    
    typedef em_ivec2 ivec2;
    typedef em_ivec3 ivec3;
    typedef em_ivec4 ivec4;
    
    typedef em_uvec2 uvec2;
    typedef em_uvec3 uvec3;
    typedef em_uvec4 uvec4;
    
    typedef em_mat3 mat3;
    typedef em_mat4 mat4;

    typedef em_interval itvl;
    typedef em_iinterval iitvl;
    typedef em_uinterval uitvl;
#endif

#define NEGATE_VEC2(v) (em_vec2) { -v.x, -v.y }
#define NEGATE_VEC3(v) (em_vec3) { -v.x, -v.y, -v.z }
#define NEGATE_VEC4(v) (em_vec4) { -v.x, -v.y, -v.z, -v.w }
#define NEGATE_IVEC2(v) (em_ivec2) { -v.x, -v.y }
#define NEGATE_IVEC3(v) (em_ivec3) { -v.x, -v.y, -v.z }
#define NEGATE_IVEC4(v) (em_ivec4) { -v.x, -v.y, -v.z, -v.w }

#define VEC2F(f) (em_vec2) { (FLOAT) (f), (FLOAT) (f) }
#define VEC3F(f) (em_vec3) { (FLOAT) (f), (FLOAT) (f), (FLOAT) (f) }
#define VEC4F(f) (em_vec4) { (FLOAT) (f), (FLOAT) (f), (FLOAT) (f), (FLOAT) (f) }
#define IVEC2I(i) (em_ivec2) { (INT) (i), (INT) (i) }
#define IVEC3I(i) (em_ivec3) { (INT) (i), (INT) (i), (INT) (i) }
#define IVEC4I(i) (em_ivec4) { (INT) (i), (INT) (i), (INT) (i), (INT) (i) }
#define UVEC2U(u) (em_uvec2) { (UINT) (u), (UINT) (u) }
#define UVEC3U(u) (em_uvec3) { (UINT) (u), (UINT) (u), (UINT) (u) }
#define UVEC4U(u) (em_uvec4) { (UINT) (u), (UINT) (u), (UINT) (u), (UINT) (u) }
#define VEC2(a, b) (em_vec2) { (FLOAT) (a), (FLOAT) (b) }
#define VEC3(a, b, c) (em_vec3) { (FLOAT) (a), (FLOAT) (b), (FLOAT) (c) }
#define VEC4(a, b, c, d) (em_vec4) { (FLOAT) (a), (FLOAT) (b), (FLOAT) (c), (FLOAT) (d) }
#define QUAT(a, b, c, d) (em_quaternion) { (FLOAT) (a), (FLOAT) (b), (FLOAT) (c), (FLOAT) (d) }
#define IVEC2(a, b) (em_ivec2) { (INT) (a), (INT) (b) }
#define IVEC3(a, b, c) (em_ivec3) { (INT) (a), (INT) (b), (INT) (c) }
#define IVEC4(a, b, c, d) (em_ivec4) { (INT) (a), (INT) (b), (INT) (c), (INT) (d) }
#define UVEC2(a, b) (em_uvec2) { (UINT) (a), (UINT) (b) }
#define UVEC3(a, b, c) (em_uvec3) { (UINT) (a), (UINT) (b), (UINT) (c) }
#define UVEC4(a, b, c, d) (em_uvec4) { (UINT) (a), (UINT) (b), (UINT) (c), (UINT) (d) }

#define AS_VEC2(v) (em_vec2) { (FLOAT) v.x, (FLOAT) v.y }
#define AS_VEC3(v) (em_vec3) { (FLOAT) v.x, (FLOAT) v.y, (FLOAT) v.z }
#define AS_VEC4(v) (em_vec4) { (FLOAT) v.x, (FLOAT) v.y, (FLOAT) v.z, (FLOAT) v.w }
#define AS_IVEC2(v) (em_ivec2) { (INT) v.x, (INT) v.y }
#define AS_IVEC3(v) (em_ivec3) { (INT) v.x, (INT) v.y, (INT) v.z }
#define AS_IVEC4(v) (em_ivec4) { (INT) v.x, (INT) v.y, (INT) v.z, (INT) v.w }
#define AS_UVEC2(v) (em_uvec2) { (UINT) v.x, (UINT) v.y }
#define AS_UVEC3(v) (em_uvec3) { (UINT) v.x, (UINT) v.y, (UINT) v.z }
#define AS_UVEC4(v) (em_uvec4) { (UINT) v.x, (UINT) v.y, (UINT) v.z, (UINT) v.w }

#define DECOMPOSE_2(v) v.x, v.y
#define DECOMPOSE_3(v) v.x, v.y, v.z
#define DECOMPOSE_4(v) v.x, v.y, v.z, v.w

#define INDEX_2(v) v.x][v.y
#define INDEX_3(v) v.x][v.y][v.z
#define INDEX_4(v) v.x][v.y][v.z][v.w

bool em_equals_vec3(em_vec3 a, em_vec3 b);
bool em_equals_vec4(em_vec4 a, em_vec4 b);
bool em_equals_quaternion(em_quaternion a, em_quaternion b);
bool em_equals_ivec2(em_ivec2 a, em_ivec2 b);
bool em_equals_ivec3(em_ivec3 a, em_ivec3 b);

em_vec2 em_sign_vec2(em_vec2 a);
em_vec3 em_sign_vec3(em_vec3 a);
em_vec4 em_sign_vec4(em_vec4 a);

em_ivec2 em_sign_ivec2(em_ivec2 a);
em_ivec3 em_sign_ivec3(em_ivec3 a);
em_ivec4 em_sign_ivec4(em_ivec4 a);

em_ivec2 em_floor_vec2(em_vec2 a);
em_ivec3 em_floor_vec3(em_vec3 a);
em_ivec4 em_floor_vec4(em_vec4 a);
em_vec2 em_floor_vec2f(em_vec2 a);
em_vec3 em_floor_vec3f(em_vec3 a);
em_vec4 em_floor_vec4f(em_vec4 a);

em_vec2 em_clamp_vec2(em_vec2 a, em_vec2 min, em_vec2 max);
em_vec3 em_clamp_vec3(em_vec3 a, em_vec3 min, em_vec3 max);
em_vec4 em_clamp_vec4(em_vec4 a, em_vec4 min, em_vec4 max);

em_vec2 em_min_vec2(em_vec2 a, em_vec2 b);
em_vec3 em_min_vec3(em_vec3 a, em_vec3 b);
em_vec4 em_min_vec4(em_vec4 a, em_vec4 b);
em_ivec2 em_min_ivec2(em_ivec2 a, em_ivec2 b);
em_ivec3 em_min_ivec3(em_ivec3 a, em_ivec3 b);
em_ivec4 em_min_ivec4(em_ivec4 a, em_ivec4 b);
em_uvec2 em_min_uvec2(em_uvec2 a, em_uvec2 b);
em_uvec3 em_min_uvec3(em_uvec3 a, em_uvec3 b);
em_uvec4 em_min_uvec4(em_uvec4 a, em_uvec4 b);

em_vec2 em_sqrt_vec2(em_vec2 a);
em_vec3 em_sqrt_vec3(em_vec3 a);
em_vec4 em_sqrt_vec4(em_vec4 a);

em_vec2 em_abs_vec2(em_vec2 a);
em_vec3 em_abs_vec3(em_vec3 a);
em_vec4 em_abs_vec4(em_vec4 a);
em_ivec2 em_abs_ivec2(em_ivec2 a);
em_ivec3 em_abs_ivec3(em_ivec3 a);
em_ivec4 em_abs_ivec4(em_ivec4 a);

em_vec2 em_ivec2_as_vec2(em_ivec2 a);
em_vec3 em_ivec3_as_vec3(em_ivec3 a);
em_vec4 em_ivec3_as_vec4(em_ivec4 a);
em_ivec2 em_vec2_as_ivec2(em_vec2 a);
em_ivec3 em_vec3_as_ivec3(em_vec3 a);
em_ivec4 em_vec3_as_ivec4(em_vec4 a);
em_vec2 em_uvec2_as_vec2(em_uvec2 a);
em_vec3 em_uvec3_as_vec3(em_uvec3 a);
em_vec4 em_uvec3_as_vec4(em_uvec4 a);

em_vec4 em_new_vec4_vec3_f(em_vec3 xyz, FLOAT w);
em_mat4 em_new_mat4(void);
em_mat4 em_new_mat4_diagonal(FLOAT diagonal);
em_quaternion em_new_quaternion_vec3_f(em_vec3 xyz, FLOAT w);

em_vec2 em_normalize_vec2(em_vec2 a);
em_vec3 em_normalize_vec3(em_vec3 a);
em_vec4 em_normalize_vec4(em_vec4 a);

FLOAT em_length_vec2(em_vec2 a);
FLOAT em_length_vec3(em_vec3 a);
FLOAT em_length_vec4(em_vec4 a);

FLOAT em_length_squared_vec2(em_vec2 a);
FLOAT em_length_squared_vec3(em_vec3 a);
FLOAT em_length_squared_vec4(em_vec4 a);

FLOAT em_dot_vec2(em_vec2 a, em_vec2 b);
FLOAT em_dot_vec3(em_vec3 a, em_vec3 b);
FLOAT em_dot_vec4(em_vec4 a, em_vec4 b);
FLOAT em_dot_ivec2(em_ivec2 a, em_ivec2 b);
FLOAT em_dot_ivec3(em_ivec3 a, em_ivec3 b);
FLOAT em_dot_ivec4(em_ivec4 a, em_ivec4 b);

em_vec3 em_cross_vec3(em_vec3 a, em_vec3 b);

em_vec2 em_add_vec2(em_vec2 a, em_vec2 b);
em_vec3 em_add_vec3(em_vec3 a, em_vec3 b);
em_vec4 em_add_vec4(em_vec4 a, em_vec4 b);
em_vec2 em_add_vec2_f(em_vec2 a, FLOAT b);
em_vec3 em_add_vec3_f(em_vec3 a, FLOAT b);
em_vec4 em_add_vec4_f(em_vec4 a, FLOAT b);
em_ivec2 em_add_ivec2(em_ivec2 a, em_ivec2 b);
em_ivec3 em_add_ivec3(em_ivec3 a, em_ivec3 b);
em_ivec4 em_add_ivec4(em_ivec4 a, em_ivec4 b);
em_ivec2 em_add_ivec2_i(em_ivec2 a, INT b);
em_ivec3 em_add_ivec3_i(em_ivec3 a, INT b);
em_ivec4 em_add_ivec4_i(em_ivec4 a, INT b);

em_vec2 em_sub_vec2(em_vec2 a, em_vec2 b);
em_vec3 em_sub_vec3(em_vec3 a, em_vec3 b);
em_vec4 em_sub_vec4(em_vec4 a, em_vec4 b);
em_ivec2 em_sub_ivec2(em_ivec2 a, em_ivec2 b);
em_ivec3 em_sub_ivec3(em_ivec3 a, em_ivec3 b);
em_ivec4 em_sub_ivec4(em_ivec4 a, em_ivec4 b);

em_vec2 em_mul_vec2(em_vec2 a, em_vec2 b);
em_vec3 em_mul_vec3(em_vec3 a, em_vec3 b);
em_vec4 em_mul_vec4(em_vec4 a, em_vec4 b);
em_vec2 em_mul_vec2_f(em_vec2 a, FLOAT b);
em_vec3 em_mul_vec3_f(em_vec3 a, FLOAT b);
em_vec4 em_mul_vec4_f(em_vec4 a, FLOAT b);
em_ivec2 em_mul_ivec2(em_ivec2 a, em_ivec2 b);
em_ivec3 em_mul_ivec3(em_ivec3 a, em_ivec3 b);
em_ivec4 em_mul_ivec4(em_ivec4 a, em_ivec4 b);
em_ivec2 em_mul_ivec2_i(em_ivec2 a, INT b);
em_ivec3 em_mul_ivec3_i(em_ivec3 a, INT b);
em_ivec4 em_mul_ivec4_i(em_ivec4 a, INT b);

em_vec2 em_div_vec2(em_vec2 a, em_vec2 b);
em_vec3 em_div_vec3(em_vec3 a, em_vec3 b);
em_vec4 em_div_vec4(em_vec4 a, em_vec4 b);
em_vec2 em_div_vec2_f(em_vec2 a, FLOAT b);
em_vec3 em_div_vec3_f(em_vec3 a, FLOAT b);
em_vec4 em_div_vec4_f(em_vec4 a, FLOAT b);
em_ivec2 em_div_ivec2(em_ivec2 a, em_ivec2 b);
em_ivec3 em_div_ivec3(em_ivec3 a, em_ivec3 b);
em_ivec4 em_div_ivec4(em_ivec4 a, em_ivec4 b);
em_ivec2 em_div_ivec2_i(em_ivec2 a, INT b);
em_ivec3 em_div_ivec3_i(em_ivec3 a, INT b);
em_ivec4 em_div_ivec4_i(em_ivec4 a, INT b);

em_mat4 em_mul_mat4(em_mat4 a, em_mat4 b);
em_mat4 em_mul_mat4_f(em_mat4 a, FLOAT b);
em_vec4 em_mul_mat4_v4(em_mat4 a, em_vec4 b);

em_mat4 em_div_mat4_f(em_mat4 a, FLOAT b);

em_mat4 em_orthographic(FLOAT left, FLOAT right, FLOAT bottom, FLOAT top, FLOAT near, FLOAT far);
// em_mat4 em_orthographic(FLOAT width, FLOAT height, FLOAT near, FLOAT far);
em_mat4 em_perspective(FLOAT fov_degrees, FLOAT aspect_ratio, FLOAT near, FLOAT far);
em_mat4 em_transpose_mat4(em_mat4 matrix);
em_mat4 em_translate_mat4(em_vec3 translation);
em_vec3 em_quaternion_rotate_vec3(em_vec3 vector, em_quaternion rotation);
em_vec3 em_rotate_vec3(em_vec3 vector, FLOAT angle_degrees, em_vec3 axis);
em_mat4 em_rotate_mat4(FLOAT angle_degrees, em_vec3 axis);
em_mat4 em_scale_mat4(em_vec3 scale);
em_mat4 em_inverse_mat4(em_mat4 matrix);
FLOAT em_determinant_mat4(em_mat4 matrix);
em_mat4 em_look_at(em_vec3 eye, em_vec3 centre, em_vec3 up);

em_quaternion em_conjugate_quaternion(em_quaternion a);
em_quaternion em_inverse_quaternion(em_quaternion a);
em_mat4 em_quaternion_to_mat4(em_quaternion a);
em_quaternion em_vec3_to_quaternion(em_vec3 a);
em_quaternion em_quaternion_from_axis_angle(em_vec3 axis, FLOAT angle_degrees);

em_quaternion em_add_quaternion(em_quaternion a, em_quaternion b);
em_quaternion em_sub_quaternion(em_quaternion a, em_quaternion b);
em_quaternion em_mul_quaternion(em_quaternion a, em_quaternion b);
em_quaternion em_mul_quaternion_f(em_quaternion a, FLOAT b);
em_quaternion em_mul_quaternion_vec3(em_quaternion a, em_vec3 b);
em_quaternion em_div_quaternion(em_quaternion a, em_quaternion b);
em_quaternion em_div_quaternion_f(em_quaternion a, FLOAT b);
em_quaternion em_normalize_quaternion(em_quaternion a);
FLOAT em_dot_quaternion(em_quaternion a, em_quaternion b);

FLOAT em_interval_size(em_interval i);
INT em_iinterval_size(em_iinterval i);
em_interval em_interval_around(FLOAT val, FLOAT size);
em_iinterval em_iinterval_around(INT val, INT size);
bool em_interval_contains(em_interval i, FLOAT val);
bool em_iinterval_contains(em_iinterval i, INT val);
bool em_interval_surrounds(em_interval i, FLOAT val);
bool em_iinterval_surrounds(em_iinterval i, INT val);

#endif // EM_MATH_INCLUDED

/*     ______  _______  __    ________  __________   ___________  ______________  _   __
 *    /   /  |/  / __ \/ /   / ____/  |/  / ____/ | / /_  __/   |/_  __/   / __ \/ | / /
 *    / // /|_/ / /_/ / /   / __/ / /|_/ / __/ /  |/ / / / / /| | / /  / // / / /  |/ /
 *   / // /  / / ____/ /___/ /___/ /  / / /___/ /|  / / / / ___ |/ /  / // /_/ / /|  /
 * /___/_/  /_/_/   /_____/_____/_/  /_/_____/_/ |_/ /_/ /_/  |_/_/ /___/\____/_/ |_/
 *
 */

#ifdef EM_MATH_IMPL

#include <math.h>

em_vec2 em_sign_vec2(em_vec2 a)
{
    return (em_vec2) {
        em_signf(a.x),
        em_signf(a.y)
    };
}

em_vec3 em_sign_vec3(em_vec3 a)
{
    return (em_vec3) {
        em_signf(a.x),
        em_signf(a.y),
        em_signf(a.z)
    };
}

em_vec4 em_sign_vec4(em_vec4 a)
{
    return (em_vec4) {
        em_signf(a.x),
        em_signf(a.y),
        em_signf(a.z),
        em_signf(a.w)
    };
}

em_ivec2 em_sign_ivec2(em_ivec2 a)
{
    return (em_ivec2) {
        em_sign(a.x),
        em_sign(a.y)
    };
}

em_ivec3 em_sign_ivec3(em_ivec3 a)
{
    return (em_ivec3) {
        em_sign(a.x),
        em_sign(a.y),
        em_sign(a.z)
    };
}

em_ivec4 em_sign_ivec4(em_ivec4 a)
{
    return (em_ivec4) {
        em_sign(a.x),
        em_sign(a.y),
        em_sign(a.z),
        em_sign(a.w)
    };
}

em_ivec2 em_floor_vec2(em_vec2 a)
{
    return (em_ivec2) {floorf(a.x), floorf(a.y)};
}

em_ivec3 em_floor_vec3(em_vec3 a)
{
    return (em_ivec3) {floorf(a.x), floorf(a.y), floorf(a.z)};
}

em_ivec4 em_floor_vec4(em_vec4 a)
{
    return (em_ivec4) {floorf(a.x), floorf(a.y), floorf(a.z), floorf(a.w)};
}

em_vec2 em_floor_vec2f(em_vec2 a)
{
    return (em_vec2) {floorf(a.x), floorf(a.y)};
}

em_vec3 em_floor_vec3f(em_vec3 a)
{
    return (em_vec3) {floorf(a.x), floorf(a.y), floorf(a.z)};
}

em_vec4 em_floor_vec4f(em_vec4 a)
{
    return (em_vec4) {floorf(a.x), floorf(a.y), floorf(a.z), floorf(a.w)};
}

em_vec2 em_clamp_vec2(em_vec2 a, em_vec2 min, em_vec2 max)
{
    return (em_vec2) {em_clamp(a.x, min.x, max.x), em_clamp(a.y, min.y, max.y)};
}

em_vec3 em_clamp_vec3(em_vec3 a, em_vec3 min, em_vec3 max)
{
    return (em_vec3) {em_clamp(a.x, min.x, max.x), em_clamp(a.y, min.y, max.y), em_clamp(a.z, min.z, max.z)};
}

em_vec4 em_clamp_vec4(em_vec4 a, em_vec4 min, em_vec4 max)
{
    return (em_vec4) {em_clamp(a.x, min.x, max.x), em_clamp(a.y, min.y, max.y), em_clamp(a.z, min.z, max.z), em_clamp(a.w, min.w, max.w)};
}

em_vec2 em_min_vec2(em_vec2 a, em_vec2 b)
{
    return (em_vec2) {em_min(a.x, b.x), em_min(a.y, b.y)};
}

em_vec3 em_min_vec3(em_vec3 a, em_vec3 b)
{
    return (em_vec3) {em_min(a.x, b.x), em_min(a.y, b.y), em_min(a.z, b.z)};
}

em_vec4 em_min_vec4(em_vec4 a, em_vec4 b)
{
    return (em_vec4) {em_min(a.x, b.x), em_min(a.y, b.y), em_min(a.z, b.z), em_min(a.w, b.w)};
}

em_ivec2 em_min_ivec2(em_ivec2 a, em_ivec2 b)
{
    return (em_ivec2) {em_min(a.x, b.x), em_min(a.y, b.y)};
}

em_ivec3 em_min_ivec3(em_ivec3 a, em_ivec3 b)
{
    return (em_ivec3) {em_min(a.x, b.x), em_min(a.y, b.y), em_min(a.z, b.z)};
}

em_ivec4 em_min_ivec4(em_ivec4 a, em_ivec4 b)
{
    return (em_ivec4) {em_min(a.x, b.x), em_min(a.y, b.y), em_min(a.z, b.z), em_min(a.w, b.w)};
}

em_uvec2 em_min_uvec2(em_uvec2 a, em_uvec2 b)
{
    return (em_uvec2) {em_min(a.x, b.x), em_min(a.y, b.y)};
}

em_uvec3 em_min_uvec3(em_uvec3 a, em_uvec3 b)
{
    return (em_uvec3) {em_min(a.x, b.x), em_min(a.y, b.y), em_min(a.z, b.z)};
}

em_uvec4 em_min_uvec4(em_uvec4 a, em_uvec4 b)
{
    return (em_uvec4) {em_min(a.x, b.x), em_min(a.y, b.y), em_min(a.z, b.z), em_min(a.w, b.w)};
}

em_vec2 em_abs_vec2(em_vec2 a)
{
    return (em_vec2) {em_abs(a.x), em_abs(a.y)};
}

em_vec3 em_abs_vec3(em_vec3 a)
{
    return (em_vec3) {em_abs(a.x), em_abs(a.y), em_abs(a.z)};
}

em_vec4 em_abs_vec4(em_vec4 a)
{
    return (em_vec4) {em_abs(a.x), em_abs(a.y), em_abs(a.z), em_abs(a.w)};
}

em_ivec2 em_abs_ivec2(em_ivec2 a)
{
    return (em_ivec2) {em_abs(a.x), em_abs(a.y)};
}

em_ivec3 em_abs_ivec3(em_ivec3 a)
{
    return (em_ivec3) {em_abs(a.x), em_abs(a.y), em_abs(a.z)};
}

em_ivec4 em_abs_ivec4(em_ivec4 a)
{
    return (em_ivec4) {em_abs(a.x), em_abs(a.y), em_abs(a.z), em_abs(a.w)};
}

em_vec2 em_sqrt_vec2(em_vec2 a)
{
    return (em_vec2) {sqrtf(a.x), sqrtf(a.y)};
}

em_vec3 em_sqrt_vec3(em_vec3 a)
{
    return (em_vec3) {sqrtf(a.x), sqrtf(a.y), sqrtf(a.z)};
}

em_vec4 em_sqrt_vec4(em_vec4 a)
{
    return (em_vec4) {sqrtf(a.x), sqrtf(a.y), sqrtf(a.z), sqrtf(a.w)};
}

bool em_equals_vec3(em_vec3 a, em_vec3 b)
{
    return (a.x == b.x) && (a.y == b.y) && (a.z == b.z);
}

bool em_equals_vec4(em_vec4 a, em_vec4 b)
{
    return (a.x == b.x) && (a.y == b.y) && (a.z == b.z) && (a.w == b.w);
}

bool em_equals_quaternion(em_quaternion a, em_quaternion b)
{
    return em_equals_vec4(a, b);
}

bool em_equals_ivec2(em_ivec2 a, em_ivec2 b)
{
    return (a.x == b.x) && (a.y == b.y);
}

bool em_equals_ivec3(em_ivec3 a, em_ivec3 b)
{
    return (a.x == b.x) && (a.y == b.y) && (a.z == b.z);
}

em_vec2 em_ivec2_as_vec2(em_ivec2 a)
{
    return (em_vec2) {(FLOAT) a.x, (FLOAT) a.y};
}

em_vec3 em_ivec3_as_vec3(em_ivec3 a)
{
    return (em_vec3) {(FLOAT) a.x, (FLOAT) a.y, (FLOAT) a.z};
}

em_vec4 em_ivec4_as_vec4(em_ivec4 a)
{
    return (em_vec4) {(FLOAT) a.x, (FLOAT) a.y, (FLOAT) a.z, (FLOAT) a.w};
}

em_ivec2 em_vec2_as_ivec2(em_vec2 a)
{
    return (em_ivec2) {(INT) a.x, (INT) a.y};
}

em_ivec3 em_vec3_as_ivec3(em_vec3 a)
{
    return (em_ivec3) {(INT) a.x, (INT) a.y, (INT) a.z};
}

em_ivec4 em_vec4_as_ivec4(em_vec4 a)
{
    return (em_ivec4) {(INT) a.x, (INT) a.y, (INT) a.z, (INT) a.w};
}

em_vec2 em_uvec2_as_vec2(em_uvec2 a)
{
    return (em_vec2) {(FLOAT) a.x, (FLOAT) a.y};
}

em_vec3 em_uvec3_as_vec3(em_uvec3 a)
{
    return (em_vec3) {(FLOAT) a.x, (FLOAT) a.y, (FLOAT) a.z};
}

em_vec4 em_uvec4_as_vec4(em_uvec4 a)
{
    return (em_vec4) {(FLOAT) a.x, (FLOAT) a.y, (FLOAT) a.z, (FLOAT) a.w};
}

em_vec4 em_new_vec4_vec3_f(em_vec3 xyz, FLOAT w)
{
    return (em_vec4) {xyz.x, xyz.y, xyz.z, w};
}

em_mat4 em_new_mat4(void) 
{
    return (em_mat4) {
        .elements = {
            {0.0, 0.0, 0.0, 0.0},
            {0.0, 0.0, 0.0, 0.0},
            {0.0, 0.0, 0.0, 0.0},
            {0.0, 0.0, 0.0, 0.0}
        }
    };
}

em_mat4 em_new_mat4_diagonal(FLOAT diagonal) 
{
    return (em_mat4) {
        .elements = {
            {diagonal, 0.0, 0.0, 0.0},
            {0.0, diagonal, 0.0, 0.0},
            {0.0, 0.0, diagonal, 0.0},
            {0.0, 0.0, 0.0, diagonal}
        }
    };
}

em_quaternion em_new_quaternion_vec3_f(em_vec3 xyz, FLOAT w)
{
    return (em_quaternion) {xyz.x, xyz.y, xyz.z, w};
}

em_vec2 em_normalize_vec2(em_vec2 a) 
{
    FLOAT len = em_length_vec2(a);
    if (len > 1.0E-6)
        return em_div_vec2_f(a, len);
    return a;
}

em_vec3 em_normalize_vec3(em_vec3 a)
{
    FLOAT len = em_length_vec3(a);
    if (len > 1.0E-6)
        return em_div_vec3_f(a, len);
    return a;
}

em_vec4 em_normalize_vec4(em_vec4 a)
{
    FLOAT len = em_length_vec4(a);
    if (len > 1.0E-6)
        return em_div_vec4_f(a, len);
    return a;
}

FLOAT em_length_vec2(em_vec2 a) 
{
    return sqrtf(em_length_squared_vec2(a));
}

FLOAT em_length_vec3(em_vec3 a)
{
    return sqrtf(em_length_squared_vec3(a));
}

FLOAT em_length_vec4(em_vec4 a)
{
    return sqrtf(em_length_squared_vec4(a));
}

FLOAT em_length_squared_vec2(em_vec2 a)
{
    return em_dot_vec2(a, a);
}

FLOAT em_length_squared_vec3(em_vec3 a)
{
    return em_dot_vec3(a, a);
}

FLOAT em_length_squared_vec4(em_vec4 a)
{
    return em_dot_vec4(a, a);
}

FLOAT em_dot_vec2(em_vec2 a, em_vec2 b) 
{
    return (a.x * b.x) + (a.y * b.y);
}

FLOAT em_dot_vec3(em_vec3 a, em_vec3 b)
{
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

FLOAT em_dot_vec4(em_vec4 a, em_vec4 b)
{
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}

FLOAT em_dot_ivec2(em_ivec2 a, em_ivec2 b) 
{
    return (a.x * b.x) + (a.y * b.y);
}

FLOAT em_dot_ivec3(em_ivec3 a, em_ivec3 b)
{
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

FLOAT em_dot_ivec4(em_ivec4 a, em_ivec4 b)
{
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}

em_vec3 em_cross_vec3(em_vec3 a, em_vec3 b)
{
    return (em_vec3) {
        (a.y * b.z) - (a.z * b.y),
        (a.z * b.x) - (a.x * b.z),
        (a.x * b.y) - (a.y * b.x)
    };
}

em_vec2 em_add_vec2(em_vec2 a, em_vec2 b)
{
    return (em_vec2) {
        a.x + b.x, 
        a.y + b.y 
    };
}

em_vec3 em_add_vec3(em_vec3 a, em_vec3 b) 
{
    return (em_vec3) {
        a.x + b.x,
        a.y + b.y,
        a.z + b.z
    };
}

em_vec4 em_add_vec4(em_vec4 a, em_vec4 b)
{
    return (em_vec4) {
        a.x + b.x,
        a.y + b.y,
        a.z + b.z,
        a.w + b.w
    };
}

em_vec2 em_add_vec2_f(em_vec2 a, FLOAT b)
{
    return (em_vec2) {
        a.x + b, 
        a.y + b 
    };
}

em_vec3 em_add_vec3_f(em_vec3 a, FLOAT b) 
{
    return (em_vec3) {
        a.x + b,
        a.y + b,
        a.z + b
    };
}

em_vec4 em_add_vec4_f(em_vec4 a, FLOAT b)
{
    return (em_vec4) {
        a.x + b,
        a.y + b,
        a.z + b,
        a.w + b
    };
}

em_ivec2 em_add_ivec2(em_ivec2 a, em_ivec2 b)
{
    return (em_ivec2) {
        a.x + b.x, 
        a.y + b.y 
    };
}

em_ivec3 em_add_ivec3(em_ivec3 a, em_ivec3 b) 
{
    return (em_ivec3) {
        a.x + b.x,
        a.y + b.y,
        a.z + b.z
    };
}

em_ivec4 em_add_ivec4(em_ivec4 a, em_ivec4 b)
{
    return (em_ivec4) {
        a.x + b.x,
        a.y + b.y,
        a.z + b.z,
        a.w + b.w
    };
}

em_ivec2 em_add_ivec2_i(em_ivec2 a, INT b)
{
    return (em_ivec2) {
        a.x + b, 
        a.y + b 
    };
}

em_ivec3 em_add_ivec3_i(em_ivec3 a, INT b) 
{
    return (em_ivec3) {
        a.x + b,
        a.y + b,
        a.z + b
    };
}

em_ivec4 em_add_ivec4_i(em_ivec4 a, INT b)
{
    return (em_ivec4) {
        a.x + b,
        a.y + b,
        a.z + b,
        a.w + b
    };
}

em_vec2 em_sub_vec2(em_vec2 a, em_vec2 b)
{
    return (em_vec2) {
        a.x - b.x, 
        a.y - b.y 
    };
}

em_vec3 em_sub_vec3(em_vec3 a, em_vec3 b) 
{
    return (em_vec3) {
        a.x - b.x,
        a.y - b.y,
        a.z - b.z
    };
}

em_vec4 em_sub_vec4(em_vec4 a, em_vec4 b)
{
    return (em_vec4) {
        a.x - b.x,
        a.y - b.y,
        a.z - b.z,
        a.w - b.w
    };
}

em_ivec2 em_sub_ivec2(em_ivec2 a, em_ivec2 b)
{
    return (em_ivec2) {
        a.x - b.x, 
        a.y - b.y 
    };
}

em_ivec3 em_sub_ivec3(em_ivec3 a, em_ivec3 b) 
{
    return (em_ivec3) {
        a.x - b.x,
        a.y - b.y,
        a.z - b.z
    };
}

em_ivec4 em_sub_ivec4(em_ivec4 a, em_ivec4 b)
{
    return (em_ivec4) {
        a.x - b.x,
        a.y - b.y,
        a.z - b.z,
        a.w - b.w
    };
}

em_vec2 em_mul_vec2(em_vec2 a, em_vec2 b)
{
    return (em_vec2) {
        a.x * b.x, 
        a.y * b.y 
    };
}

em_vec3 em_mul_vec3(em_vec3 a, em_vec3 b) 
{
    return (em_vec3) {
        a.x * b.x,
        a.y * b.y,
        a.z * b.z
    };
}

em_vec4 em_mul_vec4(em_vec4 a, em_vec4 b)
{
    return (em_vec4) {
        a.x * b.x,
        a.y * b.y,
        a.z * b.z,
        a.w * b.w
    };
}

em_vec2 em_mul_vec2_f(em_vec2 a, FLOAT b)
{
    return (em_vec2) {
        a.x * b, 
        a.y * b 
    };
}

em_vec3 em_mul_vec3_f(em_vec3 a, FLOAT b) 
{
    return (em_vec3) {
        a.x * b,
        a.y * b,
        a.z * b
    };
}

em_vec4 em_mul_vec4_f(em_vec4 a, FLOAT b)
{
    return (em_vec4) {
        a.x * b,
        a.y * b,
        a.z * b,
        a.w * b
    };
}

em_ivec2 em_mul_ivec2(em_ivec2 a, em_ivec2 b)
{
    return (em_ivec2) {
        a.x * b.x, 
        a.y * b.y 
    };
}

em_ivec3 em_mul_ivec3(em_ivec3 a, em_ivec3 b) 
{
    return (em_ivec3) {
        a.x * b.x,
        a.y * b.y,
        a.z * b.z
    };
}

em_ivec4 em_mul_ivec4(em_ivec4 a, em_ivec4 b)
{
    return (em_ivec4) {
        a.x * b.x,
        a.y * b.y,
        a.z * b.z,
        a.w * b.w
    };
}

em_ivec2 em_mul_ivec2_i(em_ivec2 a, INT b)
{
    return (em_ivec2) {
        a.x * b, 
        a.y * b 
    };
}

em_ivec3 em_mul_ivec3_i(em_ivec3 a, INT b) 
{
    return (em_ivec3) {
        a.x * b,
        a.y * b,
        a.z * b
    };
}

em_ivec4 em_mul_ivec4_i(em_ivec4 a, INT b)
{
    return (em_ivec4) {
        a.x * b,
        a.y * b,
        a.z * b,
        a.w * b
    };
}

em_vec2 em_div_vec2(em_vec2 a, em_vec2 b)
{
    return (em_vec2) {
        a.x / b.x, 
        a.y / b.y 
    };
}

em_vec3 em_div_vec3(em_vec3 a, em_vec3 b) 
{
    return (em_vec3) {
        a.x / b.x,
        a.y / b.y,
        a.z / b.z
    };
}

em_vec4 em_div_vec4(em_vec4 a, em_vec4 b)
{
    return (em_vec4) {
        a.x / b.x,
        a.y / b.y,
        a.z / b.z,
        a.w / b.w
    };
}

em_vec2 em_div_vec2_f(em_vec2 a, FLOAT b)
{
    return (em_vec2) {
        a.x / b, 
        a.y / b 
    };
}

em_vec3 em_div_vec3_f(em_vec3 a, FLOAT b) 
{
    return (em_vec3) {
        a.x / b,
        a.y / b,
        a.z / b
    };
}

em_vec4 em_div_vec4_f(em_vec4 a, FLOAT b)
{
    return (em_vec4) {
        a.x / b,
        a.y / b,
        a.z / b,
        a.w / b
    };
}

em_ivec2 em_div_ivec2(em_ivec2 a, em_ivec2 b)
{
    return (em_ivec2) {
        a.x / b.x, 
        a.y / b.y 
    };
}

em_ivec3 em_div_ivec3(em_ivec3 a, em_ivec3 b) 
{
    return (em_ivec3) {
        a.x / b.x,
        a.y / b.y,
        a.z / b.z
    };
}

em_ivec4 em_div_ivec4(em_ivec4 a, em_ivec4 b)
{
    return (em_ivec4) {
        a.x / b.x,
        a.y / b.y,
        a.z / b.z,
        a.w / b.w
    };
}

em_ivec2 em_div_ivec2_i(em_ivec2 a, INT b)
{
    return (em_ivec2) {
        a.x / b, 
        a.y / b 
    };
}

em_ivec3 em_div_ivec3_i(em_ivec3 a, INT b) 
{
    return (em_ivec3) {
        a.x / b,
        a.y / b,
        a.z / b
    };
}

em_ivec4 em_div_ivec4_i(em_ivec4 a, INT b)
{
    return (em_ivec4) {
        a.x / b,
        a.y / b,
        a.z / b,
        a.w / b
    };
}

em_mat4 em_mul_mat4(em_mat4 a, em_mat4 b)
{
    em_mat4 res = {0};

    for (SIZE i = 0; i < 4; i++)
    {
        for (SIZE j = 0; j < 4; j++)
        {
            FLOAT sum = 0.0;
            for (SIZE k = 0; k < 4; k++)
                sum += (a.elements[k][j] * b.elements[i][k]);
            res.elements[i][j] = sum;
        }
    }

    return res;
}

em_mat4 em_mul_mat4_f(em_mat4 a, FLOAT b) 
{
    em_mat4 res = {0};

    for (SIZE i = 0; i < 4; i++)
    {
        for (SIZE j = 0; j < 4; j++)
            res.elements[i][j] = a.elements[i][j] * b;
    }

    return res;
}
em_vec4 em_mul_mat4_v4(em_mat4 a, em_vec4 b)
{
    em_vec4 res = {0};

    for (SIZE i = 0; i < 4; i++)
    {
        FLOAT sum = 0.0;
        for (SIZE j = 0; j < 4; j++)
            sum += a.elements[j][i] * b.elements[j];

        res.elements[i] = sum;
    }

    return res;
}

em_mat4 em_div_mat4_f(em_mat4 a, FLOAT b) 
{
    return em_mul_mat4_f(a, 1.0 / b);
}

em_mat4 em_orthographic(FLOAT l, FLOAT r, FLOAT b, FLOAT t, FLOAT n, FLOAT f)
{
    return (em_mat4) {
        2.0 / (r - l),      0.0,                0.0,                  0.0,
        0.0,                2.0 / (t - b),      0.0,                  0.0,
        0.0,                0.0,                1.0 / (f - n),        0.0,
        // -(r + l) / (r - l), -(t + b) / (t - b), -(f + n) / (f - n),   1.0
        -(r + l) / (r - l), -(t + b) / (t - b), n / (n - f),   1.0
    };
    // em_mat4 res = em_new_mat4_diagonal(1.0);
    //
    // res.elements[0][0] = 2.0 / (r - l);
    // res.elements[1][1] = 2.0 / (t - b);
    // res.elements[2][2] = 1.0 / (f - n);
    // res.elements[3][2] = n / (n - f);
    //
    // return res;
}

// em_mat4 em_orthographic(FLOAT width, FLOAT height, FLOAT near, FLOAT far)
// {
//     em_mat4 res = em_new_mat4_diagonal(1.0);
//
//     res.elements[0][0] = 2.0 / width;
//     res.elements[1][1] = 2.0 / height;
//     res.elements[2][2] = 1.0 / (far - near);
//     res.elements[3][2] = near / (near - far);
//
//     return res;
// }

em_mat4 em_perspective(FLOAT fov_degrees, FLOAT aspect_ratio, FLOAT near, FLOAT far)
{
    em_mat4 res = em_new_mat4_diagonal(1.0);
    FLOAT tan_half_theta = tanf(fov_degrees * (em_PI / 360.0));

    res.elements[0][0] = 1.0 / tan_half_theta;
    res.elements[1][1] = aspect_ratio / tan_half_theta;
    res.elements[2][3] = -1.0;
    res.elements[2][2] = (near + far) / (near - far);
    res.elements[3][2] = (2.0 * near * far) / (near - far);
    res.elements[3][3] = 0.0;

    return res;
}

em_mat4 em_transpose_mat4(em_mat4 matrix)
{
    em_mat4 res = {0};

    for (SIZE i = 0; i < 4; i++)
    {
        for (SIZE j = 0; j < 4; j++)
            res.elements[i][j] = matrix.elements[j][i];
    }

    return res;
}

em_mat4 em_translate_mat4(em_vec3 translation) 
{
    em_mat4 res = em_new_mat4_diagonal(1.0);

    res.elements[3][0] = translation.x;
    res.elements[3][1] = translation.y;
    res.elements[3][2] = translation.z;
    
    return res;
}

em_vec3 em_quaternion_rotate_vec3(em_vec3 vector, em_quaternion rotation)
{
    em_quaternion as_q = em_vec3_to_quaternion(vector);
    em_quaternion inverse = em_inverse_quaternion(rotation);
    em_quaternion w = em_mul_quaternion(em_mul_quaternion(rotation, as_q), inverse);

    return (em_vec3) {w.x, w.y, w.z};
}

em_vec3 em_rotate_vec3(em_vec3 vector, FLOAT angle_degrees, em_vec3 axis)
{
    em_quaternion as_q = em_vec3_to_quaternion(vector);
    em_quaternion rotation = em_quaternion_from_axis_angle(axis, angle_degrees);
    em_quaternion inverse = em_inverse_quaternion(rotation);
    em_quaternion w = em_mul_quaternion(em_mul_quaternion(rotation, as_q), inverse);

    return (em_vec3) {w.x, w.y, w.z};
}

em_mat4 em_rotate_mat4(FLOAT angle_degrees, em_vec3 axis) 
{
    em_mat4 res = em_new_mat4_diagonal(1.0);

    axis = em_normalize_vec3(axis);

    FLOAT sin_theta = sinf(em_deg_to_rad(angle_degrees));
    FLOAT cos_theta = cosf(em_deg_to_rad(angle_degrees));
    FLOAT cos_val = 1.0 - cos_theta;

    res.elements[0][0] = (axis.x * axis.x * cos_val) + cos_theta;
    res.elements[0][1] = (axis.x * axis.y * cos_val) + (axis.z * sin_theta);
    res.elements[0][2] = (axis.x * axis.z * cos_val) - (axis.y * sin_theta);

    res.elements[1][0] = (axis.y * axis.x * cos_val) - (axis.z * sin_theta);
    res.elements[1][1] = (axis.y * axis.y * cos_val) + cos_theta;
    res.elements[1][2] = (axis.y * axis.z * cos_val) + (axis.x * sin_theta);

    res.elements[2][0] = (axis.z * axis.x * cos_val) + (axis.y * sin_theta);
    res.elements[2][1] = (axis.z * axis.y * cos_val) - (axis.x * sin_theta);
    res.elements[2][2] = (axis.z * axis.z * cos_val) + cos_theta;

    return res;
}

em_mat4 em_scale_mat4(em_vec3 scale)
{
    em_mat4 res = em_new_mat4_diagonal(1.0);

    res.elements[0][0] = scale.x;
    res.elements[1][1] = scale.y;
    res.elements[2][2] = scale.z;

    return res;
}

em_mat4 em_inverse_mat4(em_mat4 m)
{
    FLOAT d = em_determinant_mat4(m); 
    if (em_abs(d) < 1.0E-6) 
    {
        EM_LOG("Failed to inverse a matrix\n", NULL);
        return em_new_mat4();
    }

    return (em_mat4) {
        .x = (em_vec4) { 
            (m.y.z * m.z.w * m.w.y - m.y.w * m.z.z * m.w.y + m.y.w * m.z.y * m.w.z - m.y.y * m.z.w * m.w.z - m.y.z * m.z.y * m.w.w + m.y.y * m.z.z * m.w.w) / d,
            (m.x.w * m.z.z * m.w.y - m.x.z * m.z.w * m.w.y - m.x.w * m.z.y * m.w.z + m.x.y * m.z.w * m.w.z + m.x.z * m.z.y * m.w.w - m.x.y * m.z.z * m.w.w) / d,
            (m.x.z * m.y.w * m.w.y - m.x.w * m.y.z * m.w.y + m.x.w * m.y.y * m.w.z - m.x.y * m.y.w * m.w.z - m.x.z * m.y.y * m.w.w + m.x.y * m.y.z * m.w.w) / d,
            (m.x.w * m.y.z * m.z.y - m.x.z * m.y.w * m.z.y - m.x.w * m.y.y * m.z.z + m.x.y * m.y.w * m.z.z + m.x.z * m.y.y * m.z.w - m.x.y * m.y.z * m.z.w) / d 
        },
        .y = (em_vec4) { 
            (m.y.w * m.z.z * m.w.x - m.y.z * m.z.w * m.w.x - m.y.w * m.z.x * m.w.z + m.y.x * m.z.w * m.w.z + m.y.z * m.z.x * m.w.w - m.y.x * m.z.z * m.w.w) / d,
            (m.x.z * m.z.w * m.w.x - m.x.w * m.z.z * m.w.x + m.x.w * m.z.x * m.w.z - m.x.x * m.z.w * m.w.z - m.x.z * m.z.x * m.w.w + m.x.x * m.z.z * m.w.w) / d,
            (m.x.w * m.y.z * m.w.x - m.x.z * m.y.w * m.w.x - m.x.w * m.y.x * m.w.z + m.x.x * m.y.w * m.w.z + m.x.z * m.y.x * m.w.w - m.x.x * m.y.z * m.w.w) / d,
            (m.x.z * m.y.w * m.z.x - m.x.w * m.y.z * m.z.x + m.x.w * m.y.x * m.z.z - m.x.x * m.y.w * m.z.z - m.x.z * m.y.x * m.z.w + m.x.x * m.y.z * m.z.w) / d 
        },
        .z = (em_vec4) { 
            (m.y.y * m.z.w * m.w.x - m.y.w * m.z.y * m.w.x + m.y.w * m.z.x * m.w.y - m.y.x * m.z.w * m.w.y - m.y.y * m.z.x * m.w.w + m.y.x * m.z.y * m.w.w) / d,
            (m.x.w * m.z.y * m.w.x - m.x.y * m.z.w * m.w.x - m.x.w * m.z.x * m.w.y + m.x.x * m.z.w * m.w.y + m.x.y * m.z.x * m.w.w - m.x.x * m.z.y * m.w.w) / d,
            (m.x.y * m.y.w * m.w.x - m.x.w * m.y.y * m.w.x + m.x.w * m.y.x * m.w.y - m.x.x * m.y.w * m.w.y - m.x.y * m.y.x * m.w.w + m.x.x * m.y.y * m.w.w) / d,
            (m.x.w * m.y.y * m.z.x - m.x.y * m.y.w * m.z.x - m.x.w * m.y.x * m.z.y + m.x.x * m.y.w * m.z.y + m.x.y * m.y.x * m.z.w - m.x.x * m.y.y * m.z.w) / d 
        },
        .w = (em_vec4) { 
            (m.y.z * m.z.y * m.w.x - m.y.y * m.z.z * m.w.x - m.y.z * m.z.x * m.w.y + m.y.x * m.z.z * m.w.y + m.y.y * m.z.x * m.w.z - m.y.x * m.z.y * m.w.z) / d,
            (m.x.y * m.z.z * m.w.x - m.x.z * m.z.y * m.w.x + m.x.z * m.z.x * m.w.y - m.x.x * m.z.z * m.w.y - m.x.y * m.z.x * m.w.z + m.x.x * m.z.y * m.w.z) / d,
            (m.x.z * m.y.y * m.w.x - m.x.y * m.y.z * m.w.x - m.x.z * m.y.x * m.w.y + m.x.x * m.y.z * m.w.y + m.x.y * m.y.x * m.w.z - m.x.x * m.y.y * m.w.z) / d,
            (m.x.y * m.y.z * m.z.x - m.x.z * m.y.y * m.z.x + m.x.z * m.y.x * m.z.y - m.x.x * m.y.z * m.z.y - m.x.y * m.y.x * m.z.z + m.x.x * m.y.y * m.z.z) / d 
        } 
    };
}

FLOAT em_determinant_mat4(em_mat4 m)
{
    return m.x.w * m.y.z * m.z.y * m.w.x - 
           m.x.z * m.y.w * m.z.y * m.w.x - 
           m.x.w * m.y.y * m.z.z * m.w.x + 
           m.x.y * m.y.w * m.z.z * m.w.x +
           m.x.z * m.y.y * m.z.w * m.w.x - 
           m.x.y * m.y.z * m.z.w * m.w.x - 
           m.x.w * m.y.z * m.z.x * m.w.y +
           m.x.z * m.y.w * m.z.x * m.w.y +
           m.x.w * m.y.x * m.z.z * m.w.y -
           m.x.x * m.y.w * m.z.z * m.w.y -
           m.x.z * m.y.x * m.z.w * m.w.y + 
           m.x.x * m.y.z * m.z.w * m.w.y +
           m.x.w * m.y.y * m.z.x * m.w.z -
           m.x.y * m.y.w * m.z.x * m.w.z -
           m.x.w * m.y.x * m.z.y * m.w.z +
           m.x.x * m.y.w * m.z.y * m.w.z +
           m.x.y * m.y.x * m.z.w * m.w.z -
           m.x.x * m.y.y * m.z.w * m.w.z -
           m.x.z * m.y.y * m.z.x * m.w.w +
           m.x.y * m.y.z * m.z.x * m.w.w +
           m.x.z * m.y.x * m.z.y * m.w.w -
           m.x.x * m.y.z * m.z.y * m.w.w -
           m.x.y * m.y.x * m.z.z * m.w.w +
           m.x.x * m.y.y * m.z.z * m.w.w;
}

em_mat4 em_look_at(em_vec3 eye, em_vec3 centre, em_vec3 up)
{
    em_mat4 res = {0};

    em_vec3 f = em_normalize_vec3(em_sub_vec3(centre, eye));
    em_vec3 r = em_normalize_vec3(em_cross_vec3(f, up));
    em_vec3 u = em_cross_vec3(r, f);

    res.elements[0][0] = r.x;
    res.elements[0][1] = u.x;
    res.elements[0][2] = -f.x;

    res.elements[1][0] = r.y;
    res.elements[1][1] = u.y;
    res.elements[1][2] = -f.y;

    res.elements[2][0] = r.z;
    res.elements[2][1] = u.z;
    res.elements[2][2] = -f.z;

    res.elements[3][0] = -em_dot_vec3(r, eye);
    res.elements[3][1] = -em_dot_vec3(u, eye);
    res.elements[3][2] = em_dot_vec3(f, eye);
    res.elements[3][3] = 1.0;

    return res;
}

em_quaternion em_conjugate_quaternion(em_quaternion a)
{
    em_quaternion res = {0};

    res.x = -a.x;
    res.y = -a.y;
    res.z = -a.z;
    res.w = a.w;

    return res;
}

em_quaternion em_inverse_quaternion(em_quaternion a)
{
    em_quaternion conjugate = em_conjugate_quaternion(a);
    FLOAT norm_sqr = em_dot_quaternion(a, a);

    return em_div_quaternion_f(conjugate, norm_sqr);
}

em_mat4 em_quaternion_to_mat4(em_quaternion a)
{
    em_mat4 res = em_new_mat4_diagonal(1.0);
    
    em_quaternion norm_quat = em_normalize_quaternion(a);

    FLOAT xx = norm_quat.x * norm_quat.x;
    FLOAT yy = norm_quat.y * norm_quat.y;
    FLOAT zz = norm_quat.z * norm_quat.z;
    FLOAT xy = norm_quat.x * norm_quat.y;
    FLOAT xz = norm_quat.x * norm_quat.z;
    FLOAT yz = norm_quat.y * norm_quat.z;
    FLOAT wx = norm_quat.w * norm_quat.x;
    FLOAT wy = norm_quat.w * norm_quat.y;
    FLOAT wz = norm_quat.w * norm_quat.z;

    res.elements[0][0] = 1.0 - (2.0 * yy) - (2.0 * zz);
    res.elements[0][1] = (2.0 * xy) + (2.0 * wz);
    res.elements[0][2] = (2.0 * xz) - (2.0 * wy);

    res.elements[1][0] = (2.0 * xy) - (2.0 * wz);
    res.elements[1][1] = 1.0 - (2.0 * xx) - (2.0 * zz);
    res.elements[1][2] = (2.0 * yz) + (2.0 * wx);

    res.elements[2][0] = (2.0 * xz) + (2.0 * wy);
    res.elements[2][1] = (2.0 * yz) - (2.0 * wx);
    res.elements[2][2] = 1.0 - (2.0 * xx) - (2.0 * yy);

    return res;
}

em_quaternion em_vec3_to_quaternion(em_vec3 a)
{
    return (em_quaternion) {a.x, a.y, a.z, 0.0};
}

em_quaternion em_quaternion_from_axis_angle(em_vec3 axis, FLOAT angle_degrees)
{
    em_quaternion res = {0};

    axis = em_normalize_vec3(axis);
    FLOAT half_theta = em_deg_to_rad(angle_degrees) / 2.0;
    FLOAT sin_rot = sinf(half_theta);
    res.x = axis.x * sin_rot;
    res.y = axis.y * sin_rot;
    res.z = axis.z * sin_rot;
    res.w = cosf(half_theta);

    return res;
}

em_quaternion em_add_quaternion(em_quaternion a, em_quaternion b)
{
    return em_add_vec4(a, b);
}

em_quaternion em_sub_quaternion(em_quaternion a, em_quaternion b)
{
    return em_sub_vec4(a, b);
}

em_quaternion em_mul_quaternion(em_quaternion a, em_quaternion b)
{
    return (em_quaternion) {
        (a.w * b.x) + (a.x * b.w) + (a.y * b.z) - (a.z * b.y),
        (a.w * b.y) - (a.x * b.z) + (a.y * b.w) + (a.z * b.x),
        (a.w * b.z) + (a.x * b.y) - (a.y * b.x) + (a.z * b.w),
        (a.w * b.w) - (a.x * b.x) - (a.y * b.y) - (a.z * b.z),
    };
}

em_quaternion em_mul_quaternion_f(em_quaternion a, FLOAT b)
{
    return em_mul_vec4_f(a, b);
}

em_quaternion em_mul_quaternion_vec3(em_quaternion a, em_vec3 b)
{
    return (em_quaternion) {
        (a.w * b.x) + (a.y * b.z) - (a.z * b.y),
        (a.w * b.y) + (a.z * b.x) - (a.x * b.z),
        (a.w * b.z) + (a.x * b.y) - (a.y * b.x),
        -(a.x * b.x) - (a.y * b.y) - (a.z * b.z)
    };
}

em_quaternion em_div_quaternion(em_quaternion a, em_quaternion b)
{
    return em_div_vec4(a, b);
}

em_quaternion em_div_quaternion_f(em_quaternion a, FLOAT b)
{
    return em_div_vec4_f(a, b);
}

em_quaternion em_normalize_quaternion(em_quaternion a)
{
    return em_normalize_vec4(a);
}

FLOAT em_dot_quaternion(em_quaternion a, em_quaternion b)
{
    return em_dot_vec4(a, b);
}

FLOAT em_interval_size(em_interval i)
{
    return i.max - i.min;
}

INT em_iinterval_size(em_iinterval i)
{
    return i.max - i.min;
}

itvl em_interval_around(FLOAT value, FLOAT size)
{
    return (em_interval) { .min = value - size / 2.0, .max = value + size / 2.0 };
}

iitvl em_iinterval_around(INT value, INT size)
{
    return (em_iinterval) { .min = value - size / 2, .max = value + size / 2 };
}

bool em_interval_contains(em_interval i, FLOAT value)
{
    return value >= i.min && value <= i.max;
}

bool em_iinterval_contains(em_iinterval i, INT value)
{
    return value >= i.min && value <= i.max;
}

bool em_interval_surrounds(em_interval i, FLOAT value)
{
    return value > i.min && value < i.max;
}

bool em_iinterval_surrounds(em_iinterval i, INT value)
{
    return value > i.min && value < i.max;
}

#endif // EM_MATH_IMPL

#endif
