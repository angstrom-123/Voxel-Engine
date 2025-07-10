#include "extra_math.h"

bool em_equals_ivec3(em_ivec3 a, em_ivec3 b)
{
	return (a.x == b.x) && (a.y == b.y) && (a.z == b.z);
}

em_vec4 em_new_vec4_vec3_f(em_vec3 xyz, float w)
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

em_quaternion em_new_quaternion_vec3_f(em_vec3 xyz, float w)
{
	return (em_quaternion) {xyz.x, xyz.y, xyz.z, w};
}

em_vec2 em_normalize_vec2(em_vec2 a) 
{
	float len = em_length_vec2(a);
	if (len > 1.0E-6)
		return em_div_vec2_f(a, len);
	return a;
}

em_vec3 em_normalize_vec3(em_vec3 a)
{
	float len = em_length_vec3(a);
	if (len > 1.0E-6)
		return em_div_vec3_f(a, len);
	return a;
}

em_vec4 em_normalize_vec4(em_vec4 a)
{
	float len = em_length_vec4(a);
	if (len > 1.0E-6)
		return em_div_vec4_f(a, len);
	return a;
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
	return em_dot_vec2(a, a);
}

float em_length_squared_vec3(em_vec3 a)
{
	return em_dot_vec3(a, a);
}

float em_length_squared_vec4(em_vec4 a)
{
	return em_dot_vec4(a, a);
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

em_vec2 em_mul_vec2_f(em_vec2 a, float b)
{
	return (em_vec2) {
		a.x * b, 
		a.y * b 
	};
}

em_vec3 em_mul_vec3_f(em_vec3 a, float b) 
{
	return (em_vec3) {
		a.x * b,
		a.y * b,
		a.z * b
	};
}

em_vec4 em_mul_vec4_f(em_vec4 a, float b)
{
	return (em_vec4) {
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

em_vec2 em_div_vec2_f(em_vec2 a, float b)
{
	return (em_vec2) {
		a.x / b, 
		a.y / b 
	};
}

em_vec3 em_div_vec3_f(em_vec3 a, float b) 
{
	return (em_vec3) {
		a.x / b,
		a.y / b,
		a.z / b
	};
}

em_vec4 em_div_vec4_f(em_vec4 a, float b)
{
	return (em_vec4) {
		a.x / b,
		a.y / b,
		a.z / b,
		a.w / b
	};
}

em_mat4 em_mul_mat4(em_mat4 a, em_mat4 b)
{
	em_mat4 res = {0};

	for (unsigned i = 0; i < 4; i++)
	{
		for (unsigned j = 0; j < 4; j++)
		{
			float sum = 0.0;
			for (unsigned k = 0; k < 4; k++)
			{
				sum += (a.elements[k][j] * b.elements[i][k]);
			}
			res.elements[i][j] = sum;
		}
	}

	return res;
}

em_mat4 em_mul_mat4_f(em_mat4 a, float b) 
{
	em_mat4 res = {0};

	for (unsigned i = 0; i < 4; i++)
	{
		for (unsigned j = 0; j < 4; j++)
		{
			res.elements[i][j] = a.elements[i][j] * b;
		}
	}

	return res;
}
em_vec4 em_mul_mat4_v4(em_mat4 a, em_vec4 b)
{
	em_vec4 res = {0};

	for (unsigned i = 0; i < 4; i++)
	{
		float sum = 0.0;
		for (unsigned j = 0; j < 4; j++)
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
	float tan_half_theta = tanf(fov_degrees * (em_PI / 360.0));

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

	for (unsigned i = 0; i < 4; i++)
	{
		for (unsigned j = 0; j < 4; j++)
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

em_vec3 em_quaternion_rotate_vec3(em_vec3 vector, em_quaternion rotation)
{
	em_quaternion as_q = em_vec3_to_quaternion(vector);
	em_quaternion inverse = em_inverse_quaternion(rotation);
	em_quaternion w = em_mul_quaternion(em_mul_quaternion(rotation, as_q), inverse);

	return (em_vec3) {w.x, w.y, w.z};
}

em_vec3 em_rotate_vec3(em_vec3 vector, float angle_degrees, em_vec3 axis)
{
	em_quaternion as_q = em_vec3_to_quaternion(vector);
	em_quaternion rotation = em_quaternion_from_axis_angle(axis, angle_degrees);
	em_quaternion inverse = em_inverse_quaternion(rotation);
	em_quaternion w = em_mul_quaternion(em_mul_quaternion(rotation, as_q), inverse);

	return (em_vec3) {w.x, w.y, w.z};
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
	float norm_sqr = em_dot_quaternion(a, a);

	return em_div_quaternion_f(conjugate, norm_sqr);
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

em_quaternion em_quaternion_from_axis_angle(em_vec3 axis, float angle_degrees)
{
	em_quaternion res = {0};

	axis = em_normalize_vec3(axis);
	float half_theta = em_deg_to_rad(angle_degrees) / 2.0;
	float sin_rot = sinf(half_theta);
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

em_quaternion em_mul_quaternion_f(em_quaternion a, float b)
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
