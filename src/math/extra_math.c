#include "extra_math.h"

em_vec2 em_new_vec2(float x, float y) 
{
	return (em_vec2) {
		.x = x,
		.y = y,
	};
}

em_vec3 em_new_vec3(float x, float y, float z)
{
	return (em_vec3) {
		.x = x,
		.y = y,
		.z = z
	};
}

em_vec4 em_new_vec4(float x, float y, float z, float w)
{
	return (em_vec4) {
		.x = x,
		.y = y,
		.z = z,
		.w = w
	};
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

em_mat4 em_new_mat4_diagonal(float diagonal) 
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

em_vec2 em_normalize_vec2(em_vec2 a) 
{
	return em_div_vec2_f(a, em_length_vec2(a));
}

em_vec3 em_normalize_vec3(em_vec3 a)
{
	return em_div_vec3_f(a, em_length_vec3(a));
}

em_vec4 em_normalize_vec4(em_vec4 a)
{
	return em_div_vec4_f(a, em_length_vec4(a));
}

float em_length_vec2(em_vec2 a) 
{
	return sqrtf(em_length_squared_vec2(a));
}

float em_length_vec3(em_vec3 a)
{
	return sqrtf(em_length_squared_vec3(a));
}

float em_length_vec4(em_vec4 a)
{
	return sqrtf(em_length_squared_vec4(a));
}

float em_length_squared_vec2(em_vec2 a)
{
	return em_sqr(a.x) + em_sqr(a.y);
}

float em_length_squared_vec3(em_vec3 a)
{
	return em_sqr(a.x) + em_sqr(a.y) + em_sqr(a.z);
}

float em_length_squared_vec4(em_vec4 a)
{
	return em_sqr(a.x) + em_sqr(a.y) + em_sqr(a.z) + em_sqr(a.w);
}

float em_dot_vec2(em_vec2 a, em_vec2 b) 
{
	return (a.x * b.x) + (a.y * b.y);
}

float em_dot_vec3(em_vec3 a, em_vec3 b)
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

float em_dot_vec4(em_vec4 a, em_vec4 b)
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}

em_vec3 em_cross_vec3(em_vec3 a, em_vec3 b)
{
	return (em_vec3) {
		.x = (a.y * b.z) - (a.z * b.y),
		.y = (a.z * b.x) + (a.x - b.z),
		.z = (a.x * b.y) - (a.y * b.x)
	};
}

em_vec2 em_add_vec2(em_vec2 a, em_vec2 b)
{
	return (em_vec2) {
		.x = a.x + b.x, 
		.y = a.y + b.y 
	};
}

em_vec3 em_add_vec3(em_vec3 a, em_vec3 b) 
{
	return (em_vec3) {
		.x = a.x + b.x,
		.y = a.y + b.y,
		.z = a.z + b.z
	};
}

em_vec4 em_add_vec4(em_vec4 a, em_vec4 b)
{
	return (em_vec4) {
		.x = a.x + b.x,
		.y = a.y + b.y,
		.z = a.z + b.z,
		.w = a.w + b.w
	};
}


em_vec2 em_sub_vec2(em_vec2 a, em_vec2 b)
{
	return (em_vec2) {
		.x = a.x - b.x, 
		.y = a.y - b.y 
	};
}

em_vec3 em_sub_vec3(em_vec3 a, em_vec3 b) 
{
	return (em_vec3) {
		.x = a.x - b.x,
		.y = a.y - b.y,
		.z = a.z - b.z
	};
}

em_vec4 em_sub_vec4(em_vec4 a, em_vec4 b)
{
	return (em_vec4) {
		.x = a.x - b.x,
		.y = a.y - b.y,
		.z = a.z - b.z,
		.w = a.w - b.w
	};
}

em_vec2 em_mul_vec2(em_vec2 a, em_vec2 b)
{
	return (em_vec2) {
		.x = a.x * b.x, 
		.y = a.y * b.y 
	};
}

em_vec3 em_mul_vec3(em_vec3 a, em_vec3 b) 
{
	return (em_vec3) {
		.x = a.x * b.x,
		.y = a.y * b.y,
		.z = a.z * b.z
	};
}

em_vec4 em_mul_vec4(em_vec4 a, em_vec4 b)
{
	return (em_vec4) {
		.x = a.x * b.x,
		.y = a.y * b.y,
		.z = a.z * b.z,
		.w = a.w * b.w
	};
}

em_vec2 em_mul_vec2_f(em_vec2 a, float b)
{
	return (em_vec2) {
		.x = a.x * b, 
		.y = a.y * b 
	};
}

em_vec3 em_mul_vec3_f(em_vec3 a, float b) 
{
	return (em_vec3) {
		.x = a.x * b,
		.y = a.y * b,
		.z = a.z * b
	};
}

em_vec4 em_mul_vec4_f(em_vec4 a, float b)
{
	return (em_vec4) {
		.x = a.x * b,
		.y = a.y * b,
		.z = a.z * b,
		.w = a.w * b
	};
}

em_vec2 em_div_vec2(em_vec2 a, em_vec2 b)
{
	return (em_vec2) {
		.x = a.x / b.x, 
		.y = a.y / b.y 
	};
}

em_vec3 em_div_vec3(em_vec3 a, em_vec3 b) 
{
	return (em_vec3) {
		.x = a.x / b.x,
		.y = a.y / b.y,
		.z = a.z / b.z
	};
}

em_vec4 em_div_vec4(em_vec4 a, em_vec4 b)
{
	return (em_vec4) {
		.x = a.x / b.x,
		.y = a.y / b.y,
		.z = a.z / b.z,
		.w = a.w / b.w
	};
}

em_vec2 em_div_vec2_f(em_vec2 a, float b)
{
	return (em_vec2) {
		.x = a.x / b, 
		.y = a.y / b 
	};
}

em_vec3 em_div_vec3_f(em_vec3 a, float b) 
{
	return (em_vec3) {
		.x = a.x / b,
		.y = a.y / b,
		.z = a.z / b
	};
}

em_vec4 em_div_vec4_f(em_vec4 a, float b)
{
	return (em_vec4) {
		.x = a.x / b,
		.y = a.y / b,
		.z = a.z / b,
		.w = a.w / b
	};
}

em_mat4 em_mul_mat4(em_mat4 a, em_mat4 b)
{
	em_mat4 res;

	for (size_t i = 0; i < 4; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			float sum = 0.0;
			for (size_t k = 0; k < 4; k++)
			{
				sum += a.elements[i][k] * b.elements[k][j];
			}
			res.elements[i][j] = sum;
		}
	}

	return res;
}

em_mat4 em_mul_mat4_f(em_mat4 a, float b) 
{
	em_mat4 res;

	for (size_t i = 0; i < 4; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			res.elements[i][j] = a.elements[i][j] * b;
		}
	}

	return res;
}
em_vec4 em_mul_mat4_v4(em_mat4 a, em_vec4 b)
{
	em_vec4 res;

	for (size_t i = 0; i < 4; i++)
	{
		float sum = 0.0;
		for (size_t j = 0; j < 4; j++)
		{
			sum += a.elements[j][i] * b.elements[j];
		}

		res.elements[i] = sum;
	}

	return res;
}

em_mat4 em_div_mat4_f(em_mat4 a, float b) 
{
	return em_mul_mat4_f(a, 1.0 / b);
}

em_mat4 em_perspective(float fov_degrees, float aspect_ratio, float near, float far)
{
	em_mat4 res = em_new_mat4_diagonal(1.0);
	float half_tan_theta = tanf(fov_degrees * (em_PI / 360.0));

	res.elements[0][0] = 1.0 / half_tan_theta;
	res.elements[1][1] = aspect_ratio / half_tan_theta;
	res.elements[2][3] = -1.0;
	res.elements[2][2] = (near + far) / (near - far);
	res.elements[3][2] = (2.0 * near * far) / (near - far);
	res.elements[3][3] = 0.0;

	return res;
}

em_mat4 em_transpose_mat4(em_mat4 matrix)
{
	em_mat4 res;

	for (size_t i = 0; i < 4; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			res.elements[i][j] = matrix.elements[j][i];
		}
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

em_mat4 em_rotate_mat4(float angle_degrees, em_vec3 axis) 
{
	em_mat4 res = em_new_mat4_diagonal(1.0);

	axis = em_normalize_vec3(axis);

	float sin_theta = sinf(em_deg_to_rad(angle_degrees));
	float cos_theta = cosf(em_deg_to_rad(angle_degrees));
	float cos_val = 1.0 - cos_theta;

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

em_mat4 em_look_at(em_vec3 eye, em_vec3 centre, em_vec3 up)
{
	em_mat4 res;

	em_vec3 f = em_normalize_vec3(em_sub_vec3(centre, eye));
	em_vec3 s = em_normalize_vec3(em_cross_vec3(f, up));
	em_vec3 u = em_cross_vec3(s, f);

	res.elements[0][0] = s.x;
	res.elements[0][1] = u.x;
	res.elements[0][2] = -f.x;

	res.elements[1][0] = s.y;
	res.elements[1][1] = u.y;
	res.elements[1][2] = -f.y;

	res.elements[2][0] = s.z;
	res.elements[2][1] = u.z;
	res.elements[2][2] = -f.z;

	res.elements[3][0] = -em_dot_vec3(s, eye);
	res.elements[3][1] = -em_dot_vec3(u, eye);
	res.elements[3][2] = em_dot_vec3(f, eye);
	res.elements[3][3] = 1.0;

	return res;
}

em_quaternion em_inverse_quaternion(em_quaternion a)
{
	em_quaternion conjugate;
	em_quaternion res;

	conjugate.x = -a.x;
	conjugate.y = -a.y;
	conjugate.z = -a.z;
	conjugate.w = a.w;

	float norm = sqrtf(em_dot_quaternion(a, a));
	float norm_sqr = em_sqr(norm);

	res.x = conjugate.x / norm_sqr;
	res.y = conjugate.y / norm_sqr;
	res.z = conjugate.z / norm_sqr;
	res.w = conjugate.w / norm_sqr;

	return res;
}

em_mat4 em_quaternion_to_mat4(em_quaternion a)
{
	em_mat4 res = em_new_mat4_diagonal(1.0);
	
	em_quaternion norm_quat = em_normalize_quaternion(a);

	float xx = norm_quat.x * norm_quat.x;
	float yy = norm_quat.y * norm_quat.y;
	float zz = norm_quat.z * norm_quat.z;
	float xy = norm_quat.x * norm_quat.y;
	float xz = norm_quat.x * norm_quat.z;
	float yz = norm_quat.y * norm_quat.z;
	float wx = norm_quat.w * norm_quat.x;
	float wy = norm_quat.w * norm_quat.y;
	float wz = norm_quat.w * norm_quat.z;

	res.elements[0][0] = 1.0 - (2.0 * (yy + zz));
	res.elements[0][1] = 2.0 * (xy + wz);
	res.elements[0][2] = 2.0 * (xz - wy);

	res.elements[1][0] = 2.0 * (xy - wz);
	res.elements[1][1] = 1.0 - (2.0 * (xx + zz));
	res.elements[1][2] = 2.0 * (yz + wx);

	res.elements[2][0] = 2.0 * (xz + wy);
	res.elements[2][1] = 2.0 * (yz - wx);
	res.elements[2][2] = 1.0 - (2.0 * (xx + yy));

	return res;
}

em_quaternion em_quaternion_from_axis_angle(em_vec3 axis, float angle_degrees)
{
	em_quaternion res;

	float axis_norm = sqrtf(em_dot_vec3(axis, axis));
	float sin_rot = sinf(angle_degrees / 2.0);
	em_vec3 rotated = em_mul_vec3_f(axis, sin_rot);
	em_vec3 xyz = em_div_vec3_f(rotated, axis_norm);

	res.x = xyz.x;
	res.y = xyz.y;
	res.z = xyz.z;
	res.w = cosf(angle_degrees / 2.0);

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
	return em_mul_vec4(a, b);
}

em_quaternion em_mul_quaternion_f(em_quaternion a, float b)
{
	return em_mul_vec4_f(a, b);
}

em_quaternion em_div_quaternion(em_quaternion a, em_quaternion b)
{
	return em_div_vec4(a, b);
}

em_quaternion em_div_quaternion_f(em_quaternion a, float b)
{
	return em_div_vec4_f(a, b);
}

em_quaternion em_normalize_quaternion(em_quaternion a)
{
	return em_normalize_vec4(a);
}

float em_dot_quaternion(em_quaternion a, em_quaternion b)
{
	return em_dot_vec4(a, b);
}
