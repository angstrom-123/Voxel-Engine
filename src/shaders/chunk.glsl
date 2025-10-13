#version 300 es
precision highp uint;

@ctype mat4 em_mat4

@vs vs
layout(binding=0) uniform vs_params {
    mat4 u_mvp;      // MVP matrix  (precomputed).
    mat4 u_v;        // View matrix (alone for depth calculation).
    vec3 u_chnk_pos; // Coordinates of chunk (Y unused but required for rendering).
};

in uvec4 a_xyzn; // ubyte4 (x,y,z in chunk, packed normal).
in uint a_uv;    // uint (packed uv).

out vec2 v_uv;
out vec3 v_norm;
out float v_depth;

/*
    Packed format:

    Bit Idx | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
    Meaning |pad|pad|-z |+z |-y |+y |-x |+x |

    Cube normals are always axis aligned and normalized so can be simplified 
    to a + or - for each component. This encoding also allows for 45 degree
    angle normals (can be useful in the future for billboarded sprites)

    When implementing billboarded sprites, implementation will have to change 
    to ensure that the unpacked value is normalized.
*/
vec3 unpack_normal(uint packed) {
    float x = -((packed & 3) * 2 - 3);
    float y = -((packed & 12) * 2 - 3);
    float z = -((packed & 48) * 2 - 3);
    return vec3(x, y, z);
}

float round_nearest(float num, float to)
{
    // float mod = mod(num, to);
    // float down = (mod - to == 0) ? mod : num - mod;
    // float up = (num == down) ? num : down + to;
    // return (num - down < up - num) ? down : up;

    float mod = mod(num, to);
    float down = num - mod + (mod * int(mod == to));
    float up = (down + to) * int(num != down) + (num) * int(num == down);    
    return (num - down < up - num) ? down : up;
}

/*
    Packed as 2 consecutive 8-bit unsigned ints.
*/
vec2 unpack_uv(uint packed) {
    /* Unpack */
    float a = (packed >> 16) / 65535.0;
    float b = (packed & 0xFFFF) / 65535.0;

    /* Round to account for inaccuracies from packing */
    float a_r = round_nearest(a, 0.0005);
    float b_r = round_nearest(b, 0.0005);

    return vec2(a_r, b_r);
}

void main() {
    vec3 pos = a_xyzn.xyz;
    uint nrm = a_xyzn.w;

    gl_Position = u_mvp * vec4(pos + u_chnk_pos, 1.0);

    v_uv = unpack_uv(a_uv);
    v_norm = unpack_normal(nrm);
    v_depth = -(u_v * vec4(pos + u_chnk_pos, 1.0)).z;
}
@end

@fs fs

layout(binding=0) uniform texture2D u_tex;
layout(binding=0) uniform sampler u_smp;
layout(binding=1) uniform fs_params {
    vec4 u_fog_data; // float4 (x,y,z = colour of fog, w = fog distance).
};

in vec2 v_uv;
in vec3 v_norm;
in float v_depth;

out vec4 frag_color;

void main() {
    if (frag_color.a < 0.01) discard;

    vec4 fog_col = vec4(u_fog_data.xyz, 1.0);
    float fog_dist = u_fog_data.w;

    float fog_factor = smoothstep(fog_dist - 20, fog_dist, v_depth);
    frag_color = mix(texture(sampler2D(u_tex, u_smp), v_uv), fog_col, fog_factor);
}
@end

@program chunk vs fs
