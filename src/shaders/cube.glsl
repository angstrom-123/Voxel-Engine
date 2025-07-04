#version 330
@ctype mat4 em_mat4

@vs vs
layout(binding=0) uniform vs_params {
    mat4 u_mvp;		  // model, view, projection matrix
	int u_cnt;		  // number of instances in batch
	vec4 u_data[256]; // instance data
};

in vec3 i_pos;
in vec3 i_norm;
in vec2 i_uv;
in float i_face;

out vec3 v_norm;
out vec2 v_uv;
flat out int v_type;
flat out int v_face;

void main() {
	int idx = gl_InstanceIndex;
	if (idx < u_cnt) {
		vec4 inst = u_data[idx];
		vec3 ofst = inst.xyz;
		gl_Position = u_mvp * vec4(ofst + i_pos, 1.0);

		v_norm = i_norm;
		v_uv = i_uv;
		v_type = int(inst.w);
		v_face = int(i_face);
	} else {
		gl_Position = vec4(0.0); // Discard instances out of range of batch
	}
}
@end

@fs fs
#define MAX_BLOCK_TYPES 8

layout(binding=0) uniform texture2D u_tex;
layout(binding=0) uniform sampler u_smp;
layout(binding=1) uniform fs_params {
	vec4 u_uv_rects[MAX_BLOCK_TYPES * 6];
};

in vec3 v_norm;
in vec2 v_uv;
flat in int v_type;
flat in int v_face;

out vec4 frag_color;

void main() {
	if (frag_color.a < 0.01) discard;

	vec4 rect = u_uv_rects[v_type * 6 + v_face];
	// vec2 tex_uv = (v_uv - rect.xy) / rect.zw;
	vec2 tex_uv = (v_uv + rect.xy);

	// tex_uv.x = mod(v_type, 16.0) * (1.0 / 16.0) + v_uv.x;
	// tex_uv.y = floor(v_type / 16.0) * (1.0 / 16.0) - v_uv.y;

	frag_color = texture(sampler2D(u_tex, u_smp), tex_uv);
	frag_color.rgb *= (0.7 + 0.3 * dot(v_norm, vec3(0.0, 1.0, 0.0)));
}
@end

@program cube vs fs
