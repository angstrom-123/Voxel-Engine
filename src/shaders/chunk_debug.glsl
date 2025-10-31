#version 300 es
precision highp uint;

@ctype mat4 em_mat4

@vs vs 
layout(binding=0) uniform vs_params {
    mat4 u_mvp;
    mat4 u_view;
    vec3 u_ccord;
};

in uvec4 a_vertex;

out vec2 v_uv;
out vec3 v_normal;
out float v_depth;
flat out int v_even;

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

/*    8  7  6  5  4  3  2  1
    |  |  |  |  |  | z| y| x|
*/

vec3 _unpack_normal(uvec4 vert) {
    uint packed = (vert.z) & 0x07;
    vec3 unpacked = vec3(packed & 1, (packed >> 1) & 1, (packed >> 2) & 1);
    return vec3(1.0) - (unpacked * 2);
}

void main() {
    vec3 pos = _unpack_pos(a_vertex);
    vec2 uv = _unpack_uv(a_vertex);
    vec3 normal = _unpack_normal(a_vertex);

    gl_Position = u_mvp * vec4(pos + u_ccord, 1.0);

    v_uv = uv;
    v_normal = normal;
    v_depth = -(u_view * vec4(pos + u_ccord, 1.0)).z;

    int xeven = int((u_ccord.x / 16.0) - 2.0 * floor((u_ccord.x / 16.0) / 2.0));
    int zeven = int((u_ccord.z / 16.0) - 2.0 * floor((u_ccord.z / 16.0) / 2.0));
    v_even = (xeven == 1 ^^ zeven == 1) ? 0 : 1;
}

@end

@fs fs 
layout(binding=0) uniform texture2D u_tex;
layout(binding=0) uniform sampler u_smp;
layout(binding=1) uniform fs_params {
    vec4 u_fog_data;
};

in vec2 v_uv;
in vec3 v_normal;
in float v_depth;
flat in int v_even;

out vec4 frag_color;

void main() {
    if (frag_color.a < 0.01) discard;

    vec4 fog_col = vec4(u_fog_data.xyz, 1.0);
    float fog_dist = u_fog_data.w;

    float fog_factor = smoothstep(fog_dist - 10, fog_dist, v_depth);

    frag_color = mix(texture(sampler2D(u_tex, u_smp), v_uv), fog_col, fog_factor);
    if (v_even == 1)
        frag_color.xyz = frag_color.xyz * 0.9;
}

@end 

@program chunk vs fs
