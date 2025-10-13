#ifndef QUEUE_H
#define QUEUE_H

#include <libem/em_circular_queue.h>
#include "chunk_system_types.h" // request / result

DECLARE_CIRCULAR_QUEUE(cs_request_t, cs_request)
DECLARE_CIRCULAR_QUEUE_CLN(cs_request_t, cs_request)

DECLARE_CIRCULAR_QUEUE(cs_result_t, cs_result)
DECLARE_CIRCULAR_QUEUE_CLN(cs_result_t, cs_result)

DECLARE_CIRCULAR_QUEUE(offset_t, offset)
DECLARE_CIRCULAR_QUEUE_CLN(offset_t, offset)

#ifdef MY_CQ_IMPL
DEFINE_CIRCULAR_QUEUE(cs_result_t, cs_result)
DEFINE_CIRCULAR_QUEUE_CLN(cs_result_t, cs_result)
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

#endif // MY_CQ_IMPL

#endif
