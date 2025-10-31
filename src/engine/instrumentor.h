#ifndef INSTRUMENTOR_H
#define INSTRUMENTOR_H

#if !defined(SOKOL_TIME_INCLUDED) 
#include <sokol/sokol_time.h>
#endif

#include "base.h"

#include <stdint.h>
#include <stdio.h>
#include <threads.h>
#include <string.h>

typedef struct profiler_datapoint {
    const char *name;
    const char *cat;
    const char *ph;
    uint32_t ts;
    uint32_t dur;
    uint32_t pid;
    uint32_t tid;
} profiler_datapoint_t;

#ifdef PROFILING
#define DO_PROFILING 1
#else 
#define DO_PROFILING 0
#endif

#if DO_PROFILING
#define INSTRUMENTOR_SESSION_BEGIN(session_name) instrumentor_begin_session(session_name)
#define INSTRUMENTOR_SESSION_END() instrumentor_end_session()
#define INSTRUMENT_SCOPE_BEGIN(scope_name) uint64_t instrumentor_sample_##scope_name = instrumentor_sample_tick()
#define INSTRUMENT_SCOPE_END(scope_name) instrumentor_write_profile(instrumentor_sample_since(QUOTE(scope_name), instrumentor_sample_##scope_name))
#define INSTRUMENT_FUNC_BEGIN() uint64_t instrumentor_sample_##__func__ = instrumentor_sample_tick()
#define INSTRUMENT_FUNC_END() instrumentor_write_profile(instrumentor_sample_since(__func__, instrumentor_sample_##__func__))
#else 
#define INSTRUMENTOR_SESSION_BEGIN(session_name) 
#define INSTRUMENTOR_SESSION_END() 
#define INSTRUMENT_SCOPE_BEGIN(scope_name) 
#define INSTRUMENT_SCOPE_END(scope_name) 
#define INSTRUMENT_FUNC_BEGIN() 
#define INSTRUMENT_FUNC_END() 
#endif

extern void instrumentor_begin_session(const char *filename);
extern void instrumentor_end_session(void);
extern void instrumentor_write_profile(profiler_datapoint_t datapoint);
extern uint64_t instrumentor_sample_tick(void);
extern profiler_datapoint_t instrumentor_sample_since(const char *name, uint64_t start_tick);

#endif
