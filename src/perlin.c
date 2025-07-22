#include "perlin.h"
#include "src/romu_random.h"

vec2 _grad_2d(int32_t x, int32_t y)
{
	romu_mono32_init(em_abs(perlin_pair(x, y)));
	float x_01 = (float) romu_mono32_random() / (float) MONO32_MAX;
	float y_01 = (float) romu_mono32_random() / (float) MONO32_MAX;

	float x_cmp = (2 * x_01) - 1.0;
	float y_cmp = (2 * y_01) - 1.0;

	return em_normalize_vec2((vec2) {x_cmp, y_cmp});
}

vec3 _grad_3d(int32_t x, int32_t y, int32_t z)
{
	romu_mono32_init(em_abs(perlin_pair(x, perlin_pair(y, z))));
	float x_01 = (float) romu_mono32_random() / (float) MONO32_MAX;
	float y_01 = (float) romu_mono32_random() / (float) MONO32_MAX;
	float z_01 = (float) romu_mono32_random() / (float) MONO32_MAX;

	float x_cmp = (2 * x_01) - 1.0;
	float y_cmp = (2 * y_01) - 1.0;
	float z_cmp = (2 * z_01) - 1.0;

	return em_normalize_vec3((vec3) {x_cmp, y_cmp, z_cmp});
}

float _fade(float t)
{
	return t * t * t * (t * (t * 6 - 15.0) + 10.0);
}

float _lerp(float t, float a, float b)
{
	return a + t * (b - a);
}

int32_t perlin_hash(int32_t x)
{
	x = ~x + (x << 15);
	x = x ^ (x >> 12u);
	x = x + (x << 2);
	x = x ^ (x >> 4u);
	x = x * 2057;
	x = x ^ (x >> 16u);
	return x;
}

int32_t perlin_pair(int32_t a, int32_t b)
{
	if (a >= 0) a = 2 * a;
	else a = -2 * a - 1;

	if (b >= 0) b = 2 * b;
	else b = -2 * b - 1;

	return (a >= b) 
		? a * a + b 
		: b * b + a;
}

float perlin_2d(float x, float y, float freq)
{
	x *= freq;
	y *= freq;

	/* Decimal part within grid cell. */
	const float x_f = x - floorf(x);
	const float y_f = y - floorf(y);

	/* Integer part describing the grid cell. */
	const int32_t x_i = (int32_t) floorf(x);
	const int32_t y_i = (int32_t) floorf(y);

	/* Position vectors of grid cell corners. */
	const ivec2 p0 = {x_i, y_i};
	const ivec2 p1 = {x_i + 1, y_i};
	const ivec2 p2 = {x_i, y_i + 1};
	const ivec2 p3 = {x_i + 1, y_i + 1};

	/* Gradients for each corner. */
	const vec2 g0 = _grad_2d(p0.x, p0.y);
	const vec2 g1 = _grad_2d(p1.x, p1.y);
	const vec2 g2 = _grad_2d(p2.x, p2.y);
	const vec2 g3 = _grad_2d(p3.x, p3.y);

	/* Offset vectors from each corner to the candidate point. */
	const vec2 o0 = em_sub_vec2((vec2) {x, y}, (vec2) {p0.x, p0.y});
	const vec2 o1 = em_sub_vec2((vec2) {x, y}, (vec2) {p1.x, p1.y});
	const vec2 o2 = em_sub_vec2((vec2) {x, y}, (vec2) {p2.x, p2.y});
	const vec2 o3 = em_sub_vec2((vec2) {x, y}, (vec2) {p3.x, p3.y});

	/* Gradient weights. */
	const float d0 = em_dot_vec2(g0, o0);
	const float d1 = em_dot_vec2(g1, o1);
	const float d2 = em_dot_vec2(g2, o2);
	const float d3 = em_dot_vec2(g3, o3);

	/* Lerp constants using smoothstep. */
	const float u = _fade(x_f);
	const float v = _fade(y_f);

	/* Interpolate gradients. */
	const float x0 = _lerp(u, d0, d1);
	const float x1 = _lerp(u, d2, d3);

	const float y0 = _lerp(v, x0, x1);

	return y0;
}

float perlin_octave_2d(float x, float y, uint8_t num)
{
	float res = 0.0;
	float amp = 1.0;
	float freq = 0.005;

	for (size_t i = 0; i < num; i++)
	{
		res += amp * perlin_2d(x, y, freq);
		amp /= 2.0;
		freq *= 2.0;
	}

	return res;
}

float perlin_3d(float x, float y, float z, float freq)
{
	x *= freq;
	y *= freq;
	z *= freq;

	/* Decimal part within grid cell. */
	const float x_f = x - floorf(x);
	const float y_f = y - floorf(y);
	const float z_f = z - floorf(z);

	/* Integer part describing the grid cell. */
	const int32_t x_i = (int32_t) floorf(x);
	const int32_t y_i = (int32_t) floorf(y);
	const int32_t z_i = (int32_t) floorf(z);

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
	const vec3 g0 = _grad_3d(p0.x, p0.y, p0.z);
	const vec3 g1 = _grad_3d(p1.x, p1.y, p1.z);
	const vec3 g2 = _grad_3d(p2.x, p2.y, p2.z);
	const vec3 g3 = _grad_3d(p3.x, p3.y, p3.z);
	const vec3 g4 = _grad_3d(p4.x, p4.y, p4.z);
	const vec3 g5 = _grad_3d(p5.x, p5.y, p5.z);
	const vec3 g6 = _grad_3d(p6.x, p6.y, p6.z);
	const vec3 g7 = _grad_3d(p7.x, p7.y, p7.z);

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
	const float d0 = em_dot_vec3(g0, o0);
	const float d1 = em_dot_vec3(g1, o1);
	const float d2 = em_dot_vec3(g2, o2);
	const float d3 = em_dot_vec3(g3, o3);
	const float d4 = em_dot_vec3(g4, o4);
	const float d5 = em_dot_vec3(g5, o5);
	const float d6 = em_dot_vec3(g6, o6);
	const float d7 = em_dot_vec3(g7, o7);

	/* Lerp constants using smoothstep. */
	const float u = _fade(x_f);
	const float v = _fade(y_f);
	const float w = _fade(z_f);

	/* Interpolate gradients. */
	const float x0 = _lerp(u, d0, d1);
	const float x1 = _lerp(u, d2, d3);
	const float x2 = _lerp(u, d4, d5);
	const float x3 = _lerp(u, d6, d7);

	const float y0 = _lerp(v, x0, x1);
	const float y1 = _lerp(v, x2, x3);

	const float w1 = _lerp(w, y0, y1);

	return w1;
}

float perlin_octave_3d(float x, float y, float z, uint8_t num)
{
	float res = 0.0;
	float amp = 1.0;
	float freq = 1.0 / 16.0;
	// float freq = 0.005;

	for (size_t i = 0; i < num; i++)
	{
		res += amp * perlin_3d(x, y, z, freq);
		amp /= 2.0;
		freq *= 2.0;
	}

	return res;
}
