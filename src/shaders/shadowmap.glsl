#version 300 es

@ctype mat4 em_mat4
@ctype vec3 em_vec3

@vs vs_shadowmap
@glsl_options flip_vert_y
// @glsl_options fixup_clipspace

layout(binding=0) uniform vs_params_shadowmap {
    mat4 u_vp;
    vec3 u_ccord;
};

in uvec4 a_vertex;

vec3 _unpack_pos(uvec4 vert) {
    float x = vert.x >> 3;
    float y = vert.y;
    float z = vert.z >> 3;
    return vec3(x, y, z);
}

void main() {
    vec3 pos = _unpack_pos(a_vertex);

    // Model matrix application here is equivalent to adding u_ccord
    gl_Position = u_vp * vec4(pos + u_ccord, 1.0);
}

@end

@fs fs_shadowmap
// void main() {}
layout(location=0) out float frag_sha;

void main() {
    frag_sha = 1.0 - gl_FragCoord.z;
}

@end 

@program shadowmap vs_shadowmap fs_shadowmap
