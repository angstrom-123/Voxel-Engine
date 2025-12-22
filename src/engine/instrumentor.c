#include "instrumentor.h"

static FILE *prof_fptr = NULL;
static uint64_t last_sample;

void instrumentor_begin_session(const char *filename)
{
    last_sample = 0;
    stm_setup();

    char full_name[strlen(filename) + 6];
    sprintf(full_name, "%s.tracing.json", filename);

    prof_fptr = fopen(full_name, "w");
    fprintf(prof_fptr, "[");
}

void instrumentor_end_session(void)
{
    /* Write dummy end event to ensure the JSON is valid by closing with a "]". */
    const char *format = QUOTE(
        {
            "name": "end",
            "cat": "special",
            "ph": "X",
            "ts": %u,
            "dur": 1000,
            "pid": 1234,
            "tid": 0
        }]
    );
    fprintf(prof_fptr, format, stm_us(stm_now()));
    fclose(prof_fptr);
    prof_fptr = NULL;
}

void instrumentor_write_profile(profiler_datapoint_t dp)
{
    if (!prof_fptr)
        return;

    const char *format = QUOTE(
        {
            "name": "%s",
            "cat": "%s",
            "ph": "%s",
            "ts": %u,
            "dur": %u,
            "pid": %u,
            "tid": %u
        },
    );
    fprintf(prof_fptr, format, dp.name, dp.cat, dp.ph, dp.ts, dp.dur, dp.pid, dp.tid);
}

uint64_t instrumentor_sample_tick(void)
{
    /* Stop multiple samples returning same time which helps the flamegraph. */
    uint64_t sample = stm_now();
    sample = (sample > last_sample) ? sample : last_sample + 1;
    last_sample = sample;
    return sample;
}

profiler_datapoint_t instrumentor_sample_since(const char *name, uint64_t start_tick)
{
    return (profiler_datapoint_t){
        .name = name,
        .cat = "default",
        .ph = "X",
        .ts = stm_us(start_tick),
        .dur = stm_us(stm_since(start_tick)),
        .pid = 1234,
        .tid = thrd_current()};
}
