@ctype mat4 em_mat4

@vs vs
layout(binding=0) uniform vs_params {
    mat4 mvp;
};

in vec3 base_pos;
in vec3 inst_pos;
in vec2 tex_uv;

out vec2 uv;

void main() {
	vec4 pos = vec4(base_pos + inst_pos, 1.0);
	gl_Position = mvp * pos;
    uv = tex_uv;
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
