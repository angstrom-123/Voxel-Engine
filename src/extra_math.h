#ifndef EXTRA_MATH_H
#define EXTRA_MATH_H

#define ALIAS_TYPES true

#include <math.h> // Trigonometry
#include <stdbool.h>

#define em_PI acosf(-1.0)

#define em_deg_to_rad(t) (t * (em_PI / 180.0))
#define em_rad_to_deg(t) (t * (180.0 / em_PI))

#define em_clamp(x, mi, ma) ((x < mi) ? mi : ((x > ma) ? ma : x))
#define em_min(x, y) ((x) > (y) ? (y) : (x))
#define em_max(x, y) ((x) < (y) ? (y) : (x))
#define em_absf(x) ((x) < 0.0 ? -(x) : (x))
#define em_abs(x) ((x) < 0 ? -(x) : (x))
#define em_sqr(x) ((x) * (x))

typedef union em_vec2 {
	struct {
		float x, y;
	};

	float elements[2];
} em_vec2;

typedef union em_vec3 {
	struct {
		float x, y, z;
	};

	float elements[3];
} em_vec3;

typedef union em_vec4 {
	struct {
		float x, y, z, w;
	};

	float elements[4];
} em_vec4;

typedef union em_ivec2 {
	struct {
		int x, y;
	};

	int elements[2];
} em_ivec2;

typedef union em_ivec3 {
	struct {
		int x, y, z;
	};

	int elements[3];
} em_ivec3;

typedef union em_ivec4 {
	struct {
		int x, y, z, w;
	};

	int elements[4];
} em_ivec4;

typedef union em_uvec2 {
	struct {
		unsigned x, y;
	};

	unsigned elements[2];
} em_uvec2;

typedef union em_uvec3 {
	struct {
		unsigned x, y, z;
	};

	unsigned elements[3];
} em_uvec3;

typedef union em_uvec4 {
	struct {
		unsigned x, y, z, w;
	};

	unsigned elements[4];
} em_uvec4;

typedef struct em_mat4 {
	float elements[4][4];
} em_mat4;

typedef em_vec4 em_quaternion;

#if ALIAS_TYPES
typedef em_quaternion quaternion;

typedef em_vec2 vec2;
typedef em_vec3 vec3;
typedef em_vec4 vec4;

typedef em_ivec2 ivec2;
typedef em_ivec3 ivec3;
typedef em_ivec4 ivec4;

typedef em_uvec2 uvec2;
typedef em_uvec3 uvec3;
typedef em_uvec4 uvec4;

typedef em_mat4 mat4;
#endif

extern bool em_equals_ivec3(em_ivec3 a, em_ivec3 b);

extern em_vec2 em_ivec2_as_vec2(em_ivec2 a);
extern em_vec3 em_ivec3_as_vec3(em_ivec3 a);
extern em_vec4 em_ivec3_as_vec4(em_ivec4 a);
extern em_ivec2 em_vec2_as_ivec2(em_vec2 a);
extern em_ivec3 em_vec3_as_ivec3(em_vec3 a);
extern em_ivec4 em_vec3_as_ivec4(em_vec4 a);

extern em_vec4 em_new_vec4_vec3_f(em_vec3 xyz, float w);
extern em_mat4 em_new_mat4(void);
extern em_mat4 em_new_mat4_diagonal(float diagonal);
extern em_quaternion em_new_quaternion_vec3_f(em_vec3 xyz, float w);

extern em_vec2 em_normalize_vec2(em_vec2 a);
extern em_vec3 em_normalize_vec3(em_vec3 a);
extern em_vec4 em_normalize_vec4(em_vec4 a);

extern float em_length_vec2(em_vec2 a);
extern float em_length_vec3(em_vec3 a);
extern float em_length_vec4(em_vec4 a);

extern float em_length_squared_vec2(em_vec2 a);
extern float em_length_squared_vec3(em_vec3 a);
extern float em_length_squared_vec4(em_vec4 a);

extern float em_dot_vec2(em_vec2 a, em_vec2 b);
extern float em_dot_vec3(em_vec3 a, em_vec3 b);
extern float em_dot_vec4(em_vec4 a, em_vec4 b);
extern float em_dot_ivec2(em_ivec2 a, em_ivec2 b);
extern float em_dot_ivec3(em_ivec3 a, em_ivec3 b);
extern float em_dot_ivec4(em_ivec4 a, em_ivec4 b);

extern em_vec3 em_cross_vec3(em_vec3 a, em_vec3 b);

extern em_vec2 em_add_vec2(em_vec2 a, em_vec2 b);
extern em_vec3 em_add_vec3(em_vec3 a, em_vec3 b);
extern em_vec4 em_add_vec4(em_vec4 a, em_vec4 b);
extern em_ivec2 em_add_ivec2(em_ivec2 a, em_ivec2 b);
extern em_ivec3 em_add_ivec3(em_ivec3 a, em_ivec3 b);
extern em_ivec4 em_add_ivec4(em_ivec4 a, em_ivec4 b);

extern em_vec2 em_sub_vec2(em_vec2 a, em_vec2 b);
extern em_vec3 em_sub_vec3(em_vec3 a, em_vec3 b);
extern em_vec4 em_sub_vec4(em_vec4 a, em_vec4 b);
extern em_ivec2 em_sub_ivec2(em_ivec2 a, em_ivec2 b);
extern em_ivec3 em_sub_ivec3(em_ivec3 a, em_ivec3 b);
extern em_ivec4 em_sub_ivec4(em_ivec4 a, em_ivec4 b);

extern em_vec2 em_mul_vec2(em_vec2 a, em_vec2 b);
extern em_vec3 em_mul_vec3(em_vec3 a, em_vec3 b);
extern em_vec4 em_mul_vec4(em_vec4 a, em_vec4 b);
extern em_vec2 em_mul_vec2_f(em_vec2 a, float b);
extern em_vec3 em_mul_vec3_f(em_vec3 a, float b);
extern em_vec4 em_mul_vec4_f(em_vec4 a, float b);
extern em_ivec2 em_mul_ivec2(em_ivec2 a, em_ivec2 b);
extern em_ivec3 em_mul_ivec3(em_ivec3 a, em_ivec3 b);
extern em_ivec4 em_mul_ivec4(em_ivec4 a, em_ivec4 b);
extern em_ivec2 em_mul_ivec2_i(em_ivec2 a, int b);
extern em_ivec3 em_mul_ivec3_i(em_ivec3 a, int b);
extern em_ivec4 em_mul_ivec4_i(em_ivec4 a, int b);

extern em_vec2 em_div_vec2(em_vec2 a, em_vec2 b);
extern em_vec3 em_div_vec3(em_vec3 a, em_vec3 b);
extern em_vec4 em_div_vec4(em_vec4 a, em_vec4 b);
extern em_vec2 em_div_vec2_f(em_vec2 a, float b);
extern em_vec3 em_div_vec3_f(em_vec3 a, float b);
extern em_vec4 em_div_vec4_f(em_vec4 a, float b);
extern em_ivec2 em_div_ivec2(em_ivec2 a, em_ivec2 b);
extern em_ivec3 em_div_ivec3(em_ivec3 a, em_ivec3 b);
extern em_ivec4 em_div_ivec4(em_ivec4 a, em_ivec4 b);
extern em_ivec2 em_div_ivec2_i(em_ivec2 a, int b);
extern em_ivec3 em_div_ivec3_i(em_ivec3 a, int b);
extern em_ivec4 em_div_ivec4_i(em_ivec4 a, int b);

extern em_mat4 em_mul_mat4(em_mat4 a, em_mat4 b);
extern em_mat4 em_mul_mat4_f(em_mat4 a, float b);
extern em_vec4 em_mul_mat4_v4(em_mat4 a, em_vec4 b);

extern em_mat4 em_div_mat4_f(em_mat4 a, float b);

extern em_mat4 em_perspective(float fov_degrees, float aspect_ratio, float near, float far);
extern em_mat4 em_transpose_mat4(em_mat4 matrix);
extern em_mat4 em_translate_mat4(em_vec3 translation);
extern em_vec3 em_quaternion_rotate_vec3(em_vec3 vector, em_quaternion rotation);
extern em_vec3 em_rotate_vec3(em_vec3 vector, float angle_degrees, em_vec3 axis);
extern em_mat4 em_rotate_mat4(float angle_degrees, em_vec3 axis);
extern em_mat4 em_scale_mat4(em_vec3 scale);
extern em_mat4 em_look_at(em_vec3 eye, em_vec3 centre, em_vec3 up);

extern em_quaternion em_conjugate_quaternion(em_quaternion a);
extern em_quaternion em_inverse_quaternion(em_quaternion a);
extern em_mat4 em_quaternion_to_mat4(em_quaternion a);
extern em_quaternion em_vec3_to_quaternion(em_vec3 a);
extern em_quaternion em_quaternion_from_axis_angle(em_vec3 axis, float angle_degrees);

extern em_quaternion em_add_quaternion(em_quaternion a, em_quaternion b);
extern em_quaternion em_sub_quaternion(em_quaternion a, em_quaternion b);
extern em_quaternion em_mul_quaternion(em_quaternion a, em_quaternion b);
extern em_quaternion em_mul_quaternion_f(em_quaternion a, float b);
extern em_quaternion em_mul_quaternion_vec3(em_quaternion a, em_vec3 b);
extern em_quaternion em_div_quaternion(em_quaternion a, em_quaternion b);
extern em_quaternion em_div_quaternion_f(em_quaternion a, float b);
extern em_quaternion em_normalize_quaternion(em_quaternion a);
extern float em_dot_quaternion(em_quaternion a, em_quaternion b);

#endif
