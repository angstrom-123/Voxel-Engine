#include "camera_controller.h"

void _handle_mouse(camera_controller_t *cc, camera_t *cam, vec2 delta)
{
    cam->pitch += -delta.y * cc->turn_speed;
    cam->yaw += -delta.x * cc->turn_speed;

    /* Clamping up and down facing to ~ vertical. */
    cam->pitch = em_min(cam->pitch, MAX_PITCH);
    cam->pitch = em_max(cam->pitch, -MAX_PITCH);

    quaternion yaw_q = em_quaternion_from_axis_angle(WORLD_Y, cam->yaw);

    vec3 right = em_quaternion_rotate_vec3(WORLD_X, yaw_q);
    quaternion pitch_q = em_quaternion_from_axis_angle(right, cam->pitch);

    cam->rot = em_normalize_quaternion(em_mul_quaternion(pitch_q, yaw_q));

    cam->fwd = cam_get_fwd(cam);
    cam->right = cam_get_right(cam);
    cam->up = cam_get_up(cam);

    mat4 rot = em_quaternion_to_mat4(em_conjugate_quaternion(cam->rot));
    mat4 trans = em_translate_mat4(em_mul_vec3_f(cam->pos, -1.0));
    cam->view = em_mul_mat4(rot, trans);
}

void _handle_keyboard(camera_controller_t *cc, camera_t *cam, bool *keydown, double dt)
{
    vec3 move = {0};

    vec3 fwd = cam->fwd;
    fwd.y = 0.0;
    fwd = em_normalize_vec3(fwd);

    vec3 right = cam->right;
    right.y = 0.0;
    right = em_normalize_vec3(right);

    if (keydown[KEYCODE_W]) move = em_add_vec3(move, fwd);
    if (keydown[KEYCODE_S]) move = em_sub_vec3(move, fwd);
    if (keydown[KEYCODE_D]) move = em_add_vec3(move, right);
    if (keydown[KEYCODE_A]) move = em_sub_vec3(move, right);

    if (keydown[KEYCODE_SPACE]) move = em_add_vec3(move, WORLD_Y);
    if (keydown[KEYCODE_LEFT_CONTROL]) move = em_sub_vec3(move, WORLD_Y);

    move = em_mul_vec3_f(em_mul_vec3_f(move, cc->move_speed), dt);
    vec3 next_pos = em_add_vec3(cam->pos, move);
    cam->pos = next_pos;
}

void camera_ctl_init(camera_controller_t *cc, const camera_controller_desc_t *desc)
{
    cc->move_speed = desc->move_speed;
    cc->turn_speed = desc->turn_speed;
}

void camera_ctl_cleanup(camera_controller_t *cc)
{
    (void) cc;
}

void camera_ctl_update(camera_controller_t *cc, camera_t *cam, event_system_t *es, double dt)
{
    _handle_mouse(cc, cam, es->frame.mouse_delta);
    _handle_keyboard(cc, cam, es->keys_down, dt);
    cam_update(cam);
}
