#ifndef DLIST_H
#define DLIST_H

#include <libem/em_double_list.h>
#include <libem/em_math.h>

DECLARE_DOUBLE_LIST(ivec2, ivec2)
DECLARE_DOUBLE_LIST_CLN(ivec2, ivec2)

#ifdef MY_DLL_IMPL

DEFINE_DOUBLE_LIST(ivec2, ivec2)
DEFINE_DOUBLE_LIST_CLN(ivec2, ivec2)
{
    free(val);
}

#endif // MY_DLL_IMPL

#endif
