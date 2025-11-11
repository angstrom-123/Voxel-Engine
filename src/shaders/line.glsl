#version 300 es
precision highp uint

@ctype mat4 em_mat4
@ctype vec3 em_vec3

@vs vs_line
layout(binding=0) uniform vs_params_line {
    mat4 u_mvp;
    vec3 u_offset;
};

in vec3 a_pos;
in vec3 a_col;

out vec3 v_col;

void main() {
    gl_Position = u_mvp * vec4((a_pos + u_offset), 1.0);

    v_col = a_col;
}

@end

@fs fs_line

in vec3 v_col;

out vec4 frag_color;

void main() {
    // if (frag_color.a < 0.01) discard;
    //
    frag_color = vec4(v_col, 1.0);
}

@end 

@program line vs_line fs_line
