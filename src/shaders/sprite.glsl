#version 300 es

precision highp uint

@ctype mat4 em_mat4
@ctype vec2 em_vec2

@vs vs_sprite
@glsl_options flip_vert_y
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
layout(binding=1) uniform fs_params_sprite {
    int u_is_char;
};

layout(binding=0) uniform sampler u_smp;
layout(binding=0) uniform texture2D u_tex_sprite_atlas;
layout(binding=1) uniform texture2D u_tex_font_atlas;

in vec2 v_uv;

out vec4 frag_color;

void main() {
    if (frag_color.a < 0.01) discard;

    frag_color = bool(u_is_char)
               ? texture(sampler2D(u_tex_font_atlas, u_smp), v_uv)
               : texture(sampler2D(u_tex_sprite_atlas, u_smp), v_uv);
}

@end 

@program sprite vs_sprite fs_sprite
