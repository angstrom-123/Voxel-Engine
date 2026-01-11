#ifndef WORLD_CREATION_H
#define WORLD_CREATION_H

#include "engine.h"
#include "files.h"
#include "base.h"

#include <stdint.h>
#include <string.h>

typedef enum world_create_err {
    ERR_NONE,
    ERR_DUPLICATE_NAME,
    ERR_NOT_EXISTS,
    ERR_GENERIC,
    ERR_NUM
} world_create_err_e;

extern world_create_err_e world_new(engine_t *e, const char *name, const uint32_t seed);
extern world_create_err_e world_load(engine_t *e, const char *name);
extern world_create_err_e world_rename(engine_t *e, const char *name, const char *new_name);
extern world_create_err_e world_delete(engine_t *e, const char *name);
extern void world_print_help(void);

#endif
