#ifndef LIST_H
#define LIST_H

#include <libem/em_list.h>

DECLARE_DLL(offset_t, offset)

#ifdef MY_DLL_IMPL

DEFINE_DLL(offset_t, offset);

#endif // MY_DLL_IMPL

#endif
