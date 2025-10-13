#ifndef TEST_H
#define TEST_H

#include <libem/em_hashmap.h>
#include <libem/em_double_list.h>
#include <libem/em_circular_queue.h>
#include <libem/em_array_list.h>

DECLARE_HASHMAP(char, int32_t, c2i)
DECLARE_HASHMAP_CMP(char, c)
DECLARE_HASHMAP_HSH(char, c)
DECLARE_HASHMAP_CLN_K(char, c)
DECLARE_HASHMAP_CLN_V(int32_t, i)
typedef struct test_predicate_args {
    int32_t max;
} test_predicate_args_t;
bool test_predicate(const int32_t *i, const void *args);

DECLARE_DOUBLE_LIST(int32_t, i)
DECLARE_DOUBLE_LIST_CLN(int32_t, i)

DECLARE_CIRCULAR_QUEUE(int32_t, i)
DECLARE_CIRCULAR_QUEUE_CLN(int32_t, i)

DECLARE_ARRAY_LIST(int32_t, i)
DECLARE_ARRAY_LIST_CLN(int32_t, i)

extern void test_main(void);

#ifdef TEST_IMPL

DEFINE_HASHMAP(char, int32_t, c2i)
DEFINE_HASHMAP_CMP(char, c)
{
    return *lhs == *rhs;
}
DEFINE_HASHMAP_HSH(char, c)
{
    return em_hash((int32_t) *key);
}
DEFINE_HASHMAP_CLN_K(char, c)
{
    free(key);
}
DEFINE_HASHMAP_CLN_V(int32_t, i)
{
    free(val);
}
bool test_predicate(const int32_t *i, const void *args)
{
    const test_predicate_args_t *p_args = args;
    return (*i > p_args->max);
}

DEFINE_DOUBLE_LIST(int32_t, i)
DEFINE_DOUBLE_LIST_CLN(int32_t, i)
{
    free(val);
}

DEFINE_CIRCULAR_QUEUE(int32_t, i)
DEFINE_CIRCULAR_QUEUE_CLN(int32_t, i)
{
    free(val);
}

DEFINE_ARRAY_LIST(int32_t, i)
DEFINE_ARRAY_LIST_CLN(int32_t, i)
{
    free(val);
}

#endif // TEST_IMPL

#endif
