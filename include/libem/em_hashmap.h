#ifndef EM_HASHMAP_H
#define EM_HASHMAP_H

#ifndef EM_HASHMAP_INCLUDED
#define EM_HASHMAP_INCLUDED

#include "em_global.h"

#include <malloc.h>
#include <memory.h>
#include <stddef.h> // size_t
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/* Utilities. */
#define GET_OR_NULL(MAP, KEY) MAP->get_or_default(MAP, KEY, NULL)

/* Declaration template. */
#define DECLARE_HASHMAP_CMP(KTYP, NAME) extern bool em_hashmap_cmp_func_##NAME(const KTYP *lhs, const KTYP *rhs);
#define DECLARE_HASHMAP_HSH(KTYP, NAME) extern uint32_t em_hashmap_hsh_func_##NAME(const KTYP *key);
#define DECLARE_HASHMAP_CLN_K(KTYP, NAME) extern void em_hashmap_cln_k_func_##NAME(KTYP *key);
#define DECLARE_HASHMAP_CLN_V(VTYP, NAME) extern void em_hashmap_cln_v_func_##NAME(VTYP *val);
#define DECLARE_HASHMAP(KTYP, VTYP, NAME)\
\
typedef struct em_hashmap_##NAME {\
    /* Members. */\
    size_t size;\
    size_t count;\
    em_hashmap_t *_hashmap;\
    /* Methods. */\
    /* Get a new iterator for this hashmap. */\
    em_hashmap_iter_t *(*iterator)(struct em_hashmap_##NAME *this);\
    /* Get a new iterator for this hashmap starting from a given point. */\
    em_hashmap_iter_t *(*iterator_from)(struct em_hashmap_##NAME *this, const em_hashmap_iter_info_t *info);\
    /* Place a key/value pair into the hashmap. */\
    void (*put)(struct em_hashmap_##NAME *this, KTYP key, VTYP value);\
    /* Place a key/value pair into the hashmap. NOTE: value must be a heap pointer. */\
    void (*put_ptr)(struct em_hashmap_##NAME *this, KTYP key, VTYP *value_ptr);\
    /* Get the value stored at key from the hashmap. If there is none, runtime error. */\
    VTYP (*get)(struct em_hashmap_##NAME *this, KTYP key);\
    /* Get the pointer stored at key from the hashmap. If there is none, runtime error. */\
    VTYP *(*get_ptr)(struct em_hashmap_##NAME *this, KTYP key);\
    /* Get the pointer stored at key from the hashmap. If there is none, returns default. */\
    VTYP *(*get_or_default)(struct em_hashmap_##NAME *this, KTYP key, VTYP *dflt_ptr);\
    /* Checks if the hashmap contains the key. */\
    bool (*contains_key)(struct em_hashmap_##NAME *this, KTYP key);\
    /* Removes and returns the valuea t key. */\
    VTYP (*pop)(struct em_hashmap_##NAME *this, KTYP key);\
    /* Removes and returns the pointer at key. */\
    VTYP *(*pop_ptr)(struct em_hashmap_##NAME *this, KTYP key);\
    /* Removes the value at key. */\
    void (*remove)(struct em_hashmap_##NAME *this, KTYP key);\
    /* Frees all resources associated with this hashmap. */\
    void (*destroy)(struct em_hashmap_##NAME *this);\
    /* Removes all entries whose value matches the predicate. */\
    void (*filter_out)(struct em_hashmap_##NAME *this, bool (*pred)(const VTYP *, const void *), const void *args);\
    /* Removes and returns all entries that match the predicate from the map. */\
    em_hashmap_entry_t **(*filter_get)(struct em_hashmap_##NAME *this, bool (*pred)(const VTYP *, const void *), const void *args);\
} em_hashmap_##NAME##_t;\
\
em_hashmap_##NAME##_t *em_hashmap_##NAME##_new(const em_hashmap_desc_t *desc);\

/* Definition template. */
#define HASHMAP(NAME) em_hashmap_##NAME##_t
#define HASHMAP_NEW(NAME) em_hashmap_##NAME##_new
#define HASHMAP_CMP(NAME) em_hashmap_cmp_func_##NAME
#define HASHMAP_HSH(NAME) em_hashmap_hsh_func_##NAME
#define HASHMAP_CLN_K(NAME) em_hashmap_cln_k_func_##NAME
#define HASHMAP_CLN_V(NAME) em_hashmap_cln_v_func_##NAME
#define DEFINE_HASHMAP_CMP(KTYP, NAME) bool HASHMAP_CMP(NAME)(const KTYP *lhs, const KTYP *rhs)
#define DEFINE_HASHMAP_HSH(KTYP, NAME) uint32_t HASHMAP_HSH(NAME)(const KTYP *key)
#define DEFINE_HASHMAP_CLN_K(KTYP, NAME) void HASHMAP_CLN_K(NAME)(KTYP *key)
#define DEFINE_HASHMAP_CLN_V(VTYP, NAME) void HASHMAP_CLN_V(NAME)(VTYP *val)
#define DEFINE_HASHMAP(KTYP, VTYP, NAME)\
\
static void _update_hashmap_##NAME(em_hashmap_##NAME##_t *this)\
{\
    this->size = this->_hashmap->size;\
    this->count = this->_hashmap->count;\
}\
\
static em_hashmap_iter_t *_iterator_hashmap_##NAME(em_hashmap_##NAME##_t *this)\
{\
    return em_hashmap_iterator(this->_hashmap);\
}\
\
static em_hashmap_iter_t *_iterator_from_hashmap_##NAME(em_hashmap_##NAME##_t *this,\
                                                        const em_hashmap_iter_info_t *info)\
{\
    return em_hashmap_iterator_from(this->_hashmap, info);\
}\
\
static void _put_hashmap_##NAME(em_hashmap_##NAME##_t *this, KTYP k, VTYP v)\
{\
    KTYP *k_ptr = malloc(sizeof(KTYP));\
    *k_ptr = k;\
    VTYP *v_ptr = malloc(sizeof(VTYP));\
    *v_ptr = v;\
    em_hashmap_put(this->_hashmap, k_ptr, v_ptr);\
    _update_hashmap_##NAME(this);\
}\
\
static void _put_ptr_hashmap_##NAME(em_hashmap_##NAME##_t *this, KTYP k, VTYP *v)\
{\
    KTYP *k_ptr = malloc(sizeof(KTYP));\
    *k_ptr = k;\
    em_hashmap_put(this->_hashmap, k_ptr, v);\
    _update_hashmap_##NAME(this);\
}\
\
static VTYP _get_hashmap_##NAME(em_hashmap_##NAME##_t *this, KTYP k)\
{\
    VTYP *res = em_hashmap_get(this->_hashmap, &k);\
    return *res;\
}\
\
static VTYP *_get_ptr_hashmap_##NAME(em_hashmap_##NAME##_t *this, KTYP k)\
{\
    VTYP *res = (VTYP *) em_hashmap_get(this->_hashmap, &k);\
    return res;\
}\
\
static VTYP *_get_or_default_hashmap_##NAME(em_hashmap_##NAME##_t *this, KTYP k, VTYP *dflt)\
{\
    VTYP *res = em_hashmap_get_or_default(this->_hashmap, &k, dflt);\
    return res;\
}\
\
static bool _contains_key_hashmap_##NAME(em_hashmap_##NAME##_t *this, KTYP k)\
{\
    bool res = em_hashmap_contains_key(this->_hashmap, &k);\
    return res;\
}\
\
static VTYP _pop_hashmap_##NAME(em_hashmap_##NAME##_t *this, KTYP k)\
{\
    VTYP *res = em_hashmap_pop(this->_hashmap, &k);\
    _update_hashmap_##NAME(this);\
    return *res;\
}\
\
static VTYP *_pop_ptr_hashmap_##NAME(em_hashmap_##NAME##_t *this, KTYP k)\
{\
    VTYP *res = em_hashmap_pop(this->_hashmap, &k);\
    _update_hashmap_##NAME(this);\
    return res;\
}\
\
static void _remove_hashmap_##NAME(em_hashmap_##NAME##_t *this, KTYP k)\
{\
    em_hashmap_remove(this->_hashmap, &k);\
    _update_hashmap_##NAME(this);\
}\
\
static void _destroy_hashmap_##NAME(em_hashmap_##NAME##_t *this)\
{\
    em_hashmap_destroy(this->_hashmap);\
    free(this);\
}\
\
static void _filter_out_hashmap_##NAME(em_hashmap_##NAME##_t *this,\
                                       bool (*predicate)(const VTYP *, const void *),\
                                       const void *args)\
{\
    em_hashmap_filter_out(this->_hashmap, (void_fltr_func) predicate, args);\
    _update_hashmap_##NAME(this);\
}\
\
static em_hashmap_entry_t **_filter_get_hashmap_##NAME(em_hashmap_##NAME##_t *this,\
                                                       bool (*predicate)(const VTYP *, const void *),\
                                                       const void *args)\
{\
    em_hashmap_entry_t **res = em_hashmap_filter_get(this->_hashmap, (void_fltr_func) predicate, args);\
    _update_hashmap_##NAME(this);\
    return res;\
}\
\
em_hashmap_##NAME##_t *em_hashmap_##NAME##_new(const em_hashmap_desc_t *desc)\
{\
    if (desc->capacity == 0 || !desc->cmp_func || !desc->hsh_func\
        || !desc->cln_k_func || !desc->cln_v_func)\
    {\
        EM_LOG("Hashmap initialization params are invalid.\n", NULL);\
        exit(1);\
    }\
    \
    em_hashmap_##NAME##_t *res = malloc(sizeof(em_hashmap_##NAME##_t));\
    if (!res)\
    {\
        EM_LOG("Failed to allocate memory for hashmap.\n", NULL);\
        exit(1);\
    }\
    \
    res->_hashmap       = em_hashmap_new(desc);\
    \
    res->size           = res->_hashmap->size;\
    res->count          = res->_hashmap->count;\
    \
    res->iterator       = _iterator_hashmap_##NAME;\
    res->iterator_from  = _iterator_from_hashmap_##NAME;\
    res->put            = _put_hashmap_##NAME;\
    res->put_ptr        = _put_ptr_hashmap_##NAME;\
    res->get            = _get_hashmap_##NAME;\
    res->get_ptr        = _get_ptr_hashmap_##NAME;\
    res->get_or_default = _get_or_default_hashmap_##NAME;\
    res->contains_key   = _contains_key_hashmap_##NAME;\
    res->pop            = _pop_hashmap_##NAME;\
    res->pop_ptr        = _pop_ptr_hashmap_##NAME;\
    res->remove         = _remove_hashmap_##NAME;\
    res->destroy        = _destroy_hashmap_##NAME;\
    res->filter_out     = _filter_out_hashmap_##NAME;\
    res->filter_get     = _filter_get_hashmap_##NAME;\
    \
    return res;\
}

typedef uint32_t (*void_hsh_func)(const void *);

typedef struct em_hashmap_entry {
    void *key;
    void *val;

    struct em_hashmap_entry *_next;
} em_hashmap_entry_t;

typedef struct em_hashmap_desc {
    size_t capacity;
    void_cmp_func cmp_func;
    void_hsh_func hsh_func;
    void_cln_func cln_k_func;
    void_cln_func cln_v_func;
    int32_t flags;
} em_hashmap_desc_t;

typedef struct em_hashmap {
    size_t size;
    size_t count;

    int32_t _flags;
    void_cmp_func _cmp_func;
    void_hsh_func _hsh_func;
    void_cln_func _cln_k_func;
    void_cln_func _cln_v_func;
    em_hashmap_entry_t **_entries;
} em_hashmap_t;

typedef struct em_hashmap_iter_info {
    size_t index;
    size_t count;
    em_hashmap_entry_t *current;
} em_hashmap_iter_info_t;

typedef struct em_hashmap_iter {
    bool has_next;
    void (*next)(struct em_hashmap_iter *this);
    em_hashmap_entry_t *(*get)(struct em_hashmap_iter *this);
    em_hashmap_iter_info_t *(*query_info)(struct em_hashmap_iter *this);

    em_hashmap_t *_map;
    em_hashmap_entry_t *_curr;
    size_t _idx;
    size_t _count;
} em_hashmap_iter_t;

uint32_t em_hash(int32_t key);

em_hashmap_entry_t *_hashmap_iter_get(em_hashmap_iter_t *iter);
void _hashmap_iter_next(em_hashmap_iter_t *iter);
em_hashmap_iter_info_t *_hashmap_iter_query(em_hashmap_iter_t *iter);
em_hashmap_iter_t *em_hashmap_iterator(em_hashmap_t *this);
em_hashmap_iter_t *em_hashmap_iterator_from(em_hashmap_t *this, const em_hashmap_iter_info_t *info);

em_hashmap_t *em_hashmap_new(const em_hashmap_desc_t *desc);
void em_hashmap_put(em_hashmap_t *this, void *key, void *val);
void *em_hashmap_get(em_hashmap_t *this, const void *key);
void *em_hashmap_get_or_default(em_hashmap_t *this, const void *key, void *val);
bool em_hashmap_contains_key(em_hashmap_t *this, const void *key);
void *em_hashmap_pop(em_hashmap_t *this, const void *key);
void em_hashmap_remove(em_hashmap_t *this, const void *key);
void em_hashmap_destroy(em_hashmap_t *this);
void em_hashmap_filter_out(em_hashmap_t *this, void_fltr_func pred, const void *args);
em_hashmap_entry_t **em_hashmap_filter_get(em_hashmap_t *this, void_fltr_func pred, const void *args);

#endif // EM_HASHMAP_INCLUDED

/*     ______  _______  __    ________  __________   ___________  ______________  _   __
 *    /   /  |/  / __ \/ /   / ____/  |/  / ____/ | / /_  __/   |/_  __/   / __ \/ | / /
 *    / // /|_/ / /_/ / /   / __/ / /|_/ / __/ /  |/ / / / / /| | / /  / // / / /  |/ /
 *   / // /  / / ____/ /___/ /___/ /  / / /___/ /|  / / / / ___ |/ /  / // /_/ / /|  /
 * /___/_/  /_/_/   /_____/_____/_/  /_/_____/_/ |_/ /_/ /_/  |_/_/ /___/\____/_/ |_/
 *
 */

#ifdef EM_HASHMAP_IMPL
#include <stdlib.h> // exit
#include <stdio.h> // fprintf, stderr 

#define MAX_LOAD_FACTOR 0.75

uint32_t em_hash(int32_t k)
{
    k = ((k >> 16) ^ k) * 0x45d9f3bu;
    k = ((k >> 16) ^ k) * 0x45d9f3bu;
    k = (k >> 16) ^ k;
    return k;
}
void _em_hashmap_resize(em_hashmap_t *this)
{
    EM_LOG("Hashmap resizing.\n", NULL);
    size_t new_size = this->size * 2;
    em_hashmap_entry_t **entries = calloc(new_size, sizeof(em_hashmap_entry_t *));
    if (!entries)
    {
        EM_LOG("Failed to reallocate hashmap entries at resize.\n", NULL);
        exit(1);
    }

    for (size_t i = 0; i < this->size; i++)
    {
        em_hashmap_entry_t *e = this->_entries[i];
        while (e)
        {
            em_hashmap_entry_t *next = e->_next;

            size_t idx = this->_hsh_func(e->key) % new_size;

            e->_next = entries[idx];
            entries[idx] = e;

            e = next;
        }
    }

    free(this->_entries);
    this->_entries = entries;
    this->size = new_size;
}

em_hashmap_entry_t *_hashmap_iter_get(em_hashmap_iter_t *iter)
{
    return iter->_curr;
}

em_hashmap_iter_info_t *_hashmap_iter_query(em_hashmap_iter_t *iter)
{
    em_hashmap_iter_info_t *res = malloc(sizeof(em_hashmap_iter_info_t));
    res->current = iter->_curr;
    res->count = iter->_count;
    res->index = iter->_idx;
    return res;
}

void _hashmap_iter_next(em_hashmap_iter_t *iter)
{
    if (!iter->has_next)
        return;

    em_hashmap_entry_t *bin_e = iter->_map->_entries[iter->_idx];
    if (bin_e && iter->_curr) // Must search for current node in the chain by key.
    {
        em_hashmap_entry_t *curr = bin_e;
        while (!iter->_map->_cmp_func(curr->key, iter->_curr->key))
            curr = curr->_next;

        if (curr->_next) // If there is another in the chain we move to that.
        {
            iter->_curr = curr->_next;
            iter->_count++;
            iter->has_next = iter->_count < iter->_map->count;
            return;
        }
    }

    /* 
     * We either had no prior node or did not find another in the chain,
     * need to search for another bin with nodes.
     */
    for (size_t bin_idx = iter->_idx + 1; bin_idx < iter->_map->size; bin_idx++)
    {
        bin_e = iter->_map->_entries[bin_idx];
        if (bin_e)
        {
            iter->_idx = bin_idx;
            iter->_curr = bin_e;
            iter->_count++;
            iter->has_next = iter->_count < iter->_map->count;
            return;
        }
    }

    iter->has_next = false;
}

em_hashmap_iter_t *em_hashmap_iterator_from(em_hashmap_t *this, const em_hashmap_iter_info_t *info)
{
    em_hashmap_iter_t *res = malloc(sizeof(em_hashmap_iter_t));
    res->has_next = info->count < this->count;

    res->get = _hashmap_iter_get;
    res->next = _hashmap_iter_next;
    res->query_info = _hashmap_iter_query;

    res->_map = this;

    res->_idx = info->index;
    res->_count = info->count;
    res->_curr = info->current;

    return res;
}

em_hashmap_iter_t *em_hashmap_iterator(em_hashmap_t *this)
{
    em_hashmap_iter_t *res = malloc(sizeof(em_hashmap_iter_t));
    res->has_next = (this->count > 0);
    res->get = _hashmap_iter_get;
    res->next = _hashmap_iter_next;
    res->query_info = _hashmap_iter_query;

    res->_map = this;
    res->_idx = 0;

    res->_count = 1;

    res->_curr = NULL;
    for (size_t i = 0; i < this->size; i++)
    {
        if (this->_entries[i])
        {
            res->_idx = i;
            res->_curr = this->_entries[i];
            break;
        }
    }
    return res;
}

em_hashmap_t *em_hashmap_new(const em_hashmap_desc_t *desc)
{
    if (desc->capacity == 0) 
    {
        EM_LOG("Initial hashmap size must be greater than 0.\n", NULL);
        exit(1);
    }
    em_hashmap_t *res = malloc(sizeof(em_hashmap_t));
    res->size = desc->capacity;
    res->count = 0;
    res->_flags = desc->flags;
    res->_hsh_func = desc->hsh_func;
    res->_cmp_func = desc->cmp_func;
    res->_cln_k_func = desc->cln_k_func;
    res->_cln_v_func = desc->cln_v_func;
    res->_entries = calloc(desc->capacity, sizeof(em_hashmap_entry_t *));
    if (!res->_entries)
    {
        EM_LOG("Failed to allocate hashmap entries at init.\n", NULL);
        exit(1);
    }

    return res;
}

void em_hashmap_put(em_hashmap_t *this, void *key, void *val)
{
    em_hashmap_entry_t *e = malloc(sizeof(em_hashmap_entry_t));
    e->key = key;
    e->val = val;
    e->_next = NULL;

    size_t idx = this->_hsh_func(key) % this->size;
    em_hashmap_entry_t *curr = this->_entries[idx];
    if (curr) 
        e->_next = curr;

    this->_entries[idx] = e;
    this->count++;

    if ((this->_flags & EM_FLAG_NO_RESIZE) == EM_FLAG_NO_RESIZE)
        return;

    /* Grow table if load factor above threshold. */
    if (((float) this->count) / ((float) this->size) >= MAX_LOAD_FACTOR)
        _em_hashmap_resize(this);
}

#include <signal.h>

void *em_hashmap_get(em_hashmap_t *this, const void *key)
{
    /* Search through linked list of elements in the key's bucket for matching key. */
    size_t idx = this->_hsh_func(key) % this->size;
    em_hashmap_entry_t *e = this->_entries[idx];
    while (e)
    {
        if (!e)
            break;

        if (this->_cmp_func(e->key, key))
            return e->val;

        e = e->_next;
    }

    /* Hashmap does not contain the key, no default provided so fail. */
    EM_LOG("Attempting to access invalid element in hashmap get.\n", NULL);

    exit(1);
}

void *em_hashmap_get_or_default(em_hashmap_t *this, const void *key, void *dflt)
{
    /* Search through linked list of elements in the key's bucket for matching key. */
    size_t idx = this->_hsh_func(key) % this->size;
    em_hashmap_entry_t *e = this->_entries[idx];
    while (e)
    {
        if (!e)
            break;

        if (this->_cmp_func(e->key, key))
            return e->val;

        e = e->_next;
    }

    /* Hashmap does not contain the key, return default . */
    return dflt;
}

bool em_hashmap_contains_key(em_hashmap_t *this, const void *key)
{
    /* Search through linked list of elements in the key's bucket for matching key. */
    size_t idx = this->_hsh_func(key) % this->size;
    em_hashmap_entry_t *e = this->_entries[idx];
    while (e)
    {
        if (!e)
            break;

        if (this->_cmp_func(e->key, key))
            return true;

        e = e->_next;
    }

    /* Hashmap does not contain the key. */
    return false;
}

void *em_hashmap_pop(em_hashmap_t *this, const void *key)
{
    /* Search through linked list of elements in the key's bucket for matching key. */
    uint32_t idx = this->_hsh_func(key) % this->size;
    em_hashmap_entry_t *e = this->_entries[idx];
    em_hashmap_entry_t *prev = NULL;

    while (e && !this->_cmp_func(key, e->key))
    {
        prev = e;
        e = e->_next;
    }

    if (!e)
    {
        /* Hashmap does not contain the key. */
        EM_LOG("Attempting to access invalid element in hashmap pop.\n", NULL);
        exit(1);
    }

    void *res = e->val;

    if (!e->_next)
    {
        if (!prev)
            this->_entries[idx] = NULL;
        else 
            prev->_next = NULL;
    }
    else 
    {
        if (!prev)
            this->_entries[idx] = e->_next;
        else 
            prev->_next = e->_next;
    }

    this->count--;
    this->_cln_k_func(e->key);
    free(e);

    return res;
}

void em_hashmap_remove(em_hashmap_t *this, const void *key)
{
    /* Search through linked list of elements in the key's bucket for matching key. */
    uint32_t idx = this->_hsh_func(key) % this->size;
    em_hashmap_entry_t *e = this->_entries[idx];
    em_hashmap_entry_t *prev = NULL;

    while (e && !this->_cmp_func(key, e->key))
    {
        prev = e;
        e = e->_next;
    }

    if (!e)
    {
        /* Hashmap does not contain the key. */
        EM_LOG("Attempting to access invalid element in hashmap remove.\n", NULL);
        exit(1);
    }

    if (!e->_next)
    {
        if (!prev)
            this->_entries[idx] = NULL;
        else 
            prev->_next = NULL;
    }
    else 
    {
        if (!prev)
            this->_entries[idx] = e->_next;
        else 
            prev->_next = e->_next;
    }

    this->count--;
    this->_cln_k_func(e->key);
    this->_cln_v_func(e->val);
    free(e);
}

void em_hashmap_destroy(em_hashmap_t *this)
{
    for (size_t i = 0; i < this->size; i++)
    {
        em_hashmap_entry_t *e = this->_entries[i];
        em_hashmap_entry_t *next;
        while (e)
        {
            next = e->_next;
            this->_cln_k_func(e->key);
            this->_cln_v_func(e->val);
            free(e);
            e = next;
        }
    }
    free(this->_entries);
    free(this);
}

void em_hashmap_filter_out(em_hashmap_t *this, void_fltr_func pred, const void *args)
{
    for (size_t i = 0; i < this->size; i++)
    {
        em_hashmap_entry_t *e = this->_entries[i];
        em_hashmap_entry_t *prev = NULL;
        while (e)
        {
            em_hashmap_entry_t *next = e->_next;

            if (pred(e->val, args))
            {
                if (prev)
                    prev->_next = next;
                else 
                    this->_entries[i] = next;

                this->_cln_k_func(e->key);
                this->_cln_v_func(e->val);
                free(e);
                this->count--;
            }
            else // Only update previous node if it the current was not removed.
            {
                prev = e;
            }

            e = next;
        }
    }
}

em_hashmap_entry_t **em_hashmap_filter_get(em_hashmap_t *this, void_fltr_func pred, const void *args)
{
    em_hashmap_entry_t **res = malloc(this->size * sizeof(em_hashmap_entry_t *));
    size_t ctr = 0;

    for (size_t i = 0; i < this->size; i++)
    {
        em_hashmap_entry_t *e = this->_entries[i];
        em_hashmap_entry_t *next = NULL;
        em_hashmap_entry_t *prev = NULL;
        while (e)
        {
            next = e->_next;

            if (pred(e->val, args))
            {
                if (prev)
                    prev->_next = next;
                else 
                    this->_entries[i] = next;

                /* Set next to null to avoid use after free if e is freed externally. */
                e->_next = NULL;
                res[ctr++] = e;
                this->count--;
            } 
            else // Only update previous if the current node is not removed.
            {
                prev = e;
            }

            e = next;
        }
    }

    if (ctr == 0)
        return NULL;

    res = realloc(res, (ctr + 1) * sizeof(em_hashmap_entry_t *));
    res[ctr] = NULL;
    return res;
}

#endif // EM_HASHMAP_IMPL

#endif
