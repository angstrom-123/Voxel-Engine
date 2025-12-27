#version 300 es
precision highp uint;

@ctype mat4 em_mat4
@ctype vec3 em_vec3

@vs vs_chunk
layout(binding=0) uniform vs_params_chunk {
    mat4 u_vp;
    vec3 u_ccord;
    mat4 u_lightspace;
};

in uvec4 a_vertex;

out vec2 v_uv;
out vec3 v_normal;
out vec4 v_light_pos;
out vec4 v_position;

vec3 _unpack_pos(uvec4 vert) {
    float x = vert.x >> 3;
    float y = vert.y;
    float z = vert.z >> 3;
    return vec3(x, y, z);
}

vec2 _unpack_uv(uvec4 vert) {
    uint uo = (vert.x >> 2) & 0x01;
    uint vo = (vert.x >> 1) & 0x01;

    uint up = vert.w >> 4;
    uint vp = vert.w & 0x0F;
    float u = float(up + uo) * 0.0625;
    float v = float(vp + vo) * 0.0625;
    return vec2(u, v);
}

vec3 _unpack_normal(uvec4 vert) {
    uint packed = vert.z & 0x07;
    if      (packed == 1) return vec3( 1.0,  0.0,  0.0);
    else if (packed == 2) return vec3(-1.0,  0.0,  0.0);
    else if (packed == 3) return vec3( 0.0,  1.0,  0.0);
    else if (packed == 4) return vec3( 0.0, -1.0,  0.0);
    else if (packed == 5) return vec3( 0.0,  0.0,  1.0);
    else if (packed == 6) return vec3( 0.0,  0.0, -1.0);
    else return vec3(0.0, 0.0, 0.0);

    // uint packed = vert.z & 0x07;
    // uint axis = (packed - 1) / 2;
    // float sign_ = 1.0 - 2.0 * mod(packed, 2.0);
    // vec3 normal;
    // normal[axis] = sign_;
    //
    // return normal;
}

void main() {
    // Model matrix application here is equivalent to adding u_ccord
    vec4 model_local = vec4(_unpack_pos(a_vertex) + u_ccord, 1.0);
    gl_Position = u_vp * model_local;

    v_uv = _unpack_uv(a_vertex);
    v_normal = _unpack_normal(a_vertex);
    v_light_pos = u_lightspace * model_local;
    v_position = gl_Position;
}

@end

@fs fs_chunk
layout(binding=0) uniform sampler u_atlas_sampler;
layout(binding=0) uniform texture2D u_atlas;
layout(binding=1) uniform sampler u_shadowmap_sampler;
layout(binding=1) uniform texture2D u_shadowmap;

layout(binding=1) uniform fs_params_chunk {
    vec3 u_sun_dir;
};

in vec2 v_uv;
in vec3 v_normal;
in vec4 v_light_pos;
in vec4 v_position;

layout(location=0) out vec4 out_albedo;
layout(location=1) out vec4 out_normal;
layout(location=2) out float out_depth;
layout(location=3) out float out_shadow;

float visibility() {
    vec3 up = vec3(0.0, 1.0, 0.0);
    if (dot(u_sun_dir, up) < 0.0)
        return 0.0;

    vec3 proj_cord = -v_light_pos.xyz / v_light_pos.w;
    proj_cord = proj_cord * 0.5 + 0.5;
    proj_cord.x = 1.0 - proj_cord.x;

    // Cull shadows that fall outside of the shadow camera's view frustum.
    if (proj_cord.x > 1.0 || proj_cord.x < 0.0 || proj_cord.y > 1.0 || proj_cord.y < 0.0)
        return 1.0;

    float bias = max(0.002 * (1.0 - dot(v_normal, u_sun_dir)), 0.002);

    float curr_dep = proj_cord.z;

    float visibility = 0.0;
    float texel_size = 1.0 / 1024.0;
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            vec2 pcf_uv = proj_cord.xy + vec2(x, y) * texel_size;
            float pcf_dep = texture(sampler2D(u_shadowmap, u_shadowmap_sampler), pcf_uv).x;
            visibility += (curr_dep + bias < pcf_dep) ? 0.0 : 1.0;
        }
    }

    return visibility / 9.0;
}

void main() {
    // TODO: Handle transparency
    // if (frag_color.a < 0.01) discard;

    out_normal = vec4(v_normal * 0.5 + 0.5, 1.0);
    out_depth = gl_FragCoord.z / gl_FragCoord.w;
    // out_depth = (1.0 - v_position.xyz / v_position.w).z;
    out_albedo = texture(sampler2D(u_atlas, u_atlas_sampler), v_uv);
    out_shadow = visibility();
}

@end 

@program chunk vs_chunk fs_chunk
