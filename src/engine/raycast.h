#ifndef RAYCAST_H
#define RAYCAST_H

#include "chunk_system.h"

#include <stdbool.h>
#include <stdint.h>

#include <libem/em_math.h>

typedef struct raycast_desc {
    vec3 origin;
    vec3 direction;
    float range;
} raycast_desc_t;

typedef struct hit_desc {
    ivec3 cell;
    ivec2 ccord;
    cube_face_idx_e side;
    bool hit;
} hit_desc_t;

extern hit_desc_t raycast(chunk_system_t *cs, const raycast_desc_t *desc);

#endif
