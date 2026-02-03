#ifndef MAIN_H
#define MAIN_H

#define SOKOL_IMPL
#include <sokol/sokol_gfx.h>
#include <sokol/sokol_app.h>
#include <sokol/sokol_glue.h>
#include <sokol/sokol_log.h>
#include <sokol/sokol_time.h>

#define EM_MATH_IMPL
#include <libem/em_math.h>

#define EM_RANDOM_IMPL
#include <libem/em_random.h>

#define EM_PERLIN_IMPL
#include <libem/em_perlin.h>

#define EM_BINARY_IMPL
#include <libem/em_binary.h>

#define EM_BMP_IMPL
#include <libem/em_bmp.h>

#define DATA_STRUCTURES_IMPL
#include "data_structures.h"

#include "engine.h"
#include "app.h"
#include "instrumentor.h"

#endif
