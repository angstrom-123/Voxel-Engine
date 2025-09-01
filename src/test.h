#ifndef TEST_H
#define TEST_H

#include <libem/em_hashmap.h>
#include <libem/em_list.h>

#include <stdint.h>

DECLARE_HASHMAP(char, int32_t, c2i)
DECLARE_HASHMAP_CMP(char, c2i)
DECLARE_HASHMAP_HSH(char, c2i)

typedef struct test_predicate_args {
    int32_t max;
} test_predicate_args_t;

bool test_predicate(int32_t *i, void *args);

DECLARE_DLL(int32_t, int)

extern void test_main(void);

#ifdef TEST_IMPL

DEFINE_HASHMAP(char, int32_t, c2i)
DEFINE_HASHMAP_CMP(char, c2i)
{
    return *lhs == *rhs;
}
DEFINE_HASHMAP_HSH(char, c2i)
{
    return em_hash((int32_t) *key);
}
bool test_predicate(int32_t *i, void *args)
{
    test_predicate_args_t *p_args = args;
    return (*i > p_args->max);
}

DEFINE_DLL(int32_t, int)

#endif // TEST_IMPL

#endif
