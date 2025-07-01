#version 330
@ctype mat4 em_mat4

@vs vs
layout(binding=0) uniform vs_params {
    mat4 mvp;
	int inst_cnt;
	vec4 inst_pos[256];
};

in vec3 base_pos;
in vec2 tex_uv;

out vec2 uv;

void main() {
	vec4 base_pos4 = vec4(base_pos, 1.0);
	int inst_id = gl_InstanceIndex;
	if (inst_id < inst_cnt) {
		vec4 i_pos = inst_pos[inst_id];
		gl_Position = mvp * (base_pos4 + i_pos);
		uv = tex_uv;
	}
}
@end

@fs fs
layout(binding=0) uniform texture2D tex;
layout(binding=0) uniform sampler smp;

in vec2 uv;

out vec4 frag_color;

void main() {
    frag_color = texture(sampler2D(tex, smp), uv);
}
@end

@program cube vs fs
