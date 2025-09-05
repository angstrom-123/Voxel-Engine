#ifndef EM_HASHMAP_H
#define EM_HASHMAP_H

#ifndef EM_HASHMAP_INCLUDED
#define EM_HASHMAP_INCLUDED

#include <malloc.h>
#include <memory.h>
#include <stdlib.h> // exit
#include <stddef.h> // size_t
#include <stdint.h>
#include <stdbool.h>

/* Declaration template. */
#define DECLARE_HASHMAP_CMP(KTYP, NAME) extern bool em_hashmap_cmp_func_##NAME(KTYP *lhs, KTYP *rhs);
#define DECLARE_HASHMAP_HSH(KTYP, NAME) extern uint32_t em_hashmap_hsh_func_##NAME(KTYP *key);
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
    /* Doubles the capacity of the hashmap and rehashes all keys. WARN: Expensive. */\
    void (*resize)(struct em_hashmap_##NAME *this);\
    /* Frees all resources associated with this hashmap. */\
    void (*destroy)(struct em_hashmap_##NAME *this);\
    /* Removes all entries whose value matches the predicate. */\
    void (*filter_out)(struct em_hashmap_##NAME *this, bool (*pred)(VTYP *val, void *args),\
                       void *args);\
    /* Removes and returns all entries that match the predicate from the map. */\
    em_hashmap_entry_t **(*filter_get)(struct em_hashmap_##NAME *this, bool (*pred)(VTYP *val, void *args), void *args);\
} em_hashmap_##NAME##_t;\
\
em_hashmap_##NAME##_t *em_hashmap_##NAME##_new(size_t init_size,\
                                               bool (*cmp_func)(KTYP *lhs, KTYP *rhs),\
                                               uint32_t (*hsh_func)(KTYP *seed),\
                                               bool no_resize);\

/* Definition template. */
#define EM_NO_RESIZE true
#define EM_DO_RESIZE false
#define HASHMAP(NAME) em_hashmap_##NAME##_t
#define HASHMAP_NEW(NAME, size, cmp, hsh, rsz) em_hashmap_##NAME##_new(size, cmp, hsh, rsz)
#define HASHMAP_CMP(NAME) em_hashmap_cmp_func_##NAME
#define HASHMAP_HSH(NAME) em_hashmap_hsh_func_##NAME
#define DEFINE_HASHMAP_CMP(KTYP, NAME) bool HASHMAP_CMP(NAME)(KTYP *lhs, KTYP *rhs)
#define DEFINE_HASHMAP_HSH(KTYP, NAME) uint32_t HASHMAP_HSH(NAME)(KTYP *key)
#define DEFINE_HASHMAP(KTYP, VTYP, NAME)\
\
static void _update_##NAME(em_hashmap_##NAME##_t *this)\
{\
    this->size = this->_hashmap->size;\
    this->count = this->_hashmap->count;\
}\
\
static em_hashmap_iter_t *_iterator_##NAME(em_hashmap_##NAME##_t *this)\
{\
    return em_hashmap_iterator(this->_hashmap);\
}\
\
static void _put_##NAME(em_hashmap_##NAME##_t *this, KTYP k, VTYP v)\
{\
    KTYP *k_ptr = malloc(sizeof(KTYP));\
    *k_ptr = k;\
    VTYP *v_ptr = malloc(sizeof(VTYP));\
    *v_ptr = v;\
    em_hashmap_put(this->_hashmap, k_ptr, v_ptr);\
    _update_##NAME(this);\
}\
\
static void _put_ptr_##NAME(em_hashmap_##NAME##_t *this, KTYP k, VTYP *v)\
{\
    KTYP *k_ptr = malloc(sizeof(KTYP));\
    *k_ptr = k;\
    em_hashmap_put(this->_hashmap, k_ptr, v);\
    _update_##NAME(this);\
}\
\
static VTYP _get_##NAME(em_hashmap_##NAME##_t *this, KTYP k)\
{\
    VTYP *res = em_hashmap_get(this->_hashmap, &k);\
    return *res;\
}\
\
static VTYP *_get_ptr_##NAME(em_hashmap_##NAME##_t *this, KTYP k)\
{\
    VTYP *res = (VTYP *) em_hashmap_get(this->_hashmap, &k);\
    return res;\
}\
\
static VTYP *_get_or_default_##NAME(em_hashmap_##NAME##_t *this, KTYP k, VTYP *dflt)\
{\
    VTYP *res = em_hashmap_get_or_default(this->_hashmap, &k, dflt);\
    return res;\
}\
\
static bool _contains_key_##NAME(em_hashmap_##NAME##_t *this, KTYP k)\
{\
    bool res = em_hashmap_contains_key(this->_hashmap, &k);\
    return res;\
}\
\
static VTYP _pop_##NAME(em_hashmap_##NAME##_t *this, KTYP k)\
{\
    VTYP *pop = em_hashmap_pop(this->_hashmap, &k);\
    VTYP res = *pop;\
    free(pop);\
    _update_##NAME(this);\
    return res;\
}\
\
static VTYP *_pop_ptr_##NAME(em_hashmap_##NAME##_t *this, KTYP k)\
{\
    VTYP *res = em_hashmap_pop(this->_hashmap, &k);\
    _update_##NAME(this);\
    return res;\
}\
\
static void _remove_##NAME(em_hashmap_##NAME##_t *this, KTYP k)\
{\
    em_hashmap_remove(this->_hashmap, &k);\
    _update_##NAME(this);\
}\
\
static void _resize_##NAME(em_hashmap_##NAME##_t *this)\
{\
    em_hashmap_resize(this->_hashmap);\
    _update_##NAME(this);\
}\
\
static void _destroy_##NAME(em_hashmap_##NAME##_t *this)\
{\
    em_hashmap_destroy(this->_hashmap);\
    free(this);\
}\
\
static void _filter_out_##NAME(em_hashmap_##NAME##_t *this,\
                               bool (*predicate)(VTYP *val, void *args),\
                               void *args)\
{\
    em_hashmap_filter_out(this->_hashmap, (void_fltr_func) predicate, args);\
    _update_##NAME(this);\
}\
\
static em_hashmap_entry_t **_filter_get_##NAME(em_hashmap_##NAME##_t *this,\
                                               bool (*predicate)(VTYP *val, void *args),\
                                               void *args)\
{\
    em_hashmap_entry_t **res = em_hashmap_filter_get(this->_hashmap, (void_fltr_func) predicate, args);\
    _update_##NAME(this);\
    return res;\
}\
\
em_hashmap_##NAME##_t *em_hashmap_##NAME##_new(size_t init_size,\
                                               bool (*cmp_func)(KTYP *lhs, KTYP *rhs),\
                                               uint32_t (*hsh_func)(KTYP *seed),\
                                               bool no_resize)\
{\
    if (init_size == 0 || !cmp_func || !hsh_func)\
    {\
        fprintf(stderr, "Hashmap initialization params are invalid.\n");\
        exit(1);\
    }\
    \
    em_hashmap_##NAME##_t *res = malloc(sizeof(em_hashmap_##NAME##_t));\
    if (!res)\
    {\
        fprintf(stderr, "Failed to allocate memory for hashmap.\n");\
        exit(1);\
    }\
    \
    res->_hashmap       = em_hashmap_new(&(em_hashmap_desc_t) {\
                            .capacity = init_size,\
                            .cmp_func = (void_cmp_func) cmp_func,\
                            .hsh_func = (void_hsh_func) hsh_func,\
                            .no_resize = no_resize\
                        });\
    res->size           = init_size;\
    res->count          = 0;\
    \
    res->iterator       = _iterator_##NAME;\
    res->put            = _put_##NAME;\
    res->put_ptr        = _put_ptr_##NAME;\
    res->get            = _get_##NAME;\
    res->get_ptr        = _get_ptr_##NAME;\
    res->get_or_default = _get_or_default_##NAME;\
    res->contains_key   = _contains_key_##NAME;\
    res->pop            = _pop_##NAME;\
    res->pop_ptr        = _pop_ptr_##NAME;\
    res->remove         = _remove_##NAME;\
    res->resize         = _resize_##NAME;\
    res->destroy        = _destroy_##NAME;\
    res->filter_out     = _filter_out_##NAME;\
    res->filter_get     = _filter_get_##NAME;\
    \
    return res;\
}

typedef bool (*void_cmp_func)(const void *, const void *);
typedef uint32_t (*void_hsh_func)(const void *);
typedef bool (*void_fltr_func)(void *, void *);

typedef struct em_hashmap_desc {
    size_t capacity;
    void_cmp_func cmp_func;
    void_hsh_func hsh_func;
    bool no_resize;
} em_hashmap_desc_t;

typedef struct em_hashmap_entry {
    void *key;
    void *val;

    struct em_hashmap_entry *_next;
} em_hashmap_entry_t;

typedef struct em_hashmap {
    size_t size;
    size_t count;

    bool _no_resize;
    void_cmp_func _cmp_func;
    void_hsh_func _hsh_func;
    em_hashmap_entry_t **_entries;
} em_hashmap_t;

typedef struct em_hashmap_iter {
    bool has_next;
    void (*next)(struct em_hashmap_iter *this);
    em_hashmap_entry_t *(*get)(struct em_hashmap_iter *this);

    em_hashmap_t *_map;
    em_hashmap_entry_t *_curr;
    size_t _idx;
    size_t _count;
} em_hashmap_iter_t;

uint32_t em_hash(int32_t key);

em_hashmap_entry_t *_hashmap_iter_get(em_hashmap_iter_t *iter);
void _hashmap_iter_next(em_hashmap_iter_t *iter);
em_hashmap_iter_t *em_hashmap_iterator(em_hashmap_t *this);

em_hashmap_t *em_hashmap_new(const em_hashmap_desc_t *desc);
void em_hashmap_resize(em_hashmap_t *this);
void em_hashmap_put(em_hashmap_t *this, void *key, void *val);
void *em_hashmap_get(em_hashmap_t *this, void *key);
void *em_hashmap_get_or_default(em_hashmap_t *this, void *key, void *val);
bool em_hashmap_contains_key(em_hashmap_t *this, void *key);
void *em_hashmap_pop(em_hashmap_t *this, void *key);
void em_hashmap_remove(em_hashmap_t *this, void *key);
void em_hashmap_destroy(em_hashmap_t *this);
void em_hashmap_filter_out(em_hashmap_t *this, void_fltr_func pred, void *args);
void em_hashmap_filter_to(em_hashmap_t *this, em_hashmap_t *targ, void_fltr_func pred, void *args);

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

em_hashmap_entry_t *_hashmap_iter_get(em_hashmap_iter_t *iter)
{
    return iter->_curr;
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

em_hashmap_iter_t *em_hashmap_iterator(em_hashmap_t *this)
{
    em_hashmap_iter_t *res = malloc(sizeof(em_hashmap_iter_t));
    res->has_next = (this->count > 0);
    res->get = _hashmap_iter_get;
    res->next = _hashmap_iter_next;

    res->_map = this;
    res->_idx = 0;
    res->_count = 0;

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
        fprintf(stderr, "Initial hashmap size must be greater than 0.\n");
        exit(1);
    }
    em_hashmap_t *res = malloc(sizeof(em_hashmap_t));
    res->size = desc->capacity;
    res->count = 0;
    res->_no_resize = desc->no_resize;
    res->_hsh_func = desc->hsh_func;
    res->_cmp_func = desc->cmp_func;
    res->_entries = calloc(desc->capacity, sizeof(em_hashmap_entry_t *));
    if (!res->_entries)
    {
        fprintf(stderr, "Failed to allocate hashmap entries at init.\n");
        exit(1);
    }

    return res;
}

void em_hashmap_resize(em_hashmap_t *this)
{
    size_t new_size = this->size * 2;
    em_hashmap_entry_t **entries = calloc(new_size, sizeof(em_hashmap_entry_t *));
    if (!entries)
    {
        fprintf(stderr, "Failed to reallocate hashmap entries at resize.\n");
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

    if (this->_no_resize)
        return;

    /* Grow table if load factor above threshold. */
    if (((float) this->count) / ((float) this->size) >= MAX_LOAD_FACTOR)
        em_hashmap_resize(this);
}

void *em_hashmap_get(em_hashmap_t *this, void *key)
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

    /* Hashmap does not contain the key, no default provided so crash. */
    fprintf(stderr, "Attempting to access invalid element in hashmap get.\n");
    exit(1);
}

void *em_hashmap_get_or_default(em_hashmap_t *this, void *key, void *dflt)
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

bool em_hashmap_contains_key(em_hashmap_t *this, void *key)
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

void *em_hashmap_pop(em_hashmap_t *this, void *key)
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
        fprintf(stderr, "Attempting to access invalid element in hashmap pop.\n");
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
    free(e->key);
    free(e);

    return res;
}

void em_hashmap_remove(em_hashmap_t *this, void *key)
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
        fprintf(stderr, "Attempting to access invalid element in hashmap remove.\n");
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
    free(e->key);
    free(e->val);
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
            free(e->val);
            free(e->key);
            free(e);
            e = next;
        }
    }
    free(this->_entries);
    free(this);
}

void em_hashmap_filter_out(em_hashmap_t *this, void_fltr_func pred, void *args)
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

                free(e->val);
                free(e->key);
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

em_hashmap_entry_t **em_hashmap_filter_get(em_hashmap_t *this, void_fltr_func pred, void *args)
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
