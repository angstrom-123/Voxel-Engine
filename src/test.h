#ifndef TEST_H
#define TEST_H

#include <libem/em_hashmap.h>
#include <libem/em_list.h>

#include <stdint.h>

DECLARE_HASHMAP(char, int32_t, c2i)
DECLARE_HASHMAP_CMP(char, c2i)
DECLARE_HASHMAP_HSH(char, c2i)

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

DEFINE_DLL(int32_t, int)

#endif // TEST_IMPL

#endif
