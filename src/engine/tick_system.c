#include "tick_system.h"

static void _handle_tick(tick_system_t *ts, load_system_t *ls)
{
    (void) ts;
    (void) ls;
}

static int _thread_func(void *args)
{
    stm_setup();

    tick_system_thread_args_t *targs = args;
    tick_system_t *ts = targs->ts;
    load_system_t *ls = targs->ls;

    double period_ms = (1.0 / ts->tps) * 1000.0;
    uint64_t last_time = stm_now();

    ENGINE_LOG_OK("Tick thread init.\n", NULL);
    atomic_store(&ts->thread_ready, true);

    while (atomic_load(&ts->running))
    {
        uint64_t curr_time = stm_now();
        double elapsed_ms = stm_ms(stm_diff(curr_time, last_time));

        if (elapsed_ms >= period_ms)
        {
            last_time = curr_time;
            _handle_tick(ts, ls);
        }
    }

    ENGINE_LOG_WARN("Tick thread terminating.\n", NULL);

    return 0;
}

void tick_sys_init(tick_system_t *ts, const tick_system_desc_t *desc)
{
    ts->tps = desc->tps;
}

void tick_sys_init_thread(tick_system_t *ts, tick_system_thread_args_t *targs)
{
    ts->running = true;
    int res = thrd_create(&ts->worker, _thread_func, targs);
    ENGINE_ASSERT(res == thrd_success, "Failed to initialize worker thread.\n");

    /* Block until the thread is ready. */
    while (!atomic_load(&ts->thread_ready))
    {
        thrd_sleep(&(struct timespec) {
            .tv_nsec = THREAD_AWAIT_NS
        }, NULL);
    }
}

void tick_sys_cleanup(tick_system_t *ts)
{
    atomic_store(&ts->running, false);
    thrd_join(ts->worker, NULL);
}
