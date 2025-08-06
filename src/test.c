#include "test.h"

bool _list_equals(em_dll_int_t *l, const int32_t *e, size_t len)
{
    if (l->count != len) 
    {
        fprintf(stderr, "Lists not equal. Length mismatch: %zu vs %zu\n",
                l->count, len);
        return false;
    }

    em_dll_iter_t *it = l->iterator(l);
    size_t idx = 0;
    while (it->has_next)
    {
        if (idx >= len) 
        {
            fprintf(stderr, "Lists not equal. Index out of bounds: %zu vs %zu\n",
                    idx, len);
            free(it);
            return false;
        }
        if (*(int32_t *) it->next(it) != e[idx++])
        {
            fprintf(stderr, "List values not equal: %i vs %i\n", 
                    *(int32_t *) it->_curr->val, e[idx - 1]);
            free(it);
            return false;
        }
    }

    free(it);
    return true;
}

void _assert_list(em_dll_int_t *l, const int32_t *e, size_t len)
{
    if (!_list_equals(l, e, len))
    {
        em_dll_iter_t *it = l->iterator(l);
        size_t idx = 0;
        int32_t res[len];
        while (it->has_next)
        {
            if (idx >= len)
                break;

            res[idx++] = *(int32_t *) it->next(it);
        }

        free(it);

        fprintf(stderr, "DLL Assertion failed.\n");
        fprintf(stderr, " Actual | Expected\n");
        for (size_t i = 0; i < len; i++)
            fprintf(stderr, "idx %zu: %i | %i\n", i, res[i], e[i]);
        exit(1);
    }
}

bool _hmap_equals(em_hashmap_c2i_t *m, const char *ec, 
                  const int32_t *ei, size_t len)
{
    if (m->count != len) 
    {
        fprintf(stderr, "Maps not equal. Length mismatch: %zu vs %zu\n",
                m->count, len);
        return false;
    }

    for (size_t i = 0; i < len; i++) 
    {
        int32_t v = m->get(m, ec[i]);
        if (v != ei[i])
        {
            fprintf(stderr, "Map values not equal: %i vs %c->%i\n", 
                v, ec[i], ei[i]);
            return false;
        }
    }

    em_hashmap_iter_t *it = m->iterator(m);
    size_t ctr = 0;
    while (it->has_next)
    {
        em_hashmap_entry_t *e = it->get(it);
        int32_t v = *(int32_t *) e->val;
        bool found = false;
        for (size_t i = 0; i < len; i++) 
        {
            if (v == ei[i])
            {
                found = true;
                ctr++;
            }
        }

        if (!found) 
        {
            free(it);
            return false;
        }

        it->next(it);
    }
    free(it);

    if (ctr != m->count)
        return false;

    return true;
}

void _assert_hmap(em_hashmap_c2i_t *m, const char *ec, 
                  const int32_t *ei, size_t len)
{
    if (!_hmap_equals(m, ec, ei, len))
    {
        fprintf(stderr, "HashMap assertion failed.\n");
        exit(1);
    }
}

void _test_dll_i0(void)
{
    em_dll_int_t *list = DLL_NEW(int)
    printf("0/7 ");

    printf("\r1/7 ");
    list->insert_at(list, 0, 2);
    _assert_list(list, (int32_t[]) {2}, 1);
    printf("\r2/7 ");
    list->insert_at(list, 0, 1);
    _assert_list(list, (int32_t[]) {1, 2}, 2);
    printf("\r3/7 ");
    list->append(list, 4);
    _assert_list(list, (int32_t[]) {1, 2, 4}, 3);
    printf("\r4/7 ");
    list->insert_before(list, list->get_node(list, 2), 3);
    _assert_list(list, (int32_t[]) {1, 2, 3, 4}, 4);
    printf("\r5/7 ");
    list->insert_after(list, list->get_node(list, 3), 5);
    _assert_list(list, (int32_t[]) {1, 2, 3, 4, 5}, 5);
    printf("\r6/7 ");
    list->append(list, 6);
    _assert_list(list, (int32_t[]) {1, 2, 3, 4, 5, 6}, 6);
    printf("\r7/7 ");
    list->insert_at(list, 6, 7);
    _assert_list(list, (int32_t[]) {1, 2, 3, 4, 5, 6, 7}, 7);

    list->destroy(list);
}

void _test_dll_i1(void)
{
    em_dll_int_t *list = DLL_NEW(int)
    printf("0/7 ");

    printf("\r1/7 ");
    list->append(list, 4);
    _assert_list(list, (int32_t[]) {4}, 1);
    printf("\r2/7 ");
    list->insert_before(list, list->get_node(list, 0), 2);
    _assert_list(list, (int32_t[]) {2, 4}, 2);
    printf("\r3/7 ");
    list->insert_after(list, list->get_node(list, 0), 3);
    _assert_list(list, (int32_t[]) {2, 3, 4}, 3);
    printf("\r4/7 ");
    list->append(list, 6);
    _assert_list(list, (int32_t[]) {2, 3, 4, 6}, 4);
    printf("\r5/7 ");
    list->insert_at(list, 0, 1);
    _assert_list(list, (int32_t[]) {1, 2, 3, 4, 6}, 5);
    printf("\r6/7 ");
    list->insert_at(list, 5, 7);
    _assert_list(list, (int32_t[]) {1, 2, 3, 4, 6, 7}, 6);
    printf("\r7/7 ");
    list->insert_at(list, 4, 5);
    _assert_list(list, (int32_t[]) {1, 2, 3, 4, 5, 6, 7}, 7);

    list->destroy(list);
}

void _test_dll_d0(void)
{
    em_dll_int_t *list = DLL_NEW(int)
    printf("0/8 ");

    printf("\r1/8 ");
    list->append(list, 1);
    list->append(list, 2);
    list->append(list, 3);
    list->append(list, 4);
    list->append(list, 5);
    list->append(list, 6);
    list->append(list, 7);
    _assert_list(list, (int32_t[]) {1, 2, 3, 4, 5, 6, 7}, 7);
    printf("\r2/8 ");
    list->pop_first(list);
    _assert_list(list, (int32_t[]) {2, 3, 4, 5, 6, 7}, 6);
    printf("\r3/8 ");
    list->pop_last(list);
    _assert_list(list, (int32_t[]) {2, 3, 4, 5, 6}, 5);
    printf("\r4/8 ");
    list->pop(list, 3);
    _assert_list(list, (int32_t[]) {2, 3, 4, 6}, 4);
    printf("\r5/8 ");
    list->remove(list, 0);
    _assert_list(list, (int32_t[]) {3, 4, 6}, 3);
    printf("\r6/8 ");
    list->remove(list, 2);
    _assert_list(list, (int32_t[]) {3, 4}, 2);
    printf("\r7/8 ");
    list->remove_node(list, list->get_node(list, 1));
    _assert_list(list, (int32_t[]) {3}, 1);
    printf("\r8/8 ");
    list->pop_last(list);
    _assert_list(list, (int32_t[]) {}, 0);

    list->destroy(list);
}

void _test_dll_d1(void)
{
    em_dll_int_t *list = DLL_NEW(int)
    printf("0/8 ");

    printf("\r1/8 ");
    list->append(list, 1);
    list->append(list, 2);
    list->append(list, 3);
    list->append(list, 4);
    list->append(list, 5);
    list->append(list, 6);
    list->append(list, 7);
    _assert_list(list, (int32_t[]) {1, 2, 3, 4, 5, 6, 7}, 7);
    printf("\r2/8 ");
    list->remove_node(list, list->get_node(list, 3));
    _assert_list(list, (int32_t[]) {1, 2, 3, 5, 6, 7}, 6);
    printf("\r3/8 ");
    list->pop_first(list);
    _assert_list(list, (int32_t[]) {2, 3, 5, 6, 7}, 5);
    printf("\r4/8 ");
    list->pop_first(list);
    _assert_list(list, (int32_t[]) {3, 5, 6, 7}, 4);
    printf("\r5/8 ");
    list->remove_node(list, list->get_node(list, 3));
    _assert_list(list, (int32_t[]) {3, 5, 6}, 3);
    printf("\r6/8 ");
    list->pop_last(list);
    _assert_list(list, (int32_t[]) {3, 5}, 2);
    printf("\r7/8 ");
    list->pop(list, 0);
    _assert_list(list, (int32_t[]) {5}, 1);
    printf("\r8/8 ");
    list->pop_first(list);
    _assert_list(list, (int32_t[]) {}, 0);

    list->destroy(list);
}

void _test_hmap_i0(void)
{
    em_hashmap_c2i_t *map = HASHMAP_NEW(c2i, 20, HASHMAP_CMP(c2i), HASHMAP_HSH(c2i))
    printf("0/4 ");

    printf("\r1/4 ");
    map->put(map, 'a', 1);
    _assert_hmap(map, (char[]) {'a'}, (int32_t[]) {1}, 1);
    printf("\r2/4 ");
    map->put(map, 'b', 2);
    _assert_hmap(map, (char[]) {'a', 'b'}, (int32_t[]) {1, 2}, 2);
    printf("\r3/4 ");
    map->put(map, 'c', 3);
    _assert_hmap(map, (char[]) {'a', 'b', 'c'}, (int32_t[]) {1, 2, 3}, 3);
    printf("\r4/4 ");
    map->put(map, 'd', 4);
    _assert_hmap(map, (char[]) {'a', 'b', 'c', 'd'}, (int32_t[]) {1, 2, 3, 4}, 4);

    map->destroy(map);
}

void _test_hmap_i1(void)
{
    em_hashmap_c2i_t *map = HASHMAP_NEW(c2i, 1, HASHMAP_CMP(c2i), HASHMAP_HSH(c2i))
    printf("0/4");

    printf("\r1/4 ");
    map->put(map, 'z', 219);
    _assert_hmap(map, (char[]) {'z'}, (int32_t[]) {219}, 1);
    printf("\r2/4 ");
    map->put(map, 'h', 6);
    _assert_hmap(map, (char[]) {'z', 'h'}, (int32_t[]) {219, 6}, 2);
    printf("\r3/4 ");
    map->put(map, 'p', -123);
    _assert_hmap(map, (char[]) {'z', 'h', 'p'}, (int32_t[]) {219, 6, -123}, 3);
    printf("\r4/4 ");
    map->put(map, 'f', 910);
    _assert_hmap(map, (char[]) {'z', 'h', 'p', 'f'}, (int32_t[]) {219, 6, -123, 910}, 4);

    map->destroy(map);
}

void _test_hmap_d0(void)
{
    em_hashmap_c2i_t *map = HASHMAP_NEW(c2i, 1, HASHMAP_CMP(c2i), HASHMAP_HSH(c2i))
    printf("0/5 ");

    printf("\r1/5 ");
    map->put(map, 'z', 219);
    map->put(map, 'h', 6);
    map->put(map, 'p', -123);
    map->put(map, 'f', 910);
    _assert_hmap(map, (char[]) {'z', 'h', 'p', 'f'}, (int32_t[]) {219, 6, -123, 910}, 4);
    printf("\r2/5 ");
    map->remove(map, 'z');
    _assert_hmap(map, (char[]) {'h', 'p', 'f'}, (int32_t[]) {6, -123, 910}, 3);
    printf("\r3/5 ");
    map->remove(map, 'p');
    _assert_hmap(map, (char[]) {'h', 'f'}, (int32_t[]) {6, 910}, 2);
    printf("\r4/5 ");
    map->remove(map, 'f');
    _assert_hmap(map, (char[]) {'h'}, (int32_t[]) {6}, 1);
    printf("\r5/5 ");
    map->remove(map, 'h');
    _assert_hmap(map, (char[]) {}, (int32_t[]) {}, 0);

    map->destroy(map);
}

void _test_hmap_d1(void)
{
    em_hashmap_c2i_t *map = HASHMAP_NEW(c2i, 1, HASHMAP_CMP(c2i), HASHMAP_HSH(c2i))
    printf("0/5 ");

    printf("\r1/5 ");
    map->put(map, 'a', 1);
    map->put(map, 'b', 2);
    map->put(map, 'c', 3);
    map->put(map, 'd', 4);
    map->put(map, 'e', 5);
    map->put(map, 'f', 6);
    map->put(map, 'g', 7);
    map->put(map, 'h', 8);
    _assert_hmap(map, (char[]) {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'}, 
                 (int32_t[]) {1, 2, 3, 4, 5, 6, 7, 8}, 8);
    printf("\r2/5 ");
    map->remove(map, 'd');
    _assert_hmap(map, (char[]) {'a', 'b', 'c', 'e', 'f', 'g', 'h'}, 
                 (int32_t[]) {1, 2, 3, 5, 6, 7, 8}, 7);
    printf("\r3/5 ");
    map->remove(map, 'h');
    _assert_hmap(map, (char[]) {'a', 'b', 'c', 'e', 'f', 'g'}, 
                 (int32_t[]) {1, 2, 3, 5, 6, 7}, 6);
    printf("\r4/5 ");
    map->remove(map, 'a');
    _assert_hmap(map, (char[]) {'b', 'c', 'e', 'f', 'g'}, 
                 (int32_t[]) {2, 3, 5, 6, 7}, 5);
    printf("\r5/5 ");
    map->remove(map, 'c');
    _assert_hmap(map, (char[]) {'b', 'e', 'f', 'g'}, 
                 (int32_t[]) {2, 5, 6, 7}, 4);

    map->destroy(map);
}

void _test_dll(void)
{
    printf("Testing DLL Insertions.\n");

    _test_dll_i0();
    printf("Passed.\n");

    _test_dll_i1();
    printf("Passed.\n");

    printf("Testing DLL Deletions.\n");

    _test_dll_d0();
    printf("Passed.\n");

    _test_dll_d1();
    printf("Passed.\n");
}

void _test_hmap(void)
{
    printf("Testing HashMap Insertions.\n");

    _test_hmap_i0();
    printf("Passed.\n");

    _test_hmap_i1();
    printf("Passed.\n");

    printf("Testing HashMap Deletions.\n");

    _test_hmap_d0();
    printf("Passed.\n");

    _test_hmap_d1();
    printf("Passed.\n");
}

void test_main()
{
    _test_dll();
    _test_hmap();
}
