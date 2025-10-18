#ifndef MAIN_H
#define MAIN_H

#define SOKOL_IMPL
#include <sokol/sokol_gfx.h>
#include <sokol/sokol_app.h>
#include <sokol/sokol_glue.h>
#include <sokol/sokol_log.h>
#include <sokol/sokol_time.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_IMPLEMENTATION
#include <nuklear/nuklear.h>
#include <sokol/sokol_nuklear.h>

#include <libem/em_impl.h>
#define MY_HASHMAP_IMPL
#include "hashmap.h"
#define MY_DLL_IMPL
#include "dlist.h"
#define MY_CQ_IMPL
#include "queue.h"
#define MY_AL_IMPL
#include "alist.h"

#include "engine.h"
#include "instrumentor.h"

#endif
