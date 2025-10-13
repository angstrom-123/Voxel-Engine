#include "test.h"

bool _list_equals(DOUBLE_LIST(i) *l, const int32_t *e, size_t len)
{
    if (l->count != len) 
    {
        fprintf(stderr, "Lists not equal. Length mismatch: %zu vs %zu\n",
                l->count, len);
        return false;
    }

    em_double_list_iter_t *it = l->iterator(l);
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

        int32_t v = *(int32_t *) it->get(it)->val;
        if (v != e[idx++])
        {
            fprintf(stderr, "List values not equal: %i vs %i\n", v, e[idx - 1]);
            free(it);
            return false;
        }

        it->next(it);
    }

    free(it);
    return true;
}

void _assert_list(DOUBLE_LIST(i) *l, const int32_t *e, size_t len)
{
    if (!_list_equals(l, e, len))
    {
        fprintf(stderr, "DLL Assertion failed.\n");
        exit(1);
    }
}

bool _hmap_equals(HASHMAP(c2i) *m, const char *ec, const int32_t *ei, size_t len)
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

    bool *found = calloc(len, sizeof(bool));
    em_hashmap_iter_t *it = m->iterator(m);
    while (it->has_next)
    {
        em_hashmap_entry_t *e = it->get(it);
        char k = *(char *) e->key;
        int32_t v = *(int32_t *) e->val;

        size_t i;
        for (i = 0; i < len; i++)
        {
            if (!found[i] && ec[i] == k && ei[i] == v)
            {
                found[i] = true;
                break;
            }
        }
        if (!found[i])
        {
            fprintf(stderr, "HashMap iterator assertion failed.\n");
            fprintf(stderr, "Expected: \n");
            for (i = 0; i < len; i++)
                fprintf(stderr, "%c->%i\n", ec[i], ei[i]);
            fprintf(stderr, "Actual: \n");
            em_hashmap_iter_t *it2 = m->iterator(m);
            while (it2->has_next)
            {
                em_hashmap_entry_t *e2 = it2->get(it2);
                fprintf(stderr, "%zu: %c->%i\n", it2->_idx, *(char *) e2->key, *(int32_t *) e2->val);
                it2->next(it2);
            }
            return false;
        }

        it->next(it);
    }
    free(it);

    return true;
}

void _assert_hmap(HASHMAP(c2i) *m, const char *ec, const int32_t *ei, size_t len)
{
    if (!_hmap_equals(m, ec, ei, len))
    {
        fprintf(stderr, "HashMap assertion failed.\n");
        exit(1);
    }
}

bool _cq_equals(CIRCULAR_QUEUE(i) *q, const int32_t *ei, size_t len)
{
    if (q->count != len)
    {
        fprintf(stderr, "Queues not equal. Length mismatch: %zu vs %zu\n",
                q->count, len);
        return false;
    }

    em_circular_queue_iter_t *it = q->iterator(q);
    for (size_t i = 0; i < len; i++)
    {
        int32_t v = *(int32_t *) it->get(it)->val;

        if (v != ei[i])
        {
            fprintf(stderr, "Queue iterator assertion failed.\n");
            fprintf(stderr, "Expected: \n");
            for (i = 0; i < len; i++)
                fprintf(stderr, "%i\n", ei[i]);
            fprintf(stderr, "Actual: \n");
            em_circular_queue_iter_t *it2 = q->iterator(q);
            while (it2->has_next)
            {
                em_circular_queue_node_t *e2 = it2->get(it2);
                fprintf(stderr, "%zu: %i\n", it2->_idx, *(int32_t *) e2->val);
                it2->next(it2);
            }
            free(it2);
            
            fprintf(stderr, "Dumping queue.\n");
            for (size_t i = 0; i < q->size; i++)
            {
                em_circular_queue_node_t *n = q->_cq->_entries[i];
                if (n) fprintf(stderr, "%zu: %i\n", i, *(int32_t *) n->val);
                else fprintf(stderr, "%zu: NULL\n", i);
            }

            return false;
        }

        it->next(it);
    }
    free(it);

    return true;
}

void _assert_cq(CIRCULAR_QUEUE(i) *q, const int32_t *ei, size_t len)
{
    if (!_cq_equals(q, ei, len))
    {
        fprintf(stderr, "Circular Queue assertion failed.\n");
        exit(1);
    }
}

bool _arraylist_equals(ARRAY_LIST(i) *l, const int32_t *ei, size_t len)
{
    if (l->count != len)
    {
        fprintf(stderr, "Lists not equal. Length mismatch: %zu vs %zu\n",
                l->count, len);
        return false;
    }

    for (size_t i = 0; i < len; i++)
    {
        int32_t *val = l->_al->_entries[i]->val;
        if (*val != ei[i])
        {
            fprintf(stderr, "Array List for assertion failed. Expected: %i, got %i.\n",
                    ei[i], *val);
            return false;
        }
    }

    size_t idx = 0;
    em_array_list_iter_t *it = l->iterator(l);
    while (it->has_next)
    {
        int32_t *val = it->get(it)->val;
        if (*val != ei[idx++])
        {
            fprintf(stderr, "Array List iterator assertion failed.\n");
            fprintf(stderr, "Expected: \n");
            for (size_t i = 0; i < len; i++)
                fprintf(stderr, "%i\n", ei[i]);
            fprintf(stderr, "Actual: \n");
            em_array_list_iter_t *it2 = l->iterator(l);
            while (it2->has_next)
            {
                em_array_list_node_t *e2 = it2->get(it2);
                fprintf(stderr, "%zu: %i\n", it2->_idx, *(int32_t *) e2->val);
                it2->next(it2);
            }
            free(it2);
            
            fprintf(stderr, "Dumping List.\n");
            for (size_t i = 0; i < l->size; i++)
            {
                em_array_list_node_t *n = l->_al->_entries[i];
                if (n) fprintf(stderr, "%zu: %i\n", i, *(int32_t *) n->val);
                else fprintf(stderr, "%zu: NULL\n", i);
            }

            return false;
        }
        it->next(it);
    }

    free(it);

    return true;
}

void _assert_arraylist(ARRAY_LIST(i) *l, const int32_t *ei, size_t len)
{
    if (!_arraylist_equals(l, ei, len))
    {
        fprintf(stderr, "Array List assertion failed.\n");
        exit(1);
    }
}

void _test_dll_i0(void)
{
    DOUBLE_LIST(i) *list = DOUBLE_LIST_NEW(i)(&(em_double_list_desc_t) {
        .cln_func = (void_cln_func) DOUBLE_LIST_CLN(i),
        .flags = EM_FLAG_NONE
    });
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
    DOUBLE_LIST(i) *list = DOUBLE_LIST_NEW(i)(&(em_double_list_desc_t) {
        .cln_func = (void_cln_func) DOUBLE_LIST_CLN(i),
        .flags = EM_FLAG_NONE
    });
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
    DOUBLE_LIST(i) *list = DOUBLE_LIST_NEW(i)(&(em_double_list_desc_t) {
        .cln_func = (void_cln_func) DOUBLE_LIST_CLN(i),
        .flags = EM_FLAG_NONE
    });
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
    DOUBLE_LIST(i) *list = DOUBLE_LIST_NEW(i)(&(em_double_list_desc_t) {
        .cln_func = (void_cln_func) DOUBLE_LIST_CLN(i),
        .flags = EM_FLAG_NONE
    });
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

void _test_dll_combo(void)
{
    DOUBLE_LIST(i) *list = DOUBLE_LIST_NEW(i)(&(em_double_list_desc_t) {
        .cln_func = (void_cln_func) DOUBLE_LIST_CLN(i),
        .flags = EM_FLAG_NONE
    });
    printf("0/8 ");

    printf("\r1/8 ");
    list->insert_at(list, 0, 1);
    _assert_list(list, (int32_t[]) {1}, 1);
    printf("\r2/8 ");
    list->append(list, 2);
    _assert_list(list, (int32_t[]) {1, 2}, 2);
    printf("\r3/8 ");
    list->pop_first(list);
    _assert_list(list, (int32_t[]) {2}, 1);
    printf("\r4/8 ");
    list->append(list, 4);
    _assert_list(list, (int32_t[]) {2, 4}, 2);
    printf("\r5/8 ");
    list->remove_node(list, list->get_node(list, 0));
    _assert_list(list, (int32_t[]) {4}, 1);
    printf("\r6/8 ");
    list->insert_at(list, 1, 2);
    _assert_list(list, (int32_t[]) {4, 2}, 2);
    printf("\r6/8 ");
    list->append(list, 3);
    _assert_list(list, (int32_t[]) {4, 2, 3}, 3);
    printf("\r7/8 ");
    list->pop_last(list);
    _assert_list(list, (int32_t[]) {4, 2}, 2);
    printf("\r8/8 ");

    list->destroy(list);
}

void _test_hmap_i0(void)
{
    HASHMAP(c2i) *map = HASHMAP_NEW(c2i)(&(em_hashmap_desc_t) {
        .capacity = 20,
        .cmp_func = (void_cmp_func) HASHMAP_CMP(c),
        .hsh_func = (void_hsh_func) HASHMAP_HSH(c),
        .cln_k_func = (void_cln_func) HASHMAP_CLN_K(c),
        .cln_v_func = (void_cln_func) HASHMAP_CLN_V(i),
        .flags = EM_FLAG_NONE
    });
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
    HASHMAP(c2i) *map = HASHMAP_NEW(c2i)(&(em_hashmap_desc_t) {
        .capacity = 1,
        .cmp_func = (void_cmp_func) HASHMAP_CMP(c),
        .hsh_func = (void_hsh_func) HASHMAP_HSH(c),
        .cln_k_func = (void_cln_func) HASHMAP_CLN_K(c),
        .cln_v_func = (void_cln_func) HASHMAP_CLN_V(i),
        .flags = EM_FLAG_NONE
    });
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
    HASHMAP(c2i) *map = HASHMAP_NEW(c2i)(&(em_hashmap_desc_t) {
        .capacity = 1,
        .cmp_func = (void_cmp_func) HASHMAP_CMP(c),
        .hsh_func = (void_hsh_func) HASHMAP_HSH(c),
        .cln_k_func = (void_cln_func) HASHMAP_CLN_K(c),
        .cln_v_func = (void_cln_func) HASHMAP_CLN_V(i),
        .flags = EM_FLAG_NONE
    });
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
    HASHMAP(c2i) *map = HASHMAP_NEW(c2i)(&(em_hashmap_desc_t) {
        .capacity = 20,
        .cmp_func = (void_cmp_func) HASHMAP_CMP(c),
        .hsh_func = (void_hsh_func) HASHMAP_HSH(c),
        .cln_k_func = (void_cln_func) HASHMAP_CLN_K(c),
        .cln_v_func = (void_cln_func) HASHMAP_CLN_V(i),
        .flags = EM_FLAG_NONE
    });
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

void _test_hmap_f0(void)
{
    HASHMAP(c2i) *map = HASHMAP_NEW(c2i)(&(em_hashmap_desc_t) {
        .capacity = 1,
        .cmp_func = (void_cmp_func) HASHMAP_CMP(c),
        .hsh_func = (void_hsh_func) HASHMAP_HSH(c),
        .cln_k_func = (void_cln_func) HASHMAP_CLN_K(c),
        .cln_v_func = (void_cln_func) HASHMAP_CLN_V(i),
        .flags = EM_FLAG_NONE
    });
    printf("0/2 ");

    printf("\r1/2 ");
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
    printf("\r2/2 ");
    test_predicate_args_t args = {
        .max = 4
    };
    map->filter_out(map, test_predicate, &args);
    _assert_hmap(map, (char[]) {'a', 'b', 'c', 'd'}, 
                 (int32_t[]) {1, 2, 3, 4}, 4);

    map->destroy(map);
}

void _test_hmap_f1(void)
{
    HASHMAP(c2i) *map = HASHMAP_NEW(c2i)(&(em_hashmap_desc_t) {
        .capacity = 1,
        .cmp_func = (void_cmp_func) HASHMAP_CMP(c),
        .hsh_func = (void_hsh_func) HASHMAP_HSH(c),
        .cln_k_func = (void_cln_func) HASHMAP_CLN_K(c),
        .cln_v_func = (void_cln_func) HASHMAP_CLN_V(i),
        .flags = EM_FLAG_NONE
    });
    printf("0/2 ");

    printf("\r1/2 ");
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
    printf("\r2/2 ");
    test_predicate_args_t args = {
        .max = 4
    };
    em_hashmap_entry_t **res = map->filter_get(map, test_predicate, &args);
    _assert_hmap(map, (char[]) {'a', 'b', 'c', 'd'}, 
                 (int32_t[]) {1, 2, 3, 4}, 4);
    
    bool found[4] = {0};
    char ec[4] = {'e', 'f', 'g', 'h'};
    int32_t ei[4] = {5, 6, 7, 8};

    for (size_t i = 0; i < 4; i++)
    {
        em_hashmap_entry_t *e = res[i];
        char *key = e->key;
        int32_t *val = e->val;

        if (e)
        {
            size_t j;
            for (j = 0; j < 4; j++)
            {
                if (ec[j] == *key && ei[j] == *val && !found[j])
                {
                    found[j] = true;
                    break;
                }
            }

            if (!found[j])
            {
                fprintf(stderr, "Get result does not match expected.\n");
                exit(1);
            }
        }
    }

    map->destroy(map);
}

void _test_hmap_combo(void)
{
    HASHMAP(c2i) *map = HASHMAP_NEW(c2i)(&(em_hashmap_desc_t) {
        .capacity = 1,
        .cmp_func = (void_cmp_func) HASHMAP_CMP(c),
        .hsh_func = (void_hsh_func) HASHMAP_HSH(c),
        .cln_k_func = (void_cln_func) HASHMAP_CLN_K(c),
        .cln_v_func = (void_cln_func) HASHMAP_CLN_V(i),
        .flags = EM_FLAG_NONE
    });
    printf("0/16 ");

    printf("\r1/16 ");
    map->put(map, 'a', 1);
    _assert_hmap(map, (char[]) {'a'}, (int32_t[]) {1}, 1);
    printf("\r2/16 ");
    map->put(map, 'b', 2);
    _assert_hmap(map, (char[]) {'a', 'b'}, (int32_t[]) {1, 2}, 2);
    printf("\r3/16 ");
    map->put(map, 'c', 3);
    _assert_hmap(map, (char[]) {'a', 'b', 'c'}, (int32_t[]) {1, 2, 3}, 3);
    printf("\r4/16 ");
    map->put(map, 'd', 4);
    _assert_hmap(map, (char[]) {'a', 'b', 'c', 'd'}, (int32_t[]) {1, 2, 3, 4}, 4);
    printf("\r5/16 ");
    map->pop(map, 'c');
    _assert_hmap(map, (char[]) {'a', 'b', 'd'}, (int32_t[]) {1, 2, 4}, 3);
    printf("\r6/16 ");
    map->put(map, 'c', 10);
    _assert_hmap(map, (char[]) {'a', 'b', 'c', 'd'}, (int32_t[]) {1, 2, 10, 4}, 4);
    printf("\r7/16 ");
    test_predicate_args_t args_over_3 = {.max = 3};
    map->filter_get(map, test_predicate, &args_over_3);
    _assert_hmap(map, (char[]) {'a', 'b'}, (int32_t[]) {1, 2}, 2);
    printf("\r8/16 ");
    map->put(map, 'c', 3);
    _assert_hmap(map, (char[]) {'a', 'b', 'c'}, (int32_t[]) {1, 2, 3}, 3);
    printf("\r8/16 ");
    map->put(map, 'd', 4);
    _assert_hmap(map, (char[]) {'a', 'b', 'c', 'd'}, (int32_t[]) {1, 2, 3, 4}, 4);
    printf("\r9/16 ");
    map->put(map, 'e', 12);
    _assert_hmap(map, (char[]) {'a', 'b', 'c', 'd', 'e'}, (int32_t[]) {1, 2, 3, 4, 12}, 5);
    printf("\r10/16 ");
    map->put(map, 'f', 12);
    _assert_hmap(map, (char[]) {'a', 'b', 'c', 'd', 'e', 'f'}, (int32_t[]) {1, 2, 3, 4, 12, 12}, 6);
    printf("\r11/16 ");
    test_predicate_args_t args_over_5 = {.max = 5};
    map->filter_get(map, test_predicate, &args_over_5);
    _assert_hmap(map, (char[]) {'a', 'b', 'c', 'd'}, (int32_t[]) {1, 2, 3, 4}, 4);
    printf("\r12/16 ");
    map->remove(map, 'a');
    _assert_hmap(map, (char[]) {'b', 'c', 'd'}, (int32_t[]) {2, 3, 4}, 3);
    printf("\r13/16 ");
    map->put(map, 'a', 10);
    _assert_hmap(map, (char[]) {'b', 'c', 'd', 'a'}, (int32_t[]) {2, 3, 4, 10}, 4);
    printf("\r14/16 ");
    map->remove(map, 'a');
    _assert_hmap(map, (char[]) {'b', 'c', 'd'}, (int32_t[]) {2, 3, 4}, 3);
    printf("\r15/16 ");
    map->filter_out(map, test_predicate, &args_over_3);
    _assert_hmap(map, (char[]) {'b', 'c'}, (int32_t[]) {2, 3}, 2);
    printf("\r16/16 ");
}

void _test_cq_i0(void)
{
    CIRCULAR_QUEUE(i) *cq = CIRCULAR_QUEUE_NEW(i)(&(em_circular_queue_desc_t) {
        .capacity = 20,
        .cln_func = (void_cln_func) CIRCULAR_QUEUE_CLN(i),
        .flags = EM_FLAG_NONE
    });
    printf("0/8 ");
    cq->enqueue(cq, 1);
    _assert_cq(cq, (int32_t[]) {1}, 1);
    printf("\r1/8 ");
    cq->enqueue(cq, 2);
    _assert_cq(cq, (int32_t[]) {1, 2}, 2);
    printf("\r2/8 ");
    cq->enqueue(cq, 3);
    _assert_cq(cq, (int32_t[]) {1, 2, 3}, 3);
    printf("\r3/8 ");
    cq->enqueue(cq, 4);
    _assert_cq(cq, (int32_t[]) {1, 2, 3, 4}, 4);
    printf("\r4/8 ");
    cq->enqueue(cq, 5);
    _assert_cq(cq, (int32_t[]) {1, 2, 3, 4, 5}, 5);
    printf("\r5/8 ");
    cq->enqueue(cq, 6);
    _assert_cq(cq, (int32_t[]) {1, 2, 3, 4, 5, 6}, 6);
    printf("\r6/8 ");
    cq->enqueue(cq, 7);
    _assert_cq(cq, (int32_t[]) {1, 2, 3, 4, 5, 6, 7}, 7);
    printf("\r7/8 ");
    cq->enqueue(cq, 8);
    _assert_cq(cq, (int32_t[]) {1, 2, 3, 4, 5, 6, 7, 8}, 8);
    printf("\r8/8 ");

    cq->destroy(cq);
}

void _test_cq_i1(void)
{
    CIRCULAR_QUEUE(i) *cq = CIRCULAR_QUEUE_NEW(i)(&(em_circular_queue_desc_t) {
        .capacity = 1,
        .cln_func = (void_cln_func) CIRCULAR_QUEUE_CLN(i),
        .flags = EM_FLAG_NONE
    });
    printf("0/8 ");
    cq->enqueue(cq, 1);
    _assert_cq(cq, (int32_t[]) {1}, 1);
    printf("\r1/8 ");
    cq->enqueue(cq, 2);
    _assert_cq(cq, (int32_t[]) {1, 2}, 2);
    printf("\r2/8 ");
    cq->enqueue(cq, 3);
    _assert_cq(cq, (int32_t[]) {1, 2, 3}, 3);
    printf("\r3/8 ");
    cq->enqueue(cq, 4);
    _assert_cq(cq, (int32_t[]) {1, 2, 3, 4}, 4);
    printf("\r4/8 ");
    cq->enqueue(cq, 5);
    _assert_cq(cq, (int32_t[]) {1, 2, 3, 4, 5}, 5);
    printf("\r5/8 ");
    cq->enqueue(cq, 6);
    _assert_cq(cq, (int32_t[]) {1, 2, 3, 4, 5, 6}, 6);
    printf("\r6/8 ");
    cq->enqueue(cq, 7);
    _assert_cq(cq, (int32_t[]) {1, 2, 3, 4, 5, 6, 7}, 7);
    printf("\r7/8 ");
    cq->enqueue(cq, 8);
    _assert_cq(cq, (int32_t[]) {1, 2, 3, 4, 5, 6, 7, 8}, 8);
    printf("\r8/8 ");

    cq->destroy(cq);
}

void _test_cq_d0(void)
{
    CIRCULAR_QUEUE(i) *cq = CIRCULAR_QUEUE_NEW(i)(&(em_circular_queue_desc_t) {
        .capacity = 20,
        .cln_func = (void_cln_func) CIRCULAR_QUEUE_CLN(i),
        .flags = EM_FLAG_NONE
    });
    printf("0/10 ");

    printf("\r1/10 ");
    cq->enqueue(cq, 1);
    cq->enqueue(cq, 2);
    cq->enqueue(cq, 3);
    cq->enqueue(cq, 4);
    cq->enqueue(cq, 5);
    cq->enqueue(cq, 6);
    cq->enqueue(cq, 7);
    cq->enqueue(cq, 8);
    _assert_cq(cq, (int32_t[]) {1, 2, 3, 4, 5, 6, 7, 8}, 8);
    printf("\r2/10 ");
    cq->dequeue(cq);
    _assert_cq(cq, (int32_t[]) {2, 3, 4, 5, 6, 7, 8}, 7);
    printf("\r3/10 ");
    cq->dequeue(cq);
    _assert_cq(cq, (int32_t[]) {3, 4, 5, 6, 7, 8}, 6);
    printf("\r4/10 ");
    cq->dequeue(cq);
    _assert_cq(cq, (int32_t[]) {4, 5, 6, 7, 8}, 5);
    printf("\r5/10 ");
    cq->dequeue(cq);
    _assert_cq(cq, (int32_t[]) {5, 6, 7, 8}, 4);
    printf("\r6/10 ");
    cq->dequeue(cq);
    _assert_cq(cq, (int32_t[]) {6, 7, 8}, 3);
    printf("\r7/10 ");
    cq->dequeue(cq);
    _assert_cq(cq, (int32_t[]) {7, 8}, 2);
    printf("\r8/10 ");
    cq->dequeue(cq);
    _assert_cq(cq, (int32_t[]) {8}, 1);
    printf("\r9/10 ");
    cq->dequeue(cq);
    _assert_cq(cq, (int32_t[]) {}, 0);
    printf("\r10/10 ");

    cq->destroy(cq);
}

void _test_cq_d1(void)
{
    CIRCULAR_QUEUE(i) *cq = CIRCULAR_QUEUE_NEW(i)(&(em_circular_queue_desc_t) {
        .capacity = 1,
        .cln_func = (void_cln_func) CIRCULAR_QUEUE_CLN(i),
        .flags = EM_FLAG_NONE
    });
    printf("0/10 ");

    printf("\r1/10 ");
    cq->enqueue(cq, 1);
    cq->enqueue(cq, 2);
    cq->enqueue(cq, 3);
    cq->enqueue(cq, 4);
    cq->enqueue(cq, 5);
    cq->enqueue(cq, 6);
    cq->enqueue(cq, 7);
    cq->enqueue(cq, 8);
    _assert_cq(cq, (int32_t[]) {1, 2, 3, 4, 5, 6, 7, 8}, 8);
    printf("\r2/10 ");
    cq->dequeue(cq);
    _assert_cq(cq, (int32_t[]) {2, 3, 4, 5, 6, 7, 8}, 7);
    printf("\r3/10 ");
    cq->dequeue(cq);
    _assert_cq(cq, (int32_t[]) {3, 4, 5, 6, 7, 8}, 6);
    printf("\r4/10 ");
    cq->dequeue(cq);
    _assert_cq(cq, (int32_t[]) {4, 5, 6, 7, 8}, 5);
    printf("\r5/10 ");
    cq->dequeue(cq);
    _assert_cq(cq, (int32_t[]) {5, 6, 7, 8}, 4);
    printf("\r6/10 ");
    cq->dequeue(cq);
    _assert_cq(cq, (int32_t[]) {6, 7, 8}, 3);
    printf("\r7/10 ");
    cq->dequeue(cq);
    _assert_cq(cq, (int32_t[]) {7, 8}, 2);
    printf("\r8/10 ");
    cq->dequeue(cq);
    _assert_cq(cq, (int32_t[]) {8}, 1);
    printf("\r9/10 ");
    cq->dequeue(cq);
    _assert_cq(cq, (int32_t[]) {}, 0);
    printf("\r10/10 ");

    cq->destroy(cq);
}

void _test_al_i0(void)
{
    ARRAY_LIST(i) *al = ARRAY_LIST_NEW(i)(&(em_array_list_desc_t) {
        .capacity = 20,
        .cln_func = (void_cln_func) ARRAY_LIST_CLN(i),
        .flags = EM_FLAG_NONE
    });
    printf("0/6 ");

    printf("\r1/6 ");
    al->append(al, 1);
    _assert_arraylist(al, (int32_t[]) {1}, 1);
    printf("\r2/6 ");
    al->append(al, 2);
    _assert_arraylist(al, (int32_t[]) {1, 2}, 2);
    printf("\r3/6 ");
    al->append(al, 3);
    _assert_arraylist(al, (int32_t[]) {1, 2, 3}, 3);
    printf("\r4/6 ");
    al->append(al, 4);
    _assert_arraylist(al, (int32_t[]) {1, 2, 3, 4}, 4);
    printf("\r5/6 ");
    al->append(al, 5);
    _assert_arraylist(al, (int32_t[]) {1, 2, 3, 4, 5}, 5);
    printf("\r6/6 ");

    al->destroy(al);
}

void _test_al_i1(void)
{
    ARRAY_LIST(i) *al = ARRAY_LIST_NEW(i)(&(em_array_list_desc_t) {
        .capacity = 1,
        .cln_func = (void_cln_func) ARRAY_LIST_CLN(i),
        .flags = EM_FLAG_NONE
    });
    printf("0/6 ");

    printf("\r1/6 ");
    al->append(al, 1);
    _assert_arraylist(al, (int32_t[]) {1}, 1);
    printf("\r2/6 ");
    al->append(al, 2);
    _assert_arraylist(al, (int32_t[]) {1, 2}, 2);
    printf("\r3/6 ");
    al->append(al, 3);
    _assert_arraylist(al, (int32_t[]) {1, 2, 3}, 3);
    printf("\r4/6 ");
    al->append(al, 4);
    _assert_arraylist(al, (int32_t[]) {1, 2, 3, 4}, 4);
    printf("\r5/6 ");
    al->append(al, 5);
    _assert_arraylist(al, (int32_t[]) {1, 2, 3, 4, 5}, 5);
    printf("\r6/6 ");

    al->destroy(al);
}

void _test_al_d0(void)
{
    ARRAY_LIST(i) *al = ARRAY_LIST_NEW(i)(&(em_array_list_desc_t) {
        .capacity = 20,
        .cln_func = (void_cln_func) ARRAY_LIST_CLN(i),
        .flags = EM_FLAG_NONE
    });
    printf("0/11 ");

    printf("\r1/11 ");
    al->append(al, 1);
    al->append(al, 2);
    al->append(al, 3);
    al->append(al, 4);
    al->append(al, 5);
    al->append(al, 6);
    al->append(al, 7);
    al->append(al, 8);
    al->append(al, 9);
    _assert_arraylist(al, (int32_t[]) {1, 2, 3, 4, 5, 6, 7, 8, 9}, 9);
    printf("\r2/11 ");
    al->remove(al, 0);
    _assert_arraylist(al, (int32_t[]) {2, 3, 4, 5, 6, 7, 8, 9}, 8);
    printf("\r3/11 ");
    al->remove(al, 7);
    _assert_arraylist(al, (int32_t[]) {2, 3, 4, 5, 6, 7, 8}, 7);
    printf("\r4/11 ");
    al->remove(al, 3);
    _assert_arraylist(al, (int32_t[]) {2, 3, 4, 6, 7, 8}, 6);
    printf("\r5/11 ");
    al->remove(al, 3);
    _assert_arraylist(al, (int32_t[]) {2, 3, 4, 7, 8}, 5);
    printf("\r6/11 ");
    al->remove(al, 1);
    _assert_arraylist(al, (int32_t[]) {2, 4, 7, 8}, 4);
    printf("\r7/11 ");
    al->remove(al, 2);
    _assert_arraylist(al, (int32_t[]) {2, 4, 8}, 3);
    printf("\r8/11 ");
    al->remove(al, 1);
    _assert_arraylist(al, (int32_t[]) {2, 8}, 2);
    printf("\r9/11 ");
    al->remove(al, 1);
    _assert_arraylist(al, (int32_t[]) {2}, 1);
    printf("\r10/11 ");
    al->remove(al, 0);
    _assert_arraylist(al, (int32_t[]) {}, 0);
    printf("\r11/11 ");

    al->destroy(al);
}

void _test_al_d1(void)
{
    ARRAY_LIST(i) *al = ARRAY_LIST_NEW(i)(&(em_array_list_desc_t) {
        .capacity = 1,
        .cln_func = (void_cln_func) ARRAY_LIST_CLN(i),
        .flags = EM_FLAG_NONE
    });
    printf("0/11 ");

    printf("\r1/11 ");
    al->append(al, 1);
    al->append(al, 2);
    al->append(al, 3);
    al->append(al, 4);
    al->append(al, 5);
    al->append(al, 6);
    al->append(al, 7);
    al->append(al, 8);
    al->append(al, 9);
    _assert_arraylist(al, (int32_t[]) {1, 2, 3, 4, 5, 6, 7, 8, 9}, 9);
    printf("\r2/11 ");
    al->remove(al, 5);
    _assert_arraylist(al, (int32_t[]) {1, 2, 3, 4, 5, 7, 8, 9}, 8);
    printf("\r3/11 ");
    al->remove(al, 7);
    _assert_arraylist(al, (int32_t[]) {1, 2, 3, 4, 5, 7, 8}, 7);
    printf("\r4/11 ");
    al->remove(al, 6);
    _assert_arraylist(al, (int32_t[]) {1, 2, 3, 4, 5, 7}, 6);
    printf("\r5/11 ");
    al->remove(al, 0);
    _assert_arraylist(al, (int32_t[]) {2, 3, 4, 5, 7}, 5);
    printf("\r6/11 ");
    al->remove(al, 0);
    _assert_arraylist(al, (int32_t[]) {3, 4, 5, 7}, 4);
    printf("\r7/11 ");
    al->remove(al, 2);
    _assert_arraylist(al, (int32_t[]) {3, 4, 7}, 3);
    printf("\r8/11 ");
    al->remove(al, 1);
    _assert_arraylist(al, (int32_t[]) {3, 7}, 2);
    printf("\r9/11 ");
    al->remove(al, 1);
    _assert_arraylist(al, (int32_t[]) {3}, 1);
    printf("\r10/11 ");
    al->remove(al, 0);
    _assert_arraylist(al, (int32_t[]) {}, 0);
    printf("\r11/11 ");

    al->destroy(al);
}

void _test_dll(void)
{
    printf("Testing DLL Insertions.\n");

    _test_dll_i0();
    printf("Passed.\n");

    _test_dll_i1();
    printf("Passed.\n");

    printf("\nTesting DLL Deletions.\n");

    _test_dll_d0();
    printf("Passed.\n");

    _test_dll_d1();
    printf("Passed.\n");

    printf("\nTesting Multiple Operations on one DLL.\n");

    _test_dll_combo();
    printf("Passed.\n");
}

void _test_hmap(void)
{
    printf("Testing HashMap Insertions.\n");

    _test_hmap_i0();
    printf("Passed.\n");

    _test_hmap_i1();
    printf("Passed.\n");

    printf("\nTesting HashMap Deletions.\n");

    _test_hmap_d0();
    printf("Passed.\n");

    _test_hmap_d1();
    printf("Passed.\n");

    printf("\nTesting HashMap Filtering.\n");

    _test_hmap_f0();
    printf("Passed.\n");

    _test_hmap_f1();
    printf("Passed.\n");

    printf("\nTesting Multiple Operations On One HashMap.\n");

    _test_hmap_combo();
    printf("Passed.\n");
}

void _test_cq(void)
{
    printf("Testing Circular Queue Insertions.\n");

    _test_cq_i0();
    printf("Passed.\n");

    _test_cq_i1();
    printf("Passed.\n");

    printf("\nTesting Circular Queue Deletions.\n");

    _test_cq_d0();
    printf("Passed.\n");

    _test_cq_d1();
    printf("Passed.\n");
}

void _test_al(void)
{
    printf("Testing Array List Insertions.\n");
    
    _test_al_i0();
    printf("Passed.\n");

    _test_al_i1();
    printf("Passed.\n");

    printf("\nTesting Array List Deletions.\n");

    _test_al_d0();
    printf("Passed.\n");

    _test_al_d1();
    printf("Passed.\n");
}

void test_main()
{
    printf("===== TESTING DOUBLY LINKED LIST =====\n\n");
    _test_dll();
    printf("\n===== TESTING HASHMAP =====\n\n");
    _test_hmap();
    printf("\n===== TESTING CIRCULAR QUEUE =====\n\n");
    _test_cq();
    printf("\n===== TESTING ARRAY LIST =====\n\n");
    _test_al();

    printf("\n===== ALL TESTS PASSED =====\n");
}
