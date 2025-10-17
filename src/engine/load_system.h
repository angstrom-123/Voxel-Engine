#ifndef LOAD_SYSTEM_H
#define LOAD_SYSTEM_H

#include "chunk_system.h"

typedef enum shell_type {
    SHELL_RIM,
    SHELL_OUT,
    SHELL_NUM
} shell_type_e;

typedef struct render_coords {
    ivec2 *coords;
    size_t num;
    ivec2 offset;
} render_coords_t;

typedef struct load_system {
    shell_t shells[SHELL_NUM];
    ivec2 *base_chunk_coords;
    size_t base_chunk_count;
    ivec2 curr_pos;
    size_t load_dist;
} load_system_t;

typedef struct load_system_desc {
    ivec2 start_pos;
    size_t render_dist;
} load_system_desc_t;

extern void load_sys_init(load_system_t *ls, const load_system_desc_t *desc);
extern void load_sys_cleanup(load_system_t *ls);
extern render_coords_t load_sys_get_render_coords(load_system_t *ls);
extern void load_sys_load_initial(load_system_t *ls, chunk_system_t *cs);
extern bool load_sys_update(load_system_t *ls, chunk_system_t *cs, update_system_t *us, ivec2 new_pos);

#endif
