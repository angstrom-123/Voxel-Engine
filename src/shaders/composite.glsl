#version 300 es
precision highp uint

@ctype mat4 em_mat4
@ctype vec2 em_vec2

@vs vs_composite

in vec2 a_pos;

out vec2 v_uv;

void main() {
    gl_Position = vec4(a_pos * 2.0 - 1.0, 0.5, 1.0);
    v_uv = a_pos;
}

@end

@fs fs_composite
layout(binding=0) uniform sampler u_smp;
layout(binding=0) uniform texture2D u_tex_col;
layout(binding=1) uniform texture2D u_tex_nrm;
layout(binding=2) uniform texture2D u_tex_dep;

in vec2 v_uv;

out vec4 frag_color;

void main() {
    vec3 col = texture(sampler2D(u_tex_col, u_smp), v_uv).xyz;
    vec3 nrm = texture(sampler2D(u_tex_nrm, u_smp), v_uv).xyz;
    vec3 dep = texture(sampler2D(u_tex_dep, u_smp), v_uv).xxx;

    // Debugging outputs
    frag_color = vec4(col, 1.0);
    // frag_color = vec4(nrm, 1.0);
    // frag_color = vec4(nrm * 0.5 + 0.5, 1.0);
    // frag_color = vec4(dep, 1.0);

    // vec3 light_pos = vec3(0, 80, 0);
    // vec3 to_light = light_pos - pos;
    // float dist = length(to_light);
    // vec3 light_dir = to_light / max(dist, 0.001);
    //
    // float n_dot_l = max(dot(nrm, light_dir), 0.0);
    // float atten = 1.0 / (1.0 + 0.1 * dist + 0.01 * dist * dist);
    //
    // vec3 light_col = vec3(1.0, 1.0, 0.8);
    // float intensity = 5.0;
    //
    // vec3 diffuse = n_dot_l + light_col * intensity;
    // vec3 ambient = 0.0 * col;
    // vec3 lit = ambient + diffuse * atten;
    //
    // vec3 final = col * lit;
    //
    // float gamma = 2.2;
    // vec3 gamma_corrected = pow(final, vec3(1.0 / gamma));
    //
    // frag_color = vec4(gamma_corrected, 1.0);
}

@end

@program composite vs_composite fs_composite
