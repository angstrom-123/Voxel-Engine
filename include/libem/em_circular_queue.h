#ifndef EM_CIRCULAR_QUEUE_H
#define EM_CIRCULAR_QUEUE_H

#ifndef EM_CIRCULAR_QUEUE_INCLUDED
#define EM_CIRCULAR_QUEUE_INCLUDED

#include "em_global.h"

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h> // exit

/* Declaration template. */
#define DECLARE_CIRCULAR_QUEUE_CLN(TYPE, NAME) extern void em_circular_queue_cln_func_##NAME(TYPE *val);
#define DECLARE_CIRCULAR_QUEUE(TYPE, NAME)\
typedef struct em_circular_queue_##NAME {\
    /* Members. */\
    size_t size;\
    size_t count;\
    em_circular_queue_t *_cq;\
    /* Methods. */\
    /* Get a new iterator for this queue. */\
    em_circular_queue_iter_t *(*iterator)(struct em_circular_queue_##NAME *this);\
    /* Add an element to the end of the queue. */\
    void (*enqueue)(struct em_circular_queue_##NAME *this, TYPE val);\
    /* Add an element's pointer to the end of the queue. */\
    void (*enqueue_ptr)(struct em_circular_queue_##NAME *this, TYPE *val);\
    /* Remove and return the first element in the queue. */\
    TYPE (*dequeue)(struct em_circular_queue_##NAME *this);\
    /* Remove and return a pointer to the first element in the queue. */\
    TYPE *(*dequeue_ptr)(struct em_circular_queue_##NAME *this);\
    /* Return the first element in the queue. */\
    TYPE (*peek)(struct em_circular_queue_##NAME *this);\
    /* Return a pointer to the first element in the queue. */\
    TYPE *(*peek_ptr)(struct em_circular_queue_##NAME *this);\
    /* Frees all resources associated with the list. WARN: Requires traversal. */\
    void (*destroy)(struct em_circular_queue_##NAME *this);\
} em_circular_queue_##NAME##_t;\
\
em_circular_queue_##NAME##_t *em_circular_queue_##NAME##_new(const em_circular_queue_desc_t *desc);

/* Definition template. */
#define CIRCULAR_QUEUE(NAME) em_circular_queue_##NAME##_t
#define CIRCULAR_QUEUE_NEW(NAME) em_circular_queue_##NAME##_new
#define CIRCULAR_QUEUE_CLN(NAME) em_circular_queue_cln_func_##NAME
#define DEFINE_CIRCULAR_QUEUE_CLN(TYPE, NAME) void CIRCULAR_QUEUE_CLN(NAME)(TYPE *val)
#define DEFINE_CIRCULAR_QUEUE(TYPE, NAME)\
\
static void _update_circular_queue_##NAME(em_circular_queue_##NAME##_t *this)\
{\
    this->count = this->_cq->count;\
    this->size = this->_cq->size;\
}\
\
static em_circular_queue_iter_t *_iterator_circular_queue_##NAME(em_circular_queue_##NAME##_t *this)\
{\
    return em_cq_iterator(this->_cq);\
}\
\
static void _enqueue_circular_queue_##NAME(em_circular_queue_##NAME##_t *this, TYPE value)\
{\
    TYPE *value_ptr = malloc(sizeof(TYPE));\
    *value_ptr = value;\
    em_cq_enqueue(this->_cq, value_ptr);\
    _update_circular_queue_##NAME(this);\
}\
\
static void _enqueue_ptr_circular_queue_##NAME(em_circular_queue_##NAME##_t *this, TYPE *value)\
{\
    em_cq_enqueue(this->_cq, value);\
    _update_circular_queue_##NAME(this);\
}\
\
static TYPE _dequeue_circular_queue_##NAME(em_circular_queue_##NAME##_t *this)\
{\
    TYPE *dq = em_cq_dequeue(this->_cq);\
    TYPE res = *dq;\
    this->_cq->_cln_func(dq);\
    _update_circular_queue_##NAME(this);\
    return res;\
}\
\
static TYPE *_dequeue_ptr_circular_queue_##NAME(em_circular_queue_##NAME##_t *this)\
{\
    TYPE *res = em_cq_dequeue(this->_cq);\
    _update_circular_queue_##NAME(this);\
    return res;\
}\
\
static TYPE _peek_circular_queue_##NAME(em_circular_queue_##NAME##_t *this)\
{\
    TYPE *dq = em_cq_peek(this->_cq);\
    TYPE res = *dq;\
    return res;\
}\
\
static TYPE *_peek_ptr_circular_queue_##NAME(em_circular_queue_##NAME##_t *this)\
{\
    TYPE *res = em_cq_peek(this->_cq);\
    return res;\
}\
\
static void _destroy_circular_queue##NAME(em_circular_queue_##NAME##_t *this)\
{\
    em_cq_destroy(this->_cq);\
    free(this);\
}\
\
em_circular_queue_##NAME##_t *em_circular_queue_##NAME##_new(const em_circular_queue_desc_t *desc)\
{\
    if (desc->capacity == 0 || !desc->cln_func)\
    {\
        EM_LOG("Circular queue initialization params are invalid.\n", NULL);\
        exit(1);\
    }\
    \
    em_circular_queue_##NAME##_t *res = malloc(sizeof(em_circular_queue_##NAME##_t));\
    if (!res)\
    {\
        EM_LOG("Failed to allocate memory for circular queue.\n", NULL);\
        exit(1);\
    }\
    \
    res->_cq         = em_cq_new(desc);\
    \
    res->count       = 0;\
    res->size        = desc->capacity;\
    \
    res->iterator    = _iterator_circular_queue_##NAME;\
    res->enqueue     = _enqueue_circular_queue_##NAME;\
    res->enqueue_ptr = _enqueue_ptr_circular_queue_##NAME;\
    res->dequeue     = _dequeue_circular_queue_##NAME;\
    res->dequeue_ptr = _dequeue_ptr_circular_queue_##NAME;\
    res->peek        = _peek_circular_queue_##NAME;\
    res->peek_ptr    = _peek_ptr_circular_queue_##NAME;\
    res->destroy     = _destroy_circular_queue##NAME;\
    \
    return res;\
}

typedef struct em_circular_queue_node {
    void *val;
} em_circular_queue_node_t;

typedef struct em_circular_queue {
    size_t size;
    size_t count;

    int32_t _flags;
    void_cln_func _cln_func;
    size_t _head;
    size_t _tail;
    em_circular_queue_node_t **_entries;
} em_circular_queue_t;

typedef struct em_circular_queue_iter {
    bool has_next;
    void (*next)(struct em_circular_queue_iter *this);
    em_circular_queue_node_t *(*get)(struct em_circular_queue_iter *this);

    em_circular_queue_node_t *_curr;
    em_circular_queue_t *_cq;
    size_t _cnt;
    size_t _idx;
} em_circular_queue_iter_t;

typedef struct em_circular_queue_desc {
    size_t capacity;
    void_cln_func cln_func;
    int32_t flags;
} em_circular_queue_desc_t;

em_circular_queue_node_t *_cq_iter_get(em_circular_queue_iter_t *iter);
void _cq_iter_next(em_circular_queue_iter_t *iter);
em_circular_queue_iter_t *em_cq_iterator(em_circular_queue_t *dll);

em_circular_queue_t *em_cq_new(const em_circular_queue_desc_t *desc);
void em_cq_enqueue(em_circular_queue_t *this, void *val);
void *em_cq_dequeue(em_circular_queue_t *this);
void *em_cq_peek(em_circular_queue_t *this);

void em_cq_destroy(em_circular_queue_t *this);

#endif // EM_CIRCULAR_QUEUE_INCLUDED

/*     ______  _______  __    ________  __________   ___________  ______________  _   __
 *    /   /  |/  / __ \/ /   / ____/  |/  / ____/ | / /_  __/   |/_  __/   / __ \/ | / /
 *    / // /|_/ / /_/ / /   / __/ / /|_/ / __/ /  |/ / / / / /| | / /  / // / / /  |/ /
 *   / // /  / / ____/ /___/ /___/ /  / / /___/ /|  / / / / ___ |/ /  / // /_/ / /|  /
 * /___/_/  /_/_/   /_____/_____/_/  /_/_____/_/ |_/ /_/ /_/  |_/_/ /___/\____/_/ |_/
 *
 */

#ifdef EM_CIRCULAR_QUEUE_IMPL

#include <stdio.h> // fprintf, stderr
#include <math.h>
#include <string.h>

void _em_cq_resize(em_circular_queue_t *this)
{
    EM_LOG("Circular Queue resizing.\n", NULL);
    size_t new_size = ceilf(this->size * 1.5);
    size_t old_size = this->size;
    size_t difference = new_size - this->size;

    this->size = new_size;
    this->_entries = realloc(this->_entries, new_size * sizeof(em_circular_queue_node_t *));
    if (!this->_entries)
    {
        EM_LOG("Failed to reallocate entries array in circular queue resize.\n", NULL);
        exit(1);
    }

    if (this->_head == 0)
        return; // Since head is at 0, reallocated space is already above tail. */

    /* Shift head and all entries above it to occupy new space which frees up sspace above tail. */
    size_t num_above_head = old_size - (this->_head + 1);
    memmove(&this->_entries[this->_head + difference], &this->_entries[this->_head],
            num_above_head * sizeof(em_circular_queue_node_t *));
    memset(&this->_entries[this->_head], 0, num_above_head * sizeof(em_circular_queue_node_t *));
    this->_head += difference;
}

em_circular_queue_node_t *_cq_iter_get(em_circular_queue_iter_t *iter)
{
    return iter->_curr;
}

void _cq_iter_next(em_circular_queue_iter_t *iter)
{
    if (!iter->has_next)
        return;

    iter->_curr = iter->_cq->_entries[++iter->_idx];
    if (iter->_idx == iter->_cq->size)
        iter->_idx = 0;
    iter->_cnt++;
    iter->has_next = iter->_cnt < iter->_cq->count;
}

em_circular_queue_iter_t *em_cq_iterator(em_circular_queue_t *cq)
{
    em_circular_queue_iter_t *res = malloc(sizeof(em_circular_queue_iter_t));
    res->has_next = cq->count > 0;
    res->get = _cq_iter_get;
    res->next = _cq_iter_next;
    res->_curr = cq->_entries[cq->_head];
    res->_idx = cq->_head;
    res->_cnt = 0;
    res->_cq = cq;
    return res;
}

em_circular_queue_t *em_cq_new(const em_circular_queue_desc_t *desc)
{
    em_circular_queue_t *res = malloc(sizeof(em_circular_queue_t));

    res->_entries = calloc(desc->capacity, sizeof(em_circular_queue_node_t *));
    res->_cln_func = desc->cln_func;
    res->_flags = desc->flags;
    res->_head = 0;
    res->_tail = 0;
    res->count = 0;
    res->size = desc->capacity;

    return res;
}

void em_cq_enqueue(em_circular_queue_t *this, void *val)
{
    if (this->count == this->size)
    {
        if ((this->_flags & EM_FLAG_NO_RESIZE) == EM_FLAG_NO_RESIZE)
        {
            EM_LOG("Attempting to enqueue to full circular queue.\n", NULL);
            exit(1);
        }

        _em_cq_resize(this);
    }

    if (this->_tail == this->size)
        this->_tail = 0;

    em_circular_queue_node_t *n = malloc(sizeof(em_circular_queue_node_t));
    n->val = val;
    this->_entries[this->_tail] = n;

    this->count++;
    this->_tail++;
}

void *em_cq_dequeue(em_circular_queue_t *this)
{
    if (this->count == 0)
    {
        EM_LOG("Attempting to dequeue from an empty circular queue.\n", NULL);
        exit(1);
    }

    if (this->_head == this->size)
        this->_head = 0;

    em_circular_queue_node_t *n = this->_entries[this->_head];

    void *res = n->val;
    free(n);
    this->_entries[this->_head] = NULL;

    this->count--;
    this->_head++;

    return res;
}

void *em_cq_peek(em_circular_queue_t *this)
{
    if (this->count == 0)
    {
        EM_LOG("Attempting to peek an empty queue.\n", NULL);
        exit(1);
    }

    return this->_entries[this->_head]->val;
}

void em_cq_destroy(em_circular_queue_t *this)
{
    while (this->count > 0)
    {
        this->_cln_func(this->_entries[this->_head]->val);
        free(this->_entries[this->_head]);
        this->_head++;
        if (this->_head == this->size)
            this->_head = 0;

        this->count--;
    }

    free(this->_entries);
    free(this);
}

#endif // EM_DLL_IMPL

#endif
