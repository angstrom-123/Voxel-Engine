#version 300 es
precision highp uint

@ctype mat4 em_mat4
@ctype vec2 em_vec2

@vs vs_composite
@glsl_options flip_vert_y

in vec2 a_pos;

out vec2 v_uv;

void main() {
    gl_Position = vec4(a_pos * 2.0 - 1.0, 0.5, 1.0);
    v_uv = a_pos;
}

@end

@fs fs_composite

layout(binding=0) uniform texture2D u_tex_col;
layout(binding=1) uniform texture2D u_tex_nrm;
layout(binding=2) uniform texture2D u_tex_dep;
layout(binding=0) uniform sampler u_smp;

in vec2 v_uv;

out vec4 frag_color;

void main() {
    vec3 col = texture(sampler2D(u_tex_col, u_smp), v_uv).xyz;
    vec3 nrm = texture(sampler2D(u_tex_nrm, u_smp), v_uv).xyz;
    float dep = texture(sampler2D(u_tex_dep, u_smp), v_uv).x;

    frag_color = vec4(col, 1.0);
    // frag_color = vec4(nrm, 1.0);
    // frag_color = vec4(vec3(dep), 1.0);
}

@end

@program composite vs_composite fs_composite
