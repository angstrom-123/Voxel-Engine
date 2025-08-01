#ifndef EM_HASHMAP_H
#define EM_HASHMAP_H

#ifndef EM_HASHMAP_INCLUDED
#define EM_HASHMAP_INCLUDED

#include <malloc.h>
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
    /* Place a key/value pair into the hashmap. NOTE: value must be a heap pointer. */\
    void (*put)(struct em_hashmap_##NAME *this, KTYP key, VTYP *value_ptr);\
    /* Get the value stored at key from the hashmap. If there is none, runtime error. */\
    VTYP (*get)(struct em_hashmap_##NAME *this, KTYP key);\
    /* Get the pointer stored at key from the hashmap. If there is none, runtime error. */\
    VTYP *(*get_ptr)(struct em_hashmap_##NAME *this, KTYP key);\
    /* Get the pointer stored at key from the hashmap. If there is none, returns default. */\
    VTYP *(*get_or_default)(struct em_hashmap_##NAME *this, KTYP key, VTYP *dflt_ptr);\
    /* Checks if the hashmap contains the key. */\
    bool (*contains_key)(struct em_hashmap_##NAME *this, KTYP key);\
    /* Removes and returns the value stored at key. */\
    VTYP (*remove)(struct em_hashmap_##NAME *this, KTYP key);\
    /* Doubles the capacity of the hashmap and rehashes all keys. WARN: Expensive operation. */\
    void (*resize)(struct em_hashmap_##NAME *this);\
} em_hashmap_##NAME##_t;\
\
extern em_hashmap_##NAME##_t *em_hashmap_##NAME##_new(size_t init_size,\
                                                      bool (*cmp_func)(KTYP *lhs, KTYP *rhs),\
                                                      uint32_t (*hsh_func)(KTYP *seed));

/* Definition template. */
#define HASHMAP(NAME) em_hashmap_##NAME##_t
#define HASHMAP_NEW(NAME, size, cmp, hsh) em_hashmap_##NAME##_new(size, cmp, hsh);
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
static void _put_##NAME(em_hashmap_##NAME##_t *this, KTYP k, VTYP *v)\
{\
    KTYP *k_ptr = malloc(sizeof(KTYP));\
    *k_ptr = k;\
    hashmap_put(this->_hashmap, k_ptr, v);\
    _update_##NAME(this);\
}\
\
static VTYP _get_##NAME(em_hashmap_##NAME##_t *this, KTYP k)\
{\
    VTYP *res = (VTYP *) hashmap_get(this->_hashmap, &k);\
    return *res;\
}\
\
static VTYP *_get_ptr_##NAME(em_hashmap_##NAME##_t *this, KTYP k)\
{\
    VTYP *res = (VTYP *) hashmap_get(this->_hashmap, &k);\
    return res;\
}\
\
static VTYP *_get_or_default_##NAME(em_hashmap_##NAME##_t *this, KTYP k, VTYP *dflt)\
{\
    VTYP *res = hashmap_get_or_default(this->_hashmap, &k, dflt);\
    return res;\
}\
\
static bool _contains_key_##NAME(em_hashmap_##NAME##_t *this, KTYP k)\
{\
    bool res = hashmap_contains_key(this->_hashmap, &k);\
    return res;\
}\
\
static VTYP _remove_##NAME(em_hashmap_##NAME##_t *this, KTYP k)\
{\
    VTYP *res = (VTYP *) hashmap_remove(this->_hashmap, &k);\
    _update_##NAME(this);\
    return *res;\
}\
\
static void _resize_##NAME(em_hashmap_##NAME##_t *this)\
{\
    hashmap_resize(this->_hashmap);\
    _update_##NAME(this);\
}\
\
em_hashmap_##NAME##_t *em_hashmap_##NAME##_new(size_t init_size,\
                                               bool (*cmp_func)(KTYP *lhs, KTYP *rhs),\
                                               uint32_t (*hsh_func)(KTYP *seed))\
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
    res->_hashmap       = hashmap_new(init_size,\
                                      (void_cmp_func) cmp_func,\
                                      (void_hsh_func) hsh_func);\
    res->size           = init_size;\
    res->count          = 0;\
    \
    res->put            = _put_##NAME;\
    res->get            = _get_##NAME;\
    res->get_ptr        = _get_ptr_##NAME;\
    res->get_or_default = _get_or_default_##NAME;\
    res->contains_key   = _contains_key_##NAME;\
    res->remove         = _remove_##NAME;\
    res->resize         = _resize_##NAME;\
    \
    return res;\
}

typedef bool (*void_cmp_func)(const void *, const void *);
typedef uint32_t (*void_hsh_func)(const void *);

typedef struct em_entry {
    void *key;
    void *val;
    struct em_entry *next;
} em_entry_t;

typedef struct em_hashmap {
    size_t size;
    size_t count;

    void_cmp_func _cmp_func;
    void_hsh_func _hsh_func;
    em_entry_t **_entries;
} em_hashmap_t;

extern uint32_t em_hash(int32_t key);

extern em_hashmap_t *hashmap_new(size_t init_size, void_cmp_func cmp, void_hsh_func hsh);
extern void hashmap_put(em_hashmap_t *this, void *key, void *val);
extern void *hashmap_get(em_hashmap_t *this, void *key);
extern void *hashmap_get_or_default(em_hashmap_t *this, void *key, void *val);
extern bool hashmap_contains_key(em_hashmap_t *this, void *key);
extern void *hashmap_remove(em_hashmap_t *this, void *key);
extern void hashmap_resize(em_hashmap_t *this);

#endif // EM_HASHMAP_INCLUDED

/*     ______  _______  __    ________  __________   ___________  ______________  _   __
 *    /   /  |/  / __ \/ /   / ____/  |/  / ____/ | / /_  __/   |/_  __/   / __ \/ | / /
 *    / // /|_/ / /_/ / /   / __/ / /\_/ / __/ /  |/ / / / / /| | / /  / // / / /  |/ /
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

em_hashmap_t *hashmap_new(size_t init_size, void_cmp_func cmp, void_hsh_func hsh)
{
    if (init_size == 0) 
    {
        fprintf(stderr, "Initial hashmap size must be greater than 0.\n");
        exit(1);
    }
    em_hashmap_t *res = malloc(sizeof(em_hashmap_t));
    res->size = init_size;
    res->count = 0;
    res->_hsh_func = hsh;
    res->_cmp_func = cmp;
    res->_entries = calloc(init_size,  sizeof(void *));
    return res;
}

void hashmap_resize(em_hashmap_t *this)
{
    size_t new_size = this->size * 2;
    em_entry_t **entries = calloc(new_size, sizeof(em_entry_t *));

    /* Rehash entries into larger table. */
    for (size_t i = 0; i < this->size; i++)
    {
        em_entry_t *e = this->_entries[i];
        em_entry_t *head = NULL;

        /* Add all entries in the chain to the table. */
        while (e)
        {
            size_t idx = this->_hsh_func(e->key) % new_size;
            if (!head)
            {
                entries[idx] = e;
                head = e;
            }
            else
            {
                head->next = e;
                head = head->next;
            }
            e = e->next;
        }
    }
    free(this->_entries);
    this->_entries = entries;
    this->size = new_size;
}

void hashmap_put(em_hashmap_t *this, void *key, void *val)
{
    em_entry_t *e = malloc(sizeof(em_entry_t));
    e->key = key;
    e->val = val;
    e->next = NULL;

    size_t idx = this->_hsh_func(key) % this->size;
    em_entry_t *curr = this->_entries[idx];
    if (!curr) // No elements in bin yet.
    {
        this->_entries[idx] = e;
    }
    else // Hash colission, add entry to end of linked list.
    {
        em_entry_t *prev = NULL;
        while (curr)
        {
            prev = curr;
            curr = curr->next;
        }
        prev->next = e;
    }

    this->count++;

    /* Grow table if load factor above threshold. */
    if (((float) this->count) / ((float) this->size) >= MAX_LOAD_FACTOR)
        hashmap_resize(this);
}

void *hashmap_get(em_hashmap_t *this, void *key)
{
    /* Search through linked list of elements in the key's bucket for matching key. */
    size_t idx = this->_hsh_func(key) % this->size;
    em_entry_t *e = this->_entries[idx];
    while (e)
    {
        if (!e)
            break;

        if (this->_cmp_func(e->key, key))
            return e->val;

        e = e->next;
    }

    /* Hashmap does not contain the key, no default provided so crash. */
    fprintf(stderr, "Attempting to access invalid element in hashmap get.\n");
    exit(1);
}

void *hashmap_get_or_default(em_hashmap_t *this, void *key, void *dflt)
{
    /* Search through linked list of elements in the key's bucket for matching key. */
    size_t idx = this->_hsh_func(key) % this->size;
    em_entry_t *e = this->_entries[idx];
    while (e)
    {
        if (!e)
            break;

        if (this->_cmp_func(e->key, key))
            return e->val;

        e = e->next;
    }

    /* Hashmap does not contain the key, return default . */
    return dflt;
}

bool hashmap_contains_key(em_hashmap_t *this, void *key)
{
    /* Search through linked list of elements in the key's bucket for matching key. */
    size_t idx = this->_hsh_func(key) % this->size;
    em_entry_t *e = this->_entries[idx];
    while (e)
    {
        if (!e)
            break;

        if (this->_cmp_func(e->key, key))
            return true;

        e = e->next;
    }

    /* Hashmap does not contain the key. */
    return false;
}

void *hashmap_remove(em_hashmap_t *this, void *key)
{
    /* Search through linked list of elements in the key's bucket for matching key. */
    size_t idx = this->_hsh_func(key) % this->size;
    em_entry_t *e = this->_entries[idx];
    em_entry_t *prev = NULL;
    while (e)
    {
        if (!e)
            break;

        if (this->_cmp_func(e->key, key))
        {
            prev->next = e->next;
            void *val = e->val;

            free(e);
            this->count--;

            return val;
        }
        prev = e;
        e = e->next;
    }

    /* Hashmap does not contain the key. */
    fprintf(stderr, "Attempting to access invalid element in hashmap remove.\n");
    exit(1);
}
#endif // EM_HASHMAP_IMPL

#endif
