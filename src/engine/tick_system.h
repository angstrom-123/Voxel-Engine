#ifndef TICK_SYSTEM_H
#define TICK_SYSTEM_H

#include "data_structures.h"
#include "include_sokol.h"
#include "load_system.h"
#include "geometry.h"
#include "base.h"
#include "instrumentor.h"

#include <stdatomic.h>
#include <threads.h>

typedef _Atomic(double) atomic_double;

typedef struct tick_system {
    thrd_t worker;
    atomic_bool running;
    atomic_bool thread_ready;
    atomic_bool paused;
    atomic_double cum_dt;
    double dt;
    double tps;
} tick_system_t;

typedef struct tick_system_desc {
    double tps;
} tick_system_desc_t;

typedef struct tick_system_thread_args {
    tick_system_t *ts;
} tick_system_thread_args_t;

extern void tick_sys_init(tick_system_t *ts, const tick_system_desc_t *desc);
extern void tick_sys_cleanup(tick_system_t *ts);
extern void tick_sys_init_thread(tick_system_t *ts, tick_system_thread_args_t *targs);
extern void (*tick_func)(void); // Declared in main.c.

#endif
