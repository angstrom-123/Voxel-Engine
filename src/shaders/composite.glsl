#version 300 es
precision highp uint

@ctype mat4 em_mat4
@ctype vec2 em_vec2
@ctype vec3 em_vec3

@block remap 
vec3 remap(vec3 vec) {
    return vec * 2.0 - 1.0;
}
vec2 remap(vec2 vec) {
    return vec * 2.0 - 1.0;
}
@end

@vs vs_composite
@include_block remap

in vec2 a_pos;

out vec2 v_uv;

void main() {
    gl_Position = vec4(remap(a_pos), 0.5, 1.0);
    v_uv = a_pos;
}

@end

@fs fs_composite
@include_block remap

layout(binding=0) uniform sampler u_smp;
layout(binding=0) uniform texture2D u_tex_col;
layout(binding=1) uniform texture2D u_tex_nrm;
layout(binding=2) uniform texture2D u_tex_dep;
layout(binding=3) uniform texture2D u_tex_sha;

layout(binding=1) uniform fs_params_composite {
    mat4 u_inv_vp;
    vec3 u_light_dir;
    vec3 u_eye_pos;
};

in vec2 v_uv;

out vec4 frag_color;

vec3 world_pos_from_depth(float d) {
    vec4 clip = vec4(remap(vec3(v_uv, d)), 1.0);
    vec4 world = u_inv_vp * clip;
    return world.xyz / world.w;
}

vec4 gamma(vec4 c) {
    // float gamma = 2.2;
    float gamma = 1.5;
    return vec4(pow(c.xyz, vec3(1.0 / gamma)), c.w);
}

void main() {
    vec3 col = texture(sampler2D(u_tex_col, u_smp), v_uv).xyz;
    vec3 nrm = remap(texture(sampler2D(u_tex_nrm, u_smp), v_uv).xyz);
    float dep = texture(sampler2D(u_tex_dep, u_smp), v_uv).x;
    float sha = texture(sampler2D(u_tex_sha, u_smp), v_uv).x;
    vec3 pos = world_pos_from_depth(dep);

    // Debugging outputs
    // frag_color = vec4(col, 1.0);
    // frag_color = vec4(nrm, 1.0);
    // frag_color = vec4(vec3(dep), 1.0);
    // frag_color = vec4(vec3(sha), 1.0);
    // frag_color = vec4(pos, 1.0);

    // Sokol lighting 
    float spec_power = 8.0;
    float ambient = 0.25;
    vec3 l = normalize(u_light_dir);
    vec3 n = normalize(nrm);
    float n_dot_l = dot(n, l);
    if (n_dot_l > 0.0) {
        float s = mix(0.3, 0.8, sha);
        float diffuse = max(n_dot_l * s, 0.0);

        // vec3 v = normalize(u_eye_pos - pos);
        // vec3 r = reflect(-l, n);
        // float r_dot_v = max(dot(r, v), 0.0);
        // float specular = min(pow(r_dot_v, spec_power) * n_dot_l * s, 0.1);
        float specular = 0.0;
        // specular *= 0.0000001;

        frag_color = vec4(vec3(specular) + (diffuse + ambient) * col, 1.0);
    } else {
        frag_color = vec4(col * ambient, 1.0);
    }
    frag_color = gamma(frag_color);
}

@end

@program composite vs_composite fs_composite
