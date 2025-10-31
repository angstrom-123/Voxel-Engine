#ifndef RAYCAST_H
#define RAYCAST_H

#include "chunk_system.h"
#include "base.h"
#include <signal.h>

#include <stdbool.h>
#include <stdint.h>

#include <libem/em_math.h>

typedef struct raycast_desc {
    vec3 origin;
    vec3 direction;
    uint16_t range;
} raycast_desc_t;

typedef struct hit_desc {
    ivec2 ccord;
    ivec3 cell;
    chunk_data_t *data;
    cube_face_idx_e side;
} hit_desc_t;

typedef enum chunk_edit {
    EDIT_NONE,
    EDIT_PLACE,
    EDIT_BREAK,
    EDIT_NUM
} chunk_edit_e;

extern bool raycast_dda(chunk_system_t *cs, chunk_edit_e action, const raycast_desc_t *desc);

#endif
