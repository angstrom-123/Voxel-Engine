#include "raycast.h"

hit_desc_t raycast(chunk_system_t *cs, const raycast_desc_t *desc)
{
    const float STEP_SIZE = 0.02;

    vec3 ray_pos = desc->origin;
    float ray_len = 0.0;

    /* Used for checking if the ray has gone across a chunk boundary. */
    ivec2 prev_chunk = {
        floorf(desc->origin.x / (float) CHUNK_SIZE) * CHUNK_SIZE,
        floorf(desc->origin.z / (float) CHUNK_SIZE) * CHUNK_SIZE
    };

    vec3 step = em_mul_vec3_f(em_normalize_vec3(desc->direction), STEP_SIZE);

    ivec2 data_idx = {1, 1}; // Middle is current chunk, so 1, 1.
    chunk_data_t *cd[3][3];
    chunk_sys_borrow_surrounding_data(cs, prev_chunk, cd);

    hit_desc_t hd = {
        .hit = false
    };

    while (ray_len < desc->range)
    {
        for (size_t i = 0; i < 3; i++)
        {
            ray_pos.elements[i] += step.elements[i];

            /* Update raycast state. */
            ivec2 chunk = {
                floorf(ray_pos.x / (float) CHUNK_SIZE) * CHUNK_SIZE,
                floorf(ray_pos.z / (float) CHUNK_SIZE) * CHUNK_SIZE
            };
            ivec3 cell = {
                floorf(ray_pos.x) - chunk.x,
                floorf(ray_pos.y),
                floorf(ray_pos.z) - chunk.y
            };

            int32_t data_delta;
            int32_t delta = em_sign_vec3(desc->direction).elements[i];
            switch (i) {
            case 0:
                data_delta = (chunk.x - prev_chunk.x) / CHUNK_SIZE;
                data_idx.x += data_delta;
                break;
            case 1:
                break;
            case 2:
                data_delta = (chunk.y - prev_chunk.y) / CHUNK_SIZE;
                data_idx.y -= data_delta;
                break;
            };

            prev_chunk = chunk;

            if (delta == 0) // Still in the same cell, continue
                continue;

            if (cell.y > CHUNK_HEIGHT - 1 || cell.y < 0) // Out of bounds, continue
                continue;

            switch (i) {
            case 0: // x axis
                hd.side = (delta == 1) 
                        ? FACEIDX_LEFT
                        : FACEIDX_RIGHT;
                break;
            case 1: // y axis
                hd.side = (delta == 1) 
                        ? FACEIDX_BOTTOM
                        : FACEIDX_TOP;
                break;
            case 2: // z axis
                hd.side = (delta == 1) 
                        ? FACEIDX_BACK
                        : FACEIDX_FRONT;
                break;
            default:
                ENGINE_ASSERT(false, "Invalid raycast step reached");
                break;
            };

            /* Handle hit. */
            uint8_t *type = &cd[data_idx.x][data_idx.y]->types[cell.x][cell.y][cell.z];
            if (*type != CUBETYPE_AIR)
            {
                hd.hit = true;
                hd.cell = cell;
                hd.ccord = chunk;

                goto end;
            }
        }

        ray_len += STEP_SIZE;
    }

    end:
        chunk_sys_return_surrounding_data(cs, cd);
        return hd;
}
