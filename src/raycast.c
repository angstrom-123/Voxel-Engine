#include "raycast.h"

bool raycast_dda(state_t *s, chunk_t **hc, uvec3 *hb)
{
    const size_t MAX_STEPS = 10;

    vec3 rd = s->cam.fwd;
    vec3 ro = s->cam.pos;

    HASHMAP(ivec2_chunk) *m_hot = s->buckets[BUCKET_HOT].chunks;
    chunk_t *curr_chunk = m_hot->get_ptr(m_hot, (ivec2) {floorf(ro.x / 16) * 16, 
                                                         floorf(ro.z / 16) * 16});
    vec3 step = {
        .x = (rd.x < 0) ? -1 : 1,
        .y = (rd.y < 0) ? -1 : 1,
        .z = (rd.z < 0) ? -1 : 1
    };
    vec3 rust = {
        .x = 1.0 / em_abs(rd.x),
        .y = 1.0 / em_abs(rd.y),
        .z = 1.0 / em_abs(rd.z)
    };
    uvec3 cell = {
        .x = floorf(ro.x) - curr_chunk->x,
        .y = floorf(ro.y),
        .z = floorf(ro.z) - curr_chunk->z
    };
    vec3 ro_fract = {
        .x = ro.x - floorf(ro.x),
        .y = ro.y - floorf(ro.y),
        .z = ro.z - floorf(ro.z)
    };
    vec3 len = {
        .x = (rd.x < 0) ? ro_fract.x * rust.x : (1.0 - ro_fract.x) * rust.x,
        .y = (rd.y < 0) ? ro_fract.y * rust.y : (1.0 - ro_fract.y) * rust.y,
        .z = (rd.z < 0) ? ro_fract.z * rust.z : (1.0 - ro_fract.z) * rust.z,
    };

    for (size_t i = 0; i < MAX_STEPS; i++)
    {
        cube_type_e type = curr_chunk->blocks->types[cell.x][cell.y][cell.z];
        if (type != CUBETYPE_AIR)
        {
            *hc = curr_chunk;
            *hb = cell;
            return true;
        }

        if (len.x < len.y && len.x < len.z) // x is smallest
        {
            len.x += rust.x;

            if (cell.x == CHUNK_SIZE - 1 && step.x == 1)
            {
                cell.x = 0;
                ivec2 next_pos = {
                    curr_chunk->x + CHUNK_SIZE,
                    curr_chunk->z
                };
                curr_chunk = m_hot->get_ptr(m_hot, next_pos);
            }
            else if (cell.x == 0 && step.x == -1)
            {
                cell.x = CHUNK_SIZE - 1;
                ivec2 next_pos = {
                    curr_chunk->x - CHUNK_SIZE,
                    curr_chunk->z
                };
                curr_chunk = m_hot->get_ptr(m_hot, next_pos);
            }
            else 
            {
                cell.x += step.x;
            }
        }
        else if (len.y < len.z) // y is smallest
        {
            len.y += rust.y;

            if ((cell.y == CHUNK_HEIGHT - 1 && step.y == 1)
               || (cell.y == 0 && step.y == -1))
                return false;
            else 
                cell.y += step.y;
        }
        else // z is smallest
        {
            len.z += rust.z;

            if (cell.z == CHUNK_SIZE - 1 && step.z == 1)
            {
                cell.z = 0;
                ivec2 next_pos = {
                    curr_chunk->x,
                    curr_chunk->z + CHUNK_SIZE
                };
                curr_chunk = m_hot->get_ptr(m_hot, next_pos);
            }
            else if (cell.z == 0 && step.z == -1)
            {
                cell.z = CHUNK_SIZE - 1;
                ivec2 next_pos = {
                    curr_chunk->x,
                    curr_chunk->z - CHUNK_SIZE
                };
                curr_chunk = m_hot->get_ptr(m_hot, next_pos);
            }
            else 
            {
                cell.z += step.z;
            }
        }
    }
    return false;
}
