#ifndef LOAD_MANAGER_H
#define LOAD_MANAGER_H

#include "chunk_system.h"

typedef enum shell_type {
    SHELL_RIM,
    SHELL_OUT,
    SHELL_NUM
} shell_type_e;

typedef struct load_manager {
    shell_t shells[SHELL_NUM];
    ivec2 curr_pos;
    size_t load_dist;
} load_manager_t;

typedef struct load_manager_desc {
    ivec2 start_pos;
    size_t load_dist;
} load_manager_desc_t;

extern void manager_init(load_manager_t *lm, const load_manager_desc_t *desc);
extern void manager_cleanup(load_manager_t *lm);
extern void manager_load_initial(load_manager_t *lm, chunk_system_t *cs);
extern bool manager_update(load_manager_t *lm, chunk_system_t *cs, ivec2 new_pos);

#endif
