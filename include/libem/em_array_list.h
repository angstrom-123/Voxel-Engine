#ifndef EM_ARRAY_LIST_H
#define EM_ARRAY_LIST_H 

#ifndef EM_ARRAY_LIST_INCLUDED
#define EM_ARRAY_LIST_INCLUDED

#include "em_global.h"

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h> // exit

/* Declaration template. */
#define DECLARE_ARRAY_LIST_CLN(TYPE, NAME) extern void em_array_list_cln_func_##NAME(TYPE *val);
#define DECLARE_ARRAY_LIST(TYPE, NAME)\
typedef struct em_array_list_##NAME {\
    /* Members. */\
    size_t count;\
    size_t size;\
    em_array_list_t *_al;\
    /* Methods. */\
    /* Get a new iterator for this list. */\
    em_array_list_iter_t *(*iterator)(struct em_array_list_##NAME *this);\
    /* Get a value at an index in the list. */\
    TYPE (*get)(struct em_array_list_##NAME *this, size_t index);\
    /* Get a pointer to the value at an index in the list. */\
    TYPE *(*get_ptr)(struct em_array_list_##NAME *this, size_t index);\
    /* Add an element onto the end of the list. */\
    void (*append)(struct em_array_list_##NAME *this, TYPE value);\
    /* Add an element onto the end of the list. */\
    void (*append_ptr)(struct em_array_list_##NAME *this, TYPE *value_ptr);\
    /* Removes and returns the element at index in the list. */\
    TYPE (*pop)(struct em_array_list_##NAME *this, size_t index);\
    /* Removes and returns a pointer to the element at index in the list. */\
    TYPE *(*pop_ptr)(struct em_array_list_##NAME *this, size_t index);\
    /* Removes and returns the first element in this list. */\
    TYPE (*pop_first)(struct em_array_list_##NAME *this);\
    /* Remove an element at a specified index in the list. */\
    void (*remove)(struct em_array_list_##NAME *this, size_t index);\
    /* Frees all resources associated with the list. WARN: Requires traversal. */\
    void (*destroy)(struct em_array_list_##NAME *this);\
} em_array_list_##NAME##_t;\
\
em_array_list_##NAME##_t *em_array_list_##NAME##_new(const em_array_list_desc_t *desc);

/* Definition template. */
#define ARRAY_LIST(NAME) em_array_list_##NAME##_t
#define ARRAY_LIST_NEW(NAME) em_array_list_##NAME##_new
#define ARRAY_LIST_CLN(NAME) em_array_list_cln_func_##NAME
#define DEFINE_ARRAY_LIST_CLN(TYPE, NAME) void ARRAY_LIST_CLN(NAME)(TYPE *val)
#define DEFINE_ARRAY_LIST(TYPE, NAME)\
\
static void _update_array_list_##NAME(em_array_list_##NAME##_t *this)\
{\
    this->count = this->_al->count;\
    this->size = this->_al->size;\
}\
\
static em_array_list_iter_t *_iterator_array_list_##NAME(em_array_list_##NAME##_t *this)\
{\
    return em_al_iterator(this->_al);\
}\
\
static TYPE _get_array_list_##NAME(em_array_list_##NAME##_t *this, size_t index)\
{\
    TYPE *res = em_al_get(this->_al, index);\
    return *res;\
}\
\
static TYPE *_get_ptr_array_list_##NAME(em_array_list_##NAME##_t *this, size_t index)\
{\
    TYPE *res = em_al_get(this->_al, index);\
    return res;\
}\
\
static void _append_array_list_##NAME(em_array_list_##NAME##_t *this, TYPE value)\
{\
    TYPE *value_ptr = malloc(sizeof(TYPE));\
    *value_ptr = value;\
    em_al_append(this->_al, value_ptr);\
    _update_array_list_##NAME(this);\
}\
\
static void _append_ptr_array_list_##NAME(em_array_list_##NAME##_t *this, TYPE *value_ptr)\
{\
    em_al_append(this->_al, value_ptr);\
    _update_array_list_##NAME(this);\
}\
\
static TYPE _pop_array_list_##NAME(em_array_list_##NAME##_t *this, size_t idx)\
{\
    TYPE *pop = em_al_pop(this->_al, idx);\
    TYPE res = *pop;\
    this->_al->_cln_func(pop);\
    _update_array_list_##NAME(this);\
    return res;\
}\
\
static TYPE *_pop_ptr_array_list_##NAME(em_array_list_##NAME##_t *this, size_t idx)\
{\
    TYPE *res = em_al_pop(this->_al, idx);\
    _update_array_list_##NAME(this);\
    return res;\
}\
\
static void _remove_array_list_##NAME(em_array_list_##NAME##_t *this, size_t index)\
{\
    em_al_remove(this->_al, index);\
    _update_array_list_##NAME(this);\
}\
\
static void _destroy_array_list_##NAME(em_array_list_##NAME##_t *this)\
{\
    em_al_destroy(this->_al);\
    free(this);\
}\
\
em_array_list_##NAME##_t *em_array_list_##NAME##_new(const em_array_list_desc_t *desc)\
{\
    if (!desc->cln_func)\
    {\
        EM_LOG("Array list initialization params are invalid.\n", NULL);\
        exit(1);\
    }\
    \
    em_array_list_##NAME##_t *res = malloc(sizeof(em_array_list_##NAME##_t));\
    if (!res)\
    {\
        EM_LOG("Failed to allocate memory for array list.\n", NULL);\
        exit(1);\
    }\
    \
    res->_al               = em_al_new(desc);\
    \
    res->count             = 0;\
    res->size              = desc->capacity;\
    \
    res->iterator          = _iterator_array_list_##NAME;\
    res->get               = _get_array_list_##NAME;\
    res->get_ptr           = _get_ptr_array_list_##NAME;\
    res->append            = _append_array_list_##NAME;\
    res->append_ptr        = _append_ptr_array_list_##NAME;\
    res->pop               = _pop_array_list_##NAME;\
    res->pop_ptr           = _pop_ptr_array_list_##NAME;\
    res->remove            = _remove_array_list_##NAME;\
    res->destroy           = _destroy_array_list_##NAME;\
    \
    return res;\
}

typedef struct em_array_list_node {
    void *val;
} em_array_list_node_t;

typedef struct em_array_list {
    size_t count;
    size_t size;

    int32_t _flags;
    void_cln_func _cln_func;
    em_array_list_node_t **_entries;
} em_array_list_t;

typedef struct em_array_list_iter {
    bool has_next;
    void (*next)(struct em_array_list_iter *this);
    em_array_list_node_t *(*get)(struct em_array_list_iter *this);

    em_array_list_t *_al;
    em_array_list_node_t *_curr;
    size_t _idx;
    size_t _count;
} em_array_list_iter_t;

typedef struct em_array_list_desc {
    size_t capacity;
    void_cln_func cln_func;
    int32_t flags;
} em_array_list_desc_t;

em_array_list_node_t *_al_iter_get(em_array_list_iter_t *iter);
void _al_iter_next(em_array_list_iter_t *iter);
em_array_list_iter_t *em_al_iterator(em_array_list_t *dll);

em_array_list_t *em_al_new(const em_array_list_desc_t *desc);
void *em_al_get(em_array_list_t *this, size_t index);
em_array_list_node_t *em_al_get_node(em_array_list_t *this, size_t index);
void em_al_append(em_array_list_t *this, void *val);
void *em_al_pop(em_array_list_t *this, size_t index);
void em_al_remove(em_array_list_t *this, size_t index);

void em_al_destroy(em_array_list_t *this);

#endif // EM_ARRAY_LIST_INCLUDED

/*     ______  _______  __    ________  __________   ___________  ______________  _   __
 *    /   /  |/  / __ \/ /   / ____/  |/  / ____/ | / /_  __/   |/_  __/   / __ \/ | / /
 *    / // /|_/ / /_/ / /   / __/ / /|_/ / __/ /  |/ / / / / /| | / /  / // / / /  |/ /
 *   / // /  / / ____/ /___/ /___/ /  / / /___/ /|  / / / / ___ |/ /  / // /_/ / /|  /
 * /___/_/  /_/_/   /_____/_____/_/  /_/_____/_/ |_/ /_/ /_/  |_/_/ /___/\____/_/ |_/
 *
 */

#ifdef EM_ARRAY_LIST_IMPL
#include <stdlib.h> // exit
#include <stdio.h> // fprintf, stderr
#include <math.h> // ceilf

void _em_array_list_resize(em_array_list_t *this)
{
    EM_LOG("Array List resizing.\n", NULL);
    size_t old_size = this->size;
    this->size = ceilf(this->size * 1.5);
    this->_entries = realloc(this->_entries, this->size * sizeof(em_array_list_node_t *));
    if (!this->_entries)
    {
        EM_LOG("Failed to reallocate array list entries at resize.\n", NULL);
        exit(1);
    }

    memset(&this->_entries[old_size], 0, 
           (this->size - old_size) * sizeof(em_array_list_node_t *));
}

em_array_list_node_t *_al_iter_get(em_array_list_iter_t *iter)
{
    return iter->_curr;
}

void _al_iter_next(em_array_list_iter_t *iter)
{
    if (!iter->has_next)
        return;

    iter->_curr = iter->_al->_entries[iter->_idx++];
    iter->_count++;
    iter->has_next = iter->_idx < iter->_al->count;
}

em_array_list_iter_t *em_al_iterator(em_array_list_t *al)
{
    em_array_list_iter_t *res = malloc(sizeof(em_array_list_iter_t));

    res->has_next = al->count > 0;
    res->get = _al_iter_get;
    res->next = _al_iter_next;
    res->_al = al;
    res->_curr = al->_entries[0];
    res->_idx = 1;
    res->_count = 1;

    return res;
}

em_array_list_t *em_al_new(const em_array_list_desc_t *desc)
{
    if (desc->capacity == 0) 
    {
        EM_LOG("Initial array list size must be greater than 0.\n", NULL);
        exit(1);
    }
    em_array_list_t *res = malloc(sizeof(em_array_list_t));
    res->count = 0;
    res->size = desc->capacity;
    res->_flags = desc->flags;
    res->_cln_func = desc->cln_func;
    res->_entries = calloc(desc->capacity, sizeof(em_array_list_node_t *));
    if (!res->_entries)
    {
        EM_LOG("Failed to allocate array list entries at init.\n", NULL);
        exit(1);
    }

    return res;
}

void *em_al_get(em_array_list_t *this, size_t idx)
{
    if (idx >= this->count || this->count == 0)
    {
        EM_LOG("Attempting to access invalid index in array list get.\n", NULL);
        exit(1);
    }

    return (this->_entries[idx]->val);
}

void em_al_append(em_array_list_t *this, void *val)
{
    em_array_list_node_t *n = malloc(sizeof(em_array_list_node_t));
    n->val = val;
    this->_entries[this->count++] = n;

    if ((this->_flags & EM_FLAG_NO_RESIZE) == EM_FLAG_NO_RESIZE)
        return;

    if (this->count == this->size)
        _em_array_list_resize(this);
}

void *em_al_pop(em_array_list_t *this, size_t idx)
{
    if (this->count == 0 || idx >= this->count)
    {
        EM_LOG("Attempting to access invalid element at array list pop.\n", NULL);
        exit(1);
    }

    void *res = this->_entries[idx]->val;
    if ((this->_flags & EM_FLAG_AL_FAST_REMOVE) == EM_FLAG_AL_FAST_REMOVE)
    {
        if (this->count > 1)
        {
            memmove(&this->_entries[idx], &this->_entries[this->count - 1], 
                    sizeof(em_array_list_node_t *));
        }

        this->_entries[this->count - 1] = NULL;
    }
    else 
    {
        free(this->_entries[idx]);

        if (idx < this->count - 1)
        {
            memmove(&this->_entries[idx], &this->_entries[idx + 1], 
                    (this->count - idx - 1) * sizeof(em_array_list_node_t *));
            this->_entries[this->count - 1] = NULL;
        }
    }

    this->count--;
    return res;
}

void em_al_remove(em_array_list_t *this, size_t idx)
{
    if (this->count == 0 || idx >= this->count)
    {
        EM_LOG("Attempting to access invalid element at array list remove.\n", NULL);
        exit(1);
    }

    this->_cln_func(this->_entries[idx]->val);
    if ((this->_flags & EM_FLAG_AL_FAST_REMOVE) == EM_FLAG_AL_FAST_REMOVE)
    {
        if (this->count > 1)
        {
            memmove(&this->_entries[idx], &this->_entries[this->count - 1], 
                    sizeof(em_array_list_node_t *));
        }

        this->_entries[this->count - 1] = NULL;
    }
    else 
    {
        free(this->_entries[idx]);

        if (idx < this->count - 1)
        {
            memmove(&this->_entries[idx], &this->_entries[idx + 1], 
                    (this->count - idx - 1) * sizeof(em_array_list_node_t *));
            this->_entries[this->count - 1] = NULL;
        }
    }

    this->count--;
}

void em_al_destroy(em_array_list_t *this)
{
    for (size_t i = 0; i < this->count; i++)
    {
        em_array_list_node_t *n = this->_entries[i];
        if (n)
        {
            this->_cln_func(n->val);
            free(n);
        }
    }
    free(this->_entries);
    free(this);
}

#endif // EM_ARRAY_LIST_IMPL

#endif
