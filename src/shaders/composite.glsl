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

layout(binding=0) uniform sampler u_buffer_sampler;
layout(binding=0) uniform texture2D u_albedo_buffer;
layout(binding=1) uniform texture2D u_normal_buffer;
layout(binding=2) uniform texture2D u_depth_buffer;
layout(binding=3) uniform texture2D u_shadow_buffer;

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

vec3 gamma(vec3 c) {
    // float gamma = 2.2;
    float gamma = 1.5;
    return pow(c, vec3(1.0 / gamma));
}

vec3 light(vec3 nrm, float sha) {
    float night_ambient = 0.2;
    float day_ambient = 0.5;
    float sun_dot_up = dot(u_light_dir, vec3(0.0, 1.0, 0.0));
    float sun_strength = sun_dot_up > 0.0 ? mix(0.0, 1.0, sun_dot_up) : 0.0;

    vec3 sun_tint = vec3(1.0, vec2(max(abs(sun_dot_up), 0.3)));
    // vec3 sun_tint = vec3(1.0);
    // if (sun_dot_up > 0.0)
    //     sun_tint = vec3(1.0, vec2(max(sun_dot_up, 0.3)));
    // vec3 sun_tint = vec3(1.0);
    // vec3 sunset_tint = vec3(1.0, 0.5, 0.5);
    // sun_tint = mix(sunset_tint, sun_tint, abs_sun);

    vec3 l = normalize(u_light_dir);
    vec3 n = normalize(nrm);
    float n_dot_l = dot(n, l);

    float diffuse = 0.0;
    if (n_dot_l > 0.0) {
        float s = sha;
        diffuse = clamp(n_dot_l * s, 0.0, 0.5);
    }

    return vec3(diffuse + day_ambient) * (sun_tint * vec3(max(sun_strength, night_ambient)));
}

// float ssao(

void main() {
    vec3 albedo = texture(sampler2D(u_albedo_buffer, u_buffer_sampler), v_uv).xyz;
    vec3 normal = remap(texture(sampler2D(u_normal_buffer, u_buffer_sampler), v_uv).xyz);
    float depth = texture(sampler2D(u_depth_buffer, u_buffer_sampler), v_uv).x;
    float shadow = texture(sampler2D(u_shadow_buffer, u_buffer_sampler), v_uv).x;
    vec3 position = world_pos_from_depth(depth);

    // Debugging outputs
    // frag_color = vec4(albedo, 1.0);
    // frag_color = vec4(normal, 1.0);
    // frag_color = vec4(vec3(depth), 1.0);
    // frag_color = vec4(vec3(shadow), 1.0);
    // frag_color = vec4(position, 1.0);

    vec3 lighting = light(normal, shadow);
    vec3 output_col = lighting * albedo;
    frag_color = vec4(gamma(output_col), 1.0);

    if (depth > 100.0)
        frag_color.xyz *= vec3(max(110.0 - depth, 0.0001) / 10.0);
}

@end

@program composite vs_composite fs_composite
