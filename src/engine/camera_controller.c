#include "camera_controller.h"

struct coord_set {
    ivec3 voxel_coord;
    ivec2 chunk_coord;
    uvec3 voxel_index;
};

static struct coord_set _get_pos_data(vec3 pos, ivec2 centre)
{
    ivec3 vc = em_floor_vec3(pos);
    ivec2 cc = IVEC2(floorf(pos.x / (float) CHUNK_SIZE) * CHUNK_SIZE, 
                     floorf(pos.z / (float) CHUNK_SIZE) * CHUNK_SIZE);
    uvec3 vi = UVEC3(vc.x - centre.x, vc.y, vc.z - centre.y);
    return (struct coord_set) { vc, cc, vi };
}

static void _get_surrounding_voxels(vec3 pos, ivec2 surr_pos, chunk_data_t *surr[3][3], 
                                    cube_type_e res[3][4][3], size_t res_height, ivec3 *cam_voxel)
{
    INSTRUMENT_FUNC_BEGIN();
    struct coord_set cs = _get_pos_data(pos, surr_pos);
    *cam_voxel = cs.voxel_coord;

    for (size_t xo = 0; xo < 3; xo++) for (size_t zo = 0; zo < 3; zo++)
    {
        ivec2 cc = IVEC2(1, 1);
        ivec3 vix = AS_IVEC3(cs.voxel_index);
        vix = em_add_ivec3(vix, IVEC3(xo - 1, 0, zo - 1));
        if (vix.x < 0) 
        {
            vix.x = CHUNK_SIZE - 1;
            cc.x -= 1;
        }
        else if (vix.x > CHUNK_SIZE - 1) 
        {
            vix.x = 0;
            cc.x += 1;
        }
        if (vix.z < 0) 
        {
            vix.z = CHUNK_SIZE - 1;
            cc.y += 1;
        }
        else if (vix.z > CHUNK_SIZE - 1) 
        {
            vix.z = 0;
            cc.y -= 1;
        }

        for (size_t y = 0; y < res_height; y++)
        {
            size_t vix_y = vix.y - 2 + y;
            res[xo][y][zo] = (vix_y >= 0 && vix_y < CHUNK_HEIGHT)
                           ? surr[cc.x][cc.y]->types[vix.x][vix_y][vix.z]
                           : CUBETYPE_AIR;
        }
    }
    INSTRUMENT_FUNC_END();
}

static void _ground_check(ctl_t *cc, camera_t *cam, float epsilon)
{
    const vec3 DOWN = VEC3(0.0, -1.0, 0.0);
    const float INSET = 0.005;
    ray_t rs[4] = {
        (ray_t) {
            .direction = DOWN,
            .length = epsilon,
            .origin = VEC3(cc->collider.x.min + cam->pos.x + INSET,
                           cc->collider.y.min + cam->pos.y,
                           cc->collider.z.min + cam->pos.z + INSET)
        },
        (ray_t) {
            .direction = DOWN,
            .length = epsilon,
            .origin = VEC3(cc->collider.x.max + cam->pos.x - INSET,
                           cc->collider.y.min + cam->pos.y,
                           cc->collider.z.min + cam->pos.z + INSET)
        },
        (ray_t) {
            .direction = DOWN,
            .length = epsilon,
            .origin = VEC3(cc->collider.x.min + cam->pos.x + INSET,
                           cc->collider.y.min + cam->pos.y,
                           cc->collider.z.max + cam->pos.z - INSET)
        },
        (ray_t) {
            .direction = DOWN,
            .length = epsilon,
            .origin = VEC3(cc->collider.x.max + cam->pos.x - INSET,
                           cc->collider.y.min + cam->pos.y,
                           cc->collider.z.max + cam->pos.z - INSET)
        }
    };

    cube_type_e surr[3][4][3];
    ivec3 voxel;
    _get_surrounding_voxels(cam->pos, cc->surrounding_pos, cc->surrounding, surr, 2, &voxel);

    if (cc->velocity.y > 0.0)
    {
        cc->time_grounded = 0;
        return;
    }

    for (size_t i = 0; i < 4; i++)
    {
        // Only checking the blocks just below the player for colission with the ground.
        for (size_t x = 0; x < 3; x++) for (size_t z = 0; z < 3; z++)
        {
            if (surr[x][0][z] != CUBETYPE_AIR)
            {
                ivec3 coord = em_add_ivec3(voxel, IVEC3(x - 1, -2, z - 1));
                aabb_t box = aabb_from_voxel_coord(coord);
                if (aabb_ray_intersecting(box, rs[i]))
                {
                    cc->time_grounded++;
                    return;
                }
            }
        }
    }

    cc->time_grounded = 0;
}

static vec3 _resolve_collision(ctl_t *cc, vec3 curr_pos, vec3 move)
{
    INSTRUMENT_FUNC_BEGIN();
    ivec3 curr_voxel;
    cube_type_e surr[3][4][3];
    _get_surrounding_voxels(curr_pos, cc->surrounding_pos,  cc->surrounding, surr, 4, &curr_voxel);

    const size_t axis_order[3] = { 1, 0, 2 };

    vec3 next_pos = curr_pos;
    for (size_t i = 0; i < 3; i++)
    {
        size_t axis = axis_order[i];
        next_pos.elements[axis] += move.elements[axis];

        aabb_t next_coll = aabb_with_offset(cc->collider, next_pos);
        float axis_depth = 0.0;
        bool collided = false;

        for (size_t x = 0; x < 3; x++)
        {
        for (size_t y = 0; y < 4; y++)
        {
        for (size_t z = 0; z < 3; z++)
        {
            ivec3 voxel_coord = em_add_ivec3(curr_voxel, IVEC3(x - 1, y - 2, z - 1));
            aabb_t block_coll = aabb_from_voxel_coord(voxel_coord);

            vec3 ax;
            float d;
            bool intersecting = aabb_intersecting_depth(next_coll, block_coll, &ax, &d);
            if (surr[x][y][z] != CUBETYPE_AIR && intersecting)
            {
                if (em_abs(d) > em_abs(axis_depth))
                    axis_depth = d;
                collided = true;
            }
        }
        }
        }

        float resolve = 0.0;
        if (collided)
        {
            const float EPSILON = 1.01;
            resolve = axis_depth * EPSILON;
            cc->velocity.elements[axis] = 0.0;
        }

        next_pos.elements[axis] = curr_pos.elements[axis] + move.elements[axis] + resolve;
    }

    INSTRUMENT_FUNC_END();
    return next_pos;
}

void ctl_init(ctl_t *cc, camera_t *cam, const ctl_desc_t *desc)
{
    // Shift cam down from top of collider so it doesn't clip inside ceilings when jumping.
    const float EPSILON = 0.01;

    cam->pos           = desc->start_pos;
    cc->floor_friction = desc->floor_friction;
    cc->air_friction   = desc->air_friction;
    cc->jump_accel     = desc->jump_accel;
    cc->run_accel      = desc->run_accel;
    cc->walk_accel     = desc->walk_accel;
    cc->air_accel      = desc->air_accel;
    cc->turn_speed     = desc->turn_speed;
    cc->gravity        = desc->gravity;
    cc->velocity       = VEC3(0.0, 0.0, 0.0);
    cc->collider_size  = desc->collider_size;
    cc->collider.x     = interval_around(0.0, desc->collider_size.x);
    cc->collider.y     = (interval_t) { -desc->collider_size.y + EPSILON, EPSILON };
    cc->collider.z     = interval_around(0.0, desc->collider_size.z);
    cc->max_fall_velo  = desc->max_fall_velo;
}

void ctl_cleanup(ctl_t *cc)
{
    (void) cc;
}

void ctl_update_view(ctl_t *cc, camera_t *cam, event_system_t *es, bool get_inputs)
{
    if (get_inputs)
    {
        vec2 delta = es->frame.mouse_delta;

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

    }

    mat4 rot = em_quaternion_to_mat4(em_conjugate_quaternion(cam->rot));
    mat4 trans = em_translate_mat4(em_mul_vec3_f(cam->pos, -1.0));
    cam->view = em_mul_mat4(rot, trans);
}

void ctl_update_pos(ctl_t *cc, camera_t *cam, event_system_t *es, double dt, bool get_inputs)
{
    INSTRUMENT_FUNC_BEGIN();
    const size_t coyote_time = 4;
    const size_t cooldown_time = coyote_time + 1;

    if (!cc->surrounding_loaded) return;

    // Normalization
    vec3 fwd = cam->fwd;
    fwd.y = 0.0;
    fwd = em_normalize_vec3(fwd);
    vec3 right = cam->right;
    right.y = 0.0;
    right = em_normalize_vec3(right);

    // Ground check
    _ground_check(cc, cam, 0.001);
    if (cc->time_grounded == 0)
        cc->time_since_grounded++;
    else 
        cc->time_since_grounded = 0;

    // Collect player inputs
    bool jump = false;
    bool walk = false;
    vec3 move = {0};

    if (get_inputs)
    {
        if (es->keys_down[KEYCODE_W]) move = em_add_vec3(move, fwd);
        if (es->keys_down[KEYCODE_S]) move = em_sub_vec3(move, fwd);
        if (es->keys_down[KEYCODE_D]) move = em_add_vec3(move, right);
        if (es->keys_down[KEYCODE_A]) move = em_sub_vec3(move, right);
        jump = es->keys_down[KEYCODE_SPACE];
        walk = es->keys_down[KEYCODE_LEFT_SHIFT];
        move = em_normalize_vec3(move);
    }

    vec3 friction;
    vec3 accel;
    if (cc->time_grounded >= 1)
    {
        cc->velocity.y = 0.0;

        friction = VEC3(cc->floor_friction, cc->air_friction, cc->floor_friction);
        accel = em_mul_vec3_f(move, (walk) ? cc->walk_accel : cc->run_accel);
    }
    else 
    {
        friction = VEC3F(cc->air_friction);
        accel = em_mul_vec3_f(move, cc->air_accel);
        accel.y = cc->gravity;
    }

    if ((cc->time_grounded >= 1 || cc->time_since_grounded <= coyote_time) 
        && jump && cc->jump_cooldown == 0)
    {
        cc->jump_cooldown = cooldown_time;
        accel.y = cc->jump_accel;
    }

    cc->velocity = em_add_vec3(cc->velocity, em_mul_vec3_f(accel, dt));
    cc->velocity = em_mul_vec3(cc->velocity, em_sub_vec3(VEC3F(1.0), friction));
    cc->velocity.y = em_max(cc->velocity.y, cc->max_fall_velo);

    // Resolve collision. NOTE: Assumes that player can't move more than 1 voxel per update.
    // vec3 next_pos = em_add_vec3(cam->pos, em_mul_vec3_f(cc->velocity, dt));
    vec3 next_pos = _resolve_collision(cc, cam->pos, em_mul_vec3_f(cc->velocity, dt));

    // Update position
    cam->pos = next_pos;
    if (cc->jump_cooldown > 0) cc->jump_cooldown--;
    INSTRUMENT_FUNC_END();
}

void ctl_update_surrounding(ctl_t *cc, camera_t *cam, chunk_system_t *cs)
{
    INSTRUMENT_FUNC_BEGIN();
    ivec2 pos = {
        floorf(cam->pos.x / (float) CHUNK_SIZE) * CHUNK_SIZE, 
        floorf(cam->pos.z / (float) CHUNK_SIZE) * CHUNK_SIZE
    };

    cc->surrounding[0][0] = cs->genned->get_ptr(cs->genned, REL_NW(pos));
    cc->surrounding[1][0] = cs->genned->get_ptr(cs->genned, REL_N(pos));
    cc->surrounding[2][0] = cs->genned->get_ptr(cs->genned, REL_NE(pos));
    cc->surrounding[0][1] = cs->genned->get_ptr(cs->genned, REL_W(pos));
    cc->surrounding[1][1] = cs->genned->get_ptr(cs->genned, pos);
    cc->surrounding[2][1] = cs->genned->get_ptr(cs->genned, REL_E(pos));
    cc->surrounding[0][2] = cs->genned->get_ptr(cs->genned, REL_SW(pos));
    cc->surrounding[1][2] = cs->genned->get_ptr(cs->genned, REL_S(pos));
    cc->surrounding[2][2] = cs->genned->get_ptr(cs->genned, REL_SE(pos));

    cc->surrounding_loaded = true;
    cc->surrounding_pos = pos;
    INSTRUMENT_FUNC_END();
}
