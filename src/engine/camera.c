#include "camera.h"

static vec3 _get_forward(quaternion camera_rotation)
{
    return em_quaternion_rotate_vec3((vec3) {0.0, 0.0, -1.0}, camera_rotation);
}

static vec3 _get_right(quaternion camera_rotation)
{
    return em_quaternion_rotate_vec3((vec3) {1.0, 0.0, 0.0}, camera_rotation);
}

static vec3 _get_up(quaternion camera_rotation)
{
    return em_quaternion_rotate_vec3((vec3) {0.0, 1.0, 0.0}, camera_rotation);
}

void cam_handle_mouse(camera_t *cam, float mouse_dx, float mouse_dy)
{
    cam->pitch += -mouse_dy * cam->turn_sens;
    cam->yaw += -mouse_dx * cam->turn_sens;

    if (cam->pitch > MAX_PITCH) 
        cam->pitch = MAX_PITCH;

    if (cam->pitch < -MAX_PITCH)
        cam->pitch = -MAX_PITCH;

    quaternion yaw_q = em_quaternion_from_axis_angle(WORLD_Y, cam->yaw);

    vec3 right = em_quaternion_rotate_vec3(WORLD_X, yaw_q);
    quaternion pitch_q = em_quaternion_from_axis_angle(right, cam->pitch);

    cam->rot = em_normalize_quaternion(em_mul_quaternion(pitch_q, yaw_q));

    cam->fwd = _get_forward(cam->rot);
    cam->right = _get_right(cam->rot);
    cam->up = _get_up(cam->rot);

    mat4 rot = em_quaternion_to_mat4(em_conjugate_quaternion(cam->rot));
    mat4 trans = em_translate_mat4(em_mul_vec3_f(cam->pos, -1.0));
    cam->view = em_mul_mat4(rot, trans);
}

void cam_handle_keyboard(camera_t *cam, bool *key_down, double dt)
{
    vec3 move = {0};

    vec3 fwd = cam->fwd;
    fwd.y = 0.0;
    fwd = em_normalize_vec3(fwd);

    vec3 right = cam->right;
    right.y = 0.0;
    right = em_normalize_vec3(right);

    if (key_down[SAPP_KEYCODE_W           ]) move = em_add_vec3(move, fwd);
    if (key_down[SAPP_KEYCODE_S           ]) move = em_sub_vec3(move, fwd);
    if (key_down[SAPP_KEYCODE_D           ]) move = em_add_vec3(move, right);
    if (key_down[SAPP_KEYCODE_A           ]) move = em_sub_vec3(move, right);
    if (key_down[SAPP_KEYCODE_SPACE       ]) move = em_add_vec3(move, WORLD_Y);
    if (key_down[SAPP_KEYCODE_LEFT_CONTROL]) move = em_sub_vec3(move, WORLD_Y);

    move = em_mul_vec3_f(em_mul_vec3_f(move, cam->move_sens), dt);
    vec3 next_pos = em_add_vec3(cam->pos, move);
    cam->pos = next_pos;
}

camera_t cam_setup(const camera_desc_t *desc) 
{
    mat4 rot = em_quaternion_to_mat4(em_conjugate_quaternion(desc->rot));
    mat4 trans = em_translate_mat4(em_mul_vec3_f(desc->pos, -1.0));
    camera_t cam = {
        .near      = desc->near,
        .far       = desc->far,
        .aspect    = desc->aspect,
        .fov       = desc->fov,
        .turn_sens = desc->turn_sens,
        .move_sens = desc->move_sens,
        .rot       = em_normalize_quaternion(desc->rot),
        .pos       = desc->pos,
        .view      = em_mul_mat4(rot, trans),
        .proj      = em_perspective(desc->fov, desc->aspect, desc->near, desc->far)
    };

    cam_update(&cam);
    return cam;
}

void cam_update(camera_t *cam)
{
    cam->vp = em_mul_mat4(cam->proj, cam->view);
}

void cam_frame(const frame_desc_t *desc)
{
    cam_handle_mouse(desc->cam, *desc->mouse_dx, *desc->mouse_dy);
    cam_handle_keyboard(desc->cam, desc->key_down, desc->dt);
    cam_update(desc->cam);
    
    *desc->mouse_dx = 0.0;
    *desc->mouse_dy = 0.0;
}
