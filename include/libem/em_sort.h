#ifndef EM_SORT_H
#define EM_SORT_H

#ifndef EM_SORT_INCLUDED
#define EM_SORT_INCLUDED 

#include "em_global.h"

#include <stddef.h>

typedef int (*sort_cmp)(const void *lhs, const void *rhs);

extern void em_quicksort(void *arr, size_t n, sort_cmp cmp);

#endif // EM_SORT_INCLUDED

/*     ______  _______  __    ________  __________   ___________  ______________  _   __
 *    /   /  |/  / __ \/ /   / ____/  |/  / ____/ | / /_  __/   |/_  __/   / __ \/ | / /
 *    / // /|_/ / /_/ / /   / __/ / /|_/ / __/ /  |/ / / / / /| | / /  / // / / /  |/ /
 *   / // /  / / ____/ /___/ /___/ /  / / /___/ /|  / / / / ___ |/ /  / // /_/ / /|  /
 * /___/_/  /_/_/   /_____/_____/_/  /_/_____/_/ |_/ /_/ /_/  |_/_/ /___/\____/_/ |_/
 *
 */

#ifdef EM_SORT_IMPL

void _swap(void **lhs, void **rhs)
{
    void *tmp = *lhs;
    *lhs = *rhs;
    *rhs = tmp;
}

size_t _quicksort_partition(void *arr, size_t l, size_t r, sort_cmp cmp)
{
    void *piv = &arr[r];
    size_t i = l - 1;
    for (size_t j = l; j < r; j++)
    {
        if (cmp(&arr[j], piv) <= 0)
        {
            i++;
            _swap(&arr[i], &arr[j]);
        }
    }
    _swap(&arr[i + 1], &arr[r]);
    return i + 1;
}

void _quicksort(void *arr, size_t l, size_t r, sort_cmp cmp)
{
    if (l < r)
    {
        size_t p = _quicksort_partition(arr, l, r, cmp);
        _quicksort(arr, l, p - 1, cmp);
        _quicksort(arr, p + 1, r, cmp);
    }
}

void em_quicksort(void *arr, size_t n, sort_cmp cmp)
{
    _quicksort(arr, 0, n - 1, cmp);
}

#endif // EM_SORT_IMPL

#endif
