#version 300 es

precision highp uint

@ctype mat4 em_mat4
@ctype vec2 em_vec2

@vs vs_sprite
layout(binding=0) uniform vs_params_sprite {
    mat4 u_vp;
};

in vec2 a_pos;
in vec2 a_uv;
in float a_z_index;

out vec2 v_uv;

void main() {
    gl_Position = u_vp * vec4(a_pos, a_z_index, 1.0);

    v_uv = a_uv;
}

@end 

@fs fs_sprite
layout(binding=0) uniform texture2D u_tex;
layout(binding=0) uniform sampler u_smp;

in vec2 v_uv;

out vec4 frag_color;

void main() {
    if (frag_color.a < 0.01) discard;

    frag_color = texture(sampler2D(u_tex, u_smp), v_uv);
}

@end 

@program sprite vs_sprite fs_sprite
