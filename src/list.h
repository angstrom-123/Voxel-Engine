#ifndef LIST_H
#define LIST_H

#include <libem/em_list.h>
#include "geometry_types.h"

DECLARE_DLL(chunk_t, chunk)

#ifndef MY_DLL_IMPL

DEFINE_DLL(chunk_t, chunk);

#endif // MY_DLL_IMPL

#endif
