#ifndef WORLD_CREATION_H
#define WORLD_CREATION_H

#include "engine.h"
#include "files.h"
#include "base.h"

#include <stdint.h>
#include <string.h>

extern void world_new(engine_t *e, const char *name, const uint32_t seed);
extern void world_load(engine_t *e, const char *name);
extern void world_rename(engine_t *e, const char *name, const char *new_name);
extern void world_delete(engine_t *e, const char *name);
extern void world_print_help(void);

#endif
