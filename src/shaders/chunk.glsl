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
out vec4 v_projection;
out vec4 v_pos;
out vec4 v_light_pos;

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
    if (packed == 1) return vec3( 1.0,  0.0,  0.0);
    else if (packed == 2) return vec3(-1.0,  0.0,  0.0);
    else if (packed == 3) return vec3( 0.0,  1.0,  0.0);
    else if (packed == 4) return vec3( 0.0, -1.0,  0.0);
    else if (packed == 5) return vec3( 0.0,  0.0,  1.0);
    else if (packed == 6) return vec3( 0.0,  0.0, -1.0);
    else return vec3(0.0, 0.0, 0.0);
}

void main() {
    vec3 pos = _unpack_pos(a_vertex);
    vec2 uv = _unpack_uv(a_vertex);
    vec3 normal = _unpack_normal(a_vertex);

    // Model matrix application here is equivalent to adding u_ccord
    vec4 model_local = vec4(pos + u_ccord, 1.0);
    gl_Position = u_vp * model_local;

    v_uv = uv;
    v_normal = normal;
    v_projection = gl_Position;
    v_pos = model_local;

    v_light_pos = u_lightspace * model_local;
}

@end

@fs fs_chunk
layout(binding=0) uniform sampler u_smp_col;
layout(binding=0) uniform texture2D u_tex_col;
layout(binding=1) uniform sampler u_smp_sha;
layout(binding=1) uniform texture2D u_tex_sha;

layout(binding=1) uniform fs_params_chunk {
    vec3 u_sun_dir;
};

in vec2 v_uv;
in vec3 v_normal;
in vec4 v_projection;
in vec4 v_pos;
in vec4 v_light_pos;

layout(location=0) out vec4 frag_col;
layout(location=1) out vec4 frag_nrm;
layout(location=2) out float frag_dep;
layout(location=3) out float frag_sha;

float calc_visibility() {
    vec3 proj_cord = -v_light_pos.xyz / v_light_pos.w;
    proj_cord = proj_cord * 0.5 + 0.5;
    proj_cord.x = 1.0 - proj_cord.x;

    if (proj_cord.x > 1.0 || proj_cord.x < 0.0 || proj_cord.y > 1.0 || proj_cord.y < 0.0)
        return 1.0;

    float vis = 0.0;

    float texel_size = 1.0 / 1024.0;
    float bias = max(0.004 * (1.0 - dot(v_normal, u_sun_dir)), 0.004);

    float curr_dep = proj_cord.z;

    // for (int x = -1; x <= 1; x++) {
    //     for (int y = -1; y <= 1; y++) {
    // for (int x = -4; x <= 4; x++) {
    //     for (int y = -4; y <= 4; y++) {
            int x = 0;
            int y = 0;
            vec2 pcf_uv = proj_cord.xy + vec2(x, y) * texel_size;
            float pcf_dep = texture(sampler2D(u_tex_sha, u_smp_sha), pcf_uv).x;
            // vis += curr_dep + bias > pcf_dep ? 1.0 : 0.3;
            vis = curr_dep + bias > pcf_dep ? 1.0 : 0.0;
    //     }
    // }

    // vis /= 9.0;
    // vis /= 81.0;

    return vis;
}

void main() {
    // TODO: Handle transparency
    // if (frag_color.a < 0.01) discard;

    frag_nrm = vec4(v_normal, 1.0);
    frag_dep = v_projection.z / v_projection.w;
    frag_col = texture(sampler2D(u_tex_col, u_smp_col), v_uv);
    frag_sha = calc_visibility();
}

@end 

@program chunk vs_chunk fs_chunk
