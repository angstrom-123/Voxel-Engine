#include "raycast.h"

static bool _get_adjacent_cell(ivec3 *cell, ivec2 *chunk, ivec2 *data_pos, cube_face_idx_e side)
{
    switch (side) {
    case FACEIDX_FRONT: 
        if (cell->z == CHUNK_SIZE - 1)
        {
            cell->z = 0;
            *chunk = REL_S(*chunk);
            data_pos->y += 1;
        }
        else 
            cell->z += 1;
        break;
    case FACEIDX_BACK:
        if (cell->z == 0)
        {
            cell->z = CHUNK_SIZE - 1;
            *chunk = REL_N(*chunk);
            data_pos->y -= 1;
        }
        else 
            cell->z -= 1;
        break;
    case FACEIDX_LEFT:
        if (cell->x == 0)
        {
            cell->x = CHUNK_SIZE - 1;
            *chunk = REL_E(*chunk);
            data_pos->x += 1;
        }
        else 
            cell->x -= 1;
        break;
    case FACEIDX_RIGHT:
        if (cell->x == CHUNK_SIZE - 1)
        {
            cell->x = 0;
            *chunk = REL_W(*chunk);
            data_pos->x -= 1;
        }
        else
            cell->x += 1;
        break;
    case FACEIDX_TOP:
        if (cell->y == CHUNK_HEIGHT - 1)
            return false;
        else 
            cell->y = cell->y + 1;
        break;
    case FACEIDX_BOTTOM:
        if (cell->y == 0)
            return false;
        else 
            cell->y = cell->y - 1;
        break;
    default:
        break;
    };

    return true;
}

/*
 *  TODO: Visualize path of the dda beam: Change block of all cells in the path.
 *        This will help solve the problem to see where it is differing from expected.
 */
bool raycast_dda(chunk_system_t *cs, chunk_edit_e action, const raycast_desc_t *desc)
{
    const size_t MAX_STEPS = 20;
    ivec2 curr_pos = {
        floorf(desc->origin.x / 16.0) * 16, 
        floorf(desc->origin.z / 16.0) * 16
    };

    cube_face_idx_e side_hit = 0;
    chunk_data_t *chunk_data[3][3];
    ivec2 data_pos = (ivec2) {1, 1}; // 3x3 grid of chunk data, 1, 1 is the centre.
    chunk_sys_get_surrounding_data(cs, curr_pos, chunk_data);

    vec3 direction = em_normalize_vec3(desc->direction);
    vec3 step = {
        .x = (direction.x < 0) ? -1 : 1,
        .y = (direction.y < 0) ? -1 : 1,
        .z = (direction.z < 0) ? -1 : 1
    };
    vec3 inv_dir = {
        .x = 1.0 / em_abs(direction.x),
        .y = 1.0 / em_abs(direction.y),
        .z = 1.0 / em_abs(direction.z)
    };
    ivec3 cell = {
        .x = floorf(desc->origin.x) - curr_pos.x,
        .y = floorf(desc->origin.y),
        .z = floorf(desc->origin.z) - curr_pos.y
    };
    vec3 orig_fract = {
        .x = desc->origin.x - floorf(desc->origin.x),
        .y = desc->origin.y - floorf(desc->origin.y),
        .z = desc->origin.z - floorf(desc->origin.z)
    };
    vec3 len = {
        .x = (direction.x < 0) ? orig_fract.x * inv_dir.x : (1.0 - orig_fract.x) * inv_dir.x,
        .y = (direction.y < 0) ? orig_fract.y * inv_dir.y : (1.0 - orig_fract.y) * inv_dir.y,
        .z = (direction.z < 0) ? orig_fract.z * inv_dir.z : (1.0 - orig_fract.z) * inv_dir.z,
    };
    vec3 distance = {0};

    // vec3 direction    = em_normalize_vec3(desc->direction);
    // vec3 step         = em_sign_vec3(desc->direction);
    // vec3 inv_dir      = em_normalize_vec3(em_div_vec3((vec3) {1.0, 1.0, 1.0}, 
    //                                                   em_abs_vec3(direction)));
    // ivec3 global_cell = em_floor_vec3(desc->origin);
    // ivec3 cell        = em_sub_ivec3(global_cell, (ivec3) {curr_pos.x, 0, curr_pos.y});
    // vec3 orig_fract   = em_sub_vec3(desc->origin, em_ivec3_as_vec3(global_cell));
    // vec3 len          = em_mul_vec3(inv_dir, (vec3) {
    //                         (direction.x < 0) ? orig_fract.x : 1.0 - orig_fract.x,
    //                         (direction.y < 0) ? orig_fract.z : 1.0 - orig_fract.y,
    //                         (direction.z < 0) ? orig_fract.y : 1.0 - orig_fract.z,
    //                     });

    for (size_t i = 0; i < MAX_STEPS; i++)
    {
        if (em_length_squared_vec3(distance) >= em_sqr(desc->range))
        {
            ENGINE_LOG_WARN("DDA miss (range).\n", NULL); 
            chunk_sys_return_surrounding_data(cs, chunk_data);
            return false;
        }

        chunk_data_t *cd = chunk_data[data_pos.x][data_pos.y];
        cube_type_e type = cd->types[cell.x][cell.y][cell.z];
        if (type != CUBETYPE_AIR)
        {
            ENGINE_LOG_OK("DDA hit at chunk %i %i, block %i %i %i.\n",
                          curr_pos.x, curr_pos.y, cell.x, cell.y, cell.z);

            switch (action) {
            case EDIT_BREAK:
                cd->types[cell.x][cell.y][cell.z] = CUBETYPE_AIR;
                CS_REQUEST(cs, CSREQ_REMESH, curr_pos);
                break;
            case EDIT_PLACE:
            {
                if (_get_adjacent_cell(&cell, &curr_pos, &data_pos, side_hit))
                {
                    cd = chunk_data[data_pos.x][data_pos.y];
                    cd->types[cell.x][cell.y][cell.z] = CUBETYPE_SAND;
                    CS_REQUEST(cs, CSREQ_REMESH, curr_pos);
                }
                break;
            }
            default:
                ENGINE_LOG_WARN("No action.\n", NULL);
                break;
            };

            if (cell.x == 0)
                CS_REQUEST(cs, CSREQ_REMESH, REL_W(curr_pos));
            else if (cell.x == CHUNK_SIZE - 1)
                CS_REQUEST(cs, CSREQ_REMESH, REL_E(curr_pos));

            if (cell.z == 0)
                CS_REQUEST(cs, CSREQ_REMESH, REL_S(curr_pos));
            else if (cell.z == CHUNK_SIZE - 1)
                CS_REQUEST(cs, CSREQ_REMESH, REL_N(curr_pos));

            chunk_sys_return_surrounding_data(cs, chunk_data);
            return true;
        }

        if (len.x < len.y && len.x < len.z) // x is smallest
        {
            side_hit = (step.x > 0) ? FACEIDX_LEFT : FACEIDX_RIGHT;
            len.x += inv_dir.x;
            distance.x += inv_dir.x;

            if (cell.x == CHUNK_SIZE - 1 && step.x > 0)
            {
                cell.x = 0;
                curr_pos = REL_E(curr_pos);
                data_pos.x += 1;
            }
            else if (cell.x == 0 && step.x < 0)
            {
                cell.x = CHUNK_SIZE - 1;
                curr_pos = REL_W(curr_pos);
                data_pos.x -= 1;
            }
            else 
            {
                cell.x += step.x;
            }
        }
        else if (len.y < len.z) // y is smallest
        {
            side_hit = (step.y > 0) ? FACEIDX_BOTTOM : FACEIDX_TOP;
            len.y += inv_dir.y;
            distance.y += inv_dir.y;

            if ((cell.y == CHUNK_HEIGHT - 1 && step.y == 1) || (cell.y == 0 && step.y == -1))
            {
                chunk_sys_return_surrounding_data(cs, chunk_data);
                ENGINE_LOG_WARN("DDA miss (oob).\n", NULL); 
                return false;
            }
            else 
            {
                cell.y += step.y;
            }
        }
        else // z is smallest
        {
            side_hit = (step.z > 0) ? FACEIDX_BACK : FACEIDX_FRONT;
            len.z += inv_dir.z;
            distance.z += inv_dir.z;

            if (cell.z == CHUNK_SIZE - 1 && step.z > 0)
            {
                cell.z = 0;
                curr_pos = REL_N(curr_pos);
                data_pos.y -= 1;
            }
            else if (cell.z == 0 && step.z < 0)
            {
                cell.z = CHUNK_SIZE - 1;
                curr_pos = REL_S(curr_pos);
                data_pos.y += 1;
            }
            else 
            {
                cell.z += step.z;
            }
        }
    }

    chunk_sys_return_surrounding_data(cs, chunk_data);
    ENGINE_LOG_WARN("DDA miss (iterated through).\n", NULL); 
    return false;
}
