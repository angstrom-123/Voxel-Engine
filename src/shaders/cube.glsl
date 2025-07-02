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

out vec3 v_norm;
out vec2 v_uv;
out float v_type;

void main() {
	int idx = gl_InstanceIndex;
	if (idx < u_cnt) {
		vec4 inst = u_data[idx];
		vec3 ofst = inst.xyz;
		gl_Position = u_mvp * vec4(ofst + i_pos, 1.0);

		v_norm = i_norm;
		v_uv = i_uv;
		v_type = inst.w;
	} else {
		gl_Position = vec4(0.0); // Discard instances out of range of batch
	}
}
@end

@fs fs
layout(binding=0) uniform texture2D u_tex;
layout(binding=0) uniform sampler u_smp;

in vec3 v_norm;
in vec2 v_uv;
in float v_type;

out vec4 frag_color;

void main() {
	// TODO: use base uv and type to calculate uv coords for this block type
    frag_color = texture(sampler2D(u_tex, u_smp), v_uv);
	frag_color.rgb *= (0.7 + 0.3 * dot(v_norm, vec3(0.0, 1.0, 0.0)));
}
@end

@program cube vs fs
