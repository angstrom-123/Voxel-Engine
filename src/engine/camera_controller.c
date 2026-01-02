#include "camera_controller.h"
#include "em_math.h"

void _handle_mouse(camera_controller_t *cc, camera_t *cam, vec2 delta)
{
    cam->pitch += -delta.y * cc->turn_speed;
    cam->yaw += -delta.x * cc->turn_speed;

    cam->pitch = em_min(cam->pitch, MAX_PITCH);
    cam->pitch = em_max(cam->pitch, -MAX_PITCH);

    quat yaw_q = em_quaternion_from_axis_angle(WORLD_Y, cam->yaw);

    vec3 right = em_quaternion_rotate_vec3(WORLD_X, yaw_q);
    quat pitch_q = em_quaternion_from_axis_angle(right, cam->pitch);

    cam->rot = em_normalize_quaternion(em_mul_quaternion(pitch_q, yaw_q));

    cam->fwd = cam_get_fwd(cam);
    cam->right = cam_get_right(cam);
    cam->up = cam_get_up(cam);

    mat4 rot = em_quaternion_to_mat4(em_conjugate_quaternion(cam->rot));
    mat4 trans = em_translate_mat4(em_mul_vec3_f(cam->pos, -1.0));
    cam->view = em_mul_mat4(rot, trans);
}

// TODO: remove
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

    move = em_mul_vec3_f(move, 20.0 * dt); // Temporary, fallback for physics movement.
    vec3 next_pos = em_add_vec3(cam->pos, move);
    cam->pos = next_pos;
}

void _handle_keyboard_physics(camera_controller_t *cc, camera_t *cam, bool *keydown, double dt)
{
    // Normalization
    vec3 fwd = cam->fwd;
    fwd.y = 0.0;
    fwd = em_normalize_vec3(fwd);
    vec3 right = cam->right;
    right.y = 0.0;
    right = em_normalize_vec3(right);

    // Ground check: TODO

    // Consider free fall as the default.
    vec3 a = em_mul_vec3_f(WORLD_Y, cc->gravity);
    float friction = cc->air_friction;

    if (cc->on_ground) 
    {
        a = VEC3(0, 0, 0);
        friction = cc->floor_friction;

        // Inputs
        if (keydown[KEYCODE_W])     a = em_add_vec3(a, em_mul_vec3_f(fwd, cc->acceleration));
        if (keydown[KEYCODE_S])     a = em_sub_vec3(a, em_mul_vec3_f(fwd, cc->acceleration));
        if (keydown[KEYCODE_D])     a = em_add_vec3(a, em_mul_vec3_f(right, cc->acceleration));
        if (keydown[KEYCODE_A])     a = em_sub_vec3(a, em_mul_vec3_f(right, cc->acceleration));
        if (keydown[KEYCODE_SPACE]) a = em_add_vec3(a, em_mul_vec3_f(WORLD_Y, cc->jump_impulse));
    }

    // dt is applied twice because we are using 2 differentials here (accel, velo).
    vec3 accel = em_mul_vec3_f(em_sub_vec3(a, em_mul_vec3_f(cc->velocity, friction)), dt);

    cc->velocity = em_add_vec3(cc->velocity, accel);
    cc->velocity = em_clamp_vec3(cc->velocity, NEGATE_VEC3(cc->max_velocity), cc->max_velocity);

    vec3 move = em_mul_vec3_f(cc->velocity, dt);
    vec3 tentative_pos = em_add_vec3(cam->pos, move);

    // Check intersections here to see if the move is valid, and update move if needed.
    // Then apply the move to the position.
    //
    // TODO:
    //      Actual physical AABB colission checks
    //          How am I getting the data?
    //          Do I need a lock on the chunk data?
    //              Might be stable enough for me to access willy nilly 
    //              Minimum render distance = 3, so minimum chunk distance = 4 
    //                  And I only need data for the 3x3 chunks around the player
}

void camera_ctl_init(camera_controller_t *cc, camera_t *cam, const camera_controller_desc_t *desc)
{
    cam->pos           = desc->start_pos;
    cc->floor_friction = desc->floor_friction;
    cc->air_friction   = desc->air_friction;
    cc->jump_impulse   = desc->jump_impulse;
    cc->acceleration   = desc->acceleration;
    cc->turn_speed     = desc->turn_speed;
    cc->gravity        = desc->gravity;
    cc->velocity       = VEC3(0.0, 0.0, 0.0);
    cc->max_velocity   = desc->max_velocity;
    cc->collider.x     = interval_around(desc->start_pos.x, desc->collider_size.x);
    cc->collider.y     = interval_around(desc->start_pos.y, desc->collider_size.y);
    cc->collider.z     = interval_around(desc->start_pos.z, desc->collider_size.z);
}

void camera_ctl_cleanup(camera_controller_t *cc)
{
    (void) cc;
}

void camera_ctl_update(camera_controller_t *cc, camera_t *cam, event_system_t *es, double dt)
{
    // Order (kind of) matters! - If you do it wrong, it will update next frame anyway.
    // _handle_keyboard(cc, cam, es->keys_down, dt);  // Updates pos before view update.
    _handle_keyboard_physics(cc, cam, es->keys_down, dt); // Updates pos before view update.
    _handle_mouse(cc, cam, es->frame.mouse_delta); // Updates view matrix before vp update.
    cam_update(cam);
}
