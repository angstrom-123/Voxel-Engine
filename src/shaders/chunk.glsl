#version 300 es
precision highp uint;

@ctype mat4 em_mat4

@vs vs
layout(binding=0) uniform vs_params {
	mat4 u_mvp;
	vec3 u_chnk_pos;
};

in uvec4 a_xyzn; /* ubyte4 (x,y,z in chunk, packed normal) */
in uint a_uv;	 /* uint (packed uv) */

out vec2 v_uv;
out vec3 v_norm;
out float v_h;

/*
	Packed format:

	Bit Idx | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
	Meaning |pad|pad|-z |+z |-y |+y |-x |+x |

	Cube normals are always axis aligned and normalized so can be simplified 
	to a + or - for each component. This encoding also allows for 45 degree
	angle normals (can be useful in the future for billboarded sprites)

	When implementing billboarded sprites, implementation will have to change 
	to ensure that the unpacked values is normalized.
*/
vec3 unpack_normal(uint packed) {
	uint x_code = (packed & 3);
	uint y_code = (packed & 12) >> 2;
	uint z_code = (packed & 48) >> 4;

	float x = 0.0;
	float y = 0.0;
	float z = 0.0; 

	if (x_code == 1) x = 1.0;
	else if (x_code == 2) x = -1.0;

	if (y_code == 1) y = 1.0;
	else if (y_code == 2) y = -1.0;

	if (z_code == 1) z = 1.0;
	else if (z_code == 2) z = -1.0;

	return vec3(x, y, z);
}

float round_down(float num, float to)
{
	float mod = mod(num, to);
	if (abs(mod - to) < 1.0E-8) return mod;
	return num - mod;
}

float round_up(float down, float num, float to)
{
	if (num == down) return num;
	return down + to;
}

float round_nearest(float num, float to)
{
	float down = round_down(num, to);
	float up = round_up(down, num, to);
	if ((num - down) < (up - num)) return down;
	return up;
}

/*
	Packed as 2 consecutive 8-bit unsigned ints.
*/
vec2 unpack_uv(uint packed) {
	/* Unpack */
	float a = (packed >> 16) / 65535.0;
	float b = (packed & 0xFFFF) / 65535.0;

	/* Round to account for inaccuracies from packing */
	float a_r = round_nearest(a, 0.0005);
	float b_r = round_nearest(b, 0.0005);

	return vec2(a_r, b_r);
}

void main() {
	vec3 pos = a_xyzn.xyz;
	gl_Position = u_mvp * vec4(pos + u_chnk_pos, 1.0);

	v_uv = unpack_uv(a_uv);
	v_norm = unpack_normal(uint(a_xyzn.w));
	v_h = pos.y;
}
@end

@fs fs

layout(binding=0) uniform texture2D u_tex;
layout(binding=0) uniform sampler u_smp;

in vec2 v_uv;
in vec3 v_norm;
in float v_h;

out vec4 frag_color;

void main() {
	// frag_color = vec4(v_h / 64, v_h / 64, v_h / 64, 1.0);
	if (frag_color.a < 0.01) discard;

	frag_color = texture(sampler2D(u_tex, u_smp), v_uv);

	// Directional lighting:
	// frag_color.rgb *= (0.7 + 0.3 * dot(v_norm, vec3(0.707, 0.707, 0.707)));
}
@end

@program chunk vs fs
