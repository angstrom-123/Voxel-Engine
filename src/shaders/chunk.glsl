#version 300 es
precision highp uint;

@ctype mat4 em_mat4
@ctype vec3 em_vec3

@vs vs_chunk
layout(binding=0) uniform vs_params_chunk {
    mat4 u_vp;
    vec3 u_ccord;
};

in uvec4 a_vertex;

out vec2 v_uv;
out vec3 v_normal;
out vec4 v_projection;
out vec4 v_pos;

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
    int axis = (int(packed) - 1) / 2;

    return vec3(
        sign(packed) * float(axis == 0),
        sign(packed) * float(axis == 1),
        sign(packed) * float(axis == 2)
    );
}

void main() {
    vec3 pos = _unpack_pos(a_vertex);
    vec2 uv = _unpack_uv(a_vertex);
    vec3 normal = _unpack_normal(a_vertex);

    // Model matrix application here is equivalent to adding u_ccord
    gl_Position = u_vp * vec4(pos + u_ccord, 1.0);

    v_uv = uv;
    v_normal = normal;
    v_projection = gl_Position;
    v_pos = vec4(pos + u_ccord, 1.0);
}

@end

@fs fs_chunk
layout(binding=0) uniform texture2D u_tex;
layout(binding=0) uniform sampler u_smp;

in vec2 v_uv;
in vec3 v_normal;
in vec4 v_projection;
in vec4 v_pos;

layout(location=0) out vec4 frag_col;
layout(location=1) out vec4 frag_nrm;
layout(location=2) out float frag_dep;
layout(location=3) out vec4 frag_pos;

void main() {
    // TODO: Handle transparency
    // if (frag_color.a < 0.01) discard;

    frag_col = texture(sampler2D(u_tex, u_smp), v_uv);
    frag_nrm = vec4(v_normal, 1.0);
    frag_dep = 1.0 - v_projection.z / v_projection.w;
    frag_pos = v_pos;
}

@end 

@program chunk vs_chunk fs_chunk
