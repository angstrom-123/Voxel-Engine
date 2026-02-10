#ifndef WORLD_GEN_H
#define WORLD_GEN_H

#include <libem/em_perlin.h>
#include <libem/em_math.h>
#include <libem/em_random.h>

#include "files.h"
#include "geometry_types.h"
#include "chunk_system_types.h"
#include "base.h"
#include "physics.h"

#define PERLIN_SCALE 0.004
#define PERLIN_OCTAVES 6
#define TREE_THRESHOLD 0.9

typedef struct gen_tree {
    int32_t size_x;
    int32_t size_y;
    int32_t size_z;
    cube_type_e ***types;
} gen_tree_t;

static gen_tree_t tree_basic;

extern bool load_model_files();
extern void unload_model_files();
extern chunk_data_t *generate_chunk_data(ivec2 pos, uint32_t seed);

#endif
