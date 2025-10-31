#ifndef QUEUE_H
#define QUEUE_H

#include <libem/em_circular_queue.h>
#include "chunk_system_types.h" // cs_request
#include "update_system_types.h" // us_request

DECLARE_CIRCULAR_QUEUE(cs_request_t, cs_request)
DECLARE_CIRCULAR_QUEUE_CLN(cs_request_t, cs_request)

DECLARE_CIRCULAR_QUEUE(us_request_t, us_request)
DECLARE_CIRCULAR_QUEUE_CLN(us_request_t, us_request)

DECLARE_CIRCULAR_QUEUE(offset_t, offset)
DECLARE_CIRCULAR_QUEUE_CLN(offset_t, offset)

DECLARE_CIRCULAR_QUEUE(us_buffer_pair_t, sokol_buffers);
DECLARE_CIRCULAR_QUEUE_CLN(us_buffer_pair_t, sokol_buffers);

#ifdef MY_CQ_IMPL
DEFINE_CIRCULAR_QUEUE(us_request_t, us_request)
DEFINE_CIRCULAR_QUEUE_CLN(us_request_t, us_request)
{
    free(val);
}

DEFINE_CIRCULAR_QUEUE(cs_request_t, cs_request)
DEFINE_CIRCULAR_QUEUE_CLN(cs_request_t, cs_request)
{
    free(val);
}

DEFINE_CIRCULAR_QUEUE(offset_t, offset)
DEFINE_CIRCULAR_QUEUE_CLN(offset_t, offset)
{
    free(val);
}

DEFINE_CIRCULAR_QUEUE(us_buffer_pair_t, sokol_buffers)
DEFINE_CIRCULAR_QUEUE_CLN(us_buffer_pair_t, sokol_buffers)
{
    sg_destroy_buffer(val->vertex);
    sg_destroy_buffer(val->index);
    free(val);
}

#endif // MY_CQ_IMPL

#endif
