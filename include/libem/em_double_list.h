#ifndef EM_DOUBLE_LIST_H
#define EM_DOUBLE_LIST_H 

#ifndef EM_DOUBLE_LIST_INCLUDED
#define EM_DOUBLE_LIST_INCLUDED

#include "em_global.h"

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h> // exit

/* Declaration template. */
#define DECLARE_DOUBLE_LIST_CLN(TYPE, NAME) extern void em_double_list_cln_func_##NAME(TYPE *val);
#define DECLARE_DOUBLE_LIST(TYPE, NAME)\
typedef struct em_double_list_##NAME {\
    /* Members. */\
    size_t count;\
    em_double_list_t *_dll;\
    /* Methods. */\
    /* Get a new iterator for this dll. */\
    em_double_list_iter_t *(*iterator)(struct em_double_list_##NAME *this);\
    /* Return the value of the head of the list. */\
    TYPE (*get_first)(struct em_double_list_##NAME *this);\
    /* Return the value of the head of the list. */\
    TYPE *(*get_ptr_first)(struct em_double_list_##NAME *this);\
    /* Return the value of the tail of the list. */\
    TYPE (*get_last)(struct em_double_list_##NAME *this);\
    /* Return the value of the tail of the list. */\
    TYPE *(*get_ptr_last)(struct em_double_list_##NAME *this);\
    /* Get a value at an index in the linked list. WARN: Requires traversal. */\
    TYPE (*get)(struct em_double_list_##NAME *this, size_t index);\
    /* Get a pointer to the value at an index in the linked list. WARN: Requires traversal. */\
    TYPE *(*get_ptr)(struct em_double_list_##NAME *this, size_t index);\
    /* Get a pointer to the node in the dll at an index in the linked list. WARN: Requires traversal. */\
    em_double_list_node_t *(*get_node)(struct em_double_list_##NAME *this, size_t index);\
    /* Add a new element after an existing element in the list. */\
    void (*insert_after)(struct em_double_list_##NAME *this, em_double_list_node_t *node, TYPE value);\
    /* Add a new element after an existing element in the list. */\
    void (*insert_ptr_after)(struct em_double_list_##NAME *this, em_double_list_node_t *node, TYPE *value_ptr);\
    /* Add a new element before an existing element in the list. */\
    void (*insert_before)(struct em_double_list_##NAME *this, em_double_list_node_t *node, TYPE value);\
    /* Add a new element before an existing element in the list. */\
    void (*insert_ptr_before)(struct em_double_list_##NAME *this, em_double_list_node_t *node, TYPE *value_ptr);\
    /* Add a new element at a specified index in the list. WARN: Requires traversal. */\
    void (*insert_at)(struct em_double_list_##NAME *this, size_t index, TYPE value);\
    /* Add a new element at a specified index in the list. WARN: Requires traversal. */\
    void (*insert_ptr_at)(struct em_double_list_##NAME *this, size_t index, TYPE *value_ptr);\
    /* Add an element onto the end of the linked list. */\
    void (*append)(struct em_double_list_##NAME *this, TYPE value);\
    /* Add an element onto the end of the linked list. */\
    void (*append_ptr)(struct em_double_list_##NAME *this, TYPE *value_ptr);\
    /* Removes and returns the element at index in the list. WARN: Requires traversal. */\
    TYPE (*pop)(struct em_double_list_##NAME *this, size_t index);\
    /* Removes and returns the element at index in the list. WARN: Requires traversal. */\
    TYPE *(*pop_ptr)(struct em_double_list_##NAME *this, size_t index);\
    /* Removes and returns the first element in this list. */\
    TYPE (*pop_first)(struct em_double_list_##NAME *this);\
    /* Removes and returns a pointer to the first element in this list. */\
    TYPE *(*pop_ptr_first)(struct em_double_list_##NAME *this);\
    /* Removes and returns the last element in this list. */\
    TYPE (*pop_last)(struct em_double_list_##NAME *this);\
    /* Removes and returns a pointer to the last element in this list. */\
    TYPE *(*pop_ptr_last)(struct em_double_list_##NAME *this);\
    /* Remove a given element from the linked list. */\
    void (*remove_node)(struct em_double_list_##NAME *this, em_double_list_node_t *node);\
    /* Remove an element at a specified index in the list. WARN: Requires traversal. */\
    void (*remove)(struct em_double_list_##NAME *this, size_t index);\
    /* Frees all resources associated with the list. WARN: Requires traversal. */\
    void (*destroy)(struct em_double_list_##NAME *this);\
} em_double_list_##NAME##_t;\
\
em_double_list_##NAME##_t *em_double_list_##NAME##_new(const em_double_list_desc_t *desc);

/* Definition template. */
#define DOUBLE_LIST(NAME) em_double_list_##NAME##_t
#define DOUBLE_LIST_NEW(NAME) em_double_list_##NAME##_new
#define DOUBLE_LIST_CLN(NAME) em_double_list_cln_func_##NAME
#define DEFINE_DOUBLE_LIST_CLN(TYPE, NAME) void DOUBLE_LIST_CLN(NAME)(TYPE *val)
#define DEFINE_DOUBLE_LIST(TYPE, NAME)\
\
static void _update_double_list_##NAME(em_double_list_##NAME##_t *this)\
{\
    this->count = this->_dll->count;\
}\
\
static em_double_list_iter_t *_iterator_double_list_##NAME(em_double_list_##NAME##_t *this)\
{\
    return em_dll_iterator(this->_dll);\
}\
\
static TYPE _get_first_double_list_##NAME(em_double_list_##NAME##_t *this)\
{\
    TYPE *res = em_dll_first(this->_dll);\
    return *res;\
}\
\
static TYPE *_get_ptr_first_double_list_##NAME(em_double_list_##NAME##_t *this)\
{\
    TYPE *res = em_dll_first(this->_dll);\
    return res;\
}\
\
static TYPE _get_last_double_list_##NAME(em_double_list_##NAME##_t *this)\
{\
    TYPE *res = em_dll_last(this->_dll);\
    return *res;\
}\
\
static TYPE *_get_ptr_last_double_list_##NAME(em_double_list_##NAME##_t *this)\
{\
    TYPE *res = em_dll_last(this->_dll);\
    return res;\
}\
\
static TYPE _get_double_list_##NAME(em_double_list_##NAME##_t *this, size_t index)\
{\
    TYPE *res = em_dll_get(this->_dll, index);\
    return *res;\
}\
\
static TYPE *_get_ptr_double_list_##NAME(em_double_list_##NAME##_t *this, size_t index)\
{\
    TYPE *res = em_dll_get(this->_dll, index);\
    return res;\
}\
\
static em_double_list_node_t *_get_node_double_list_##NAME(em_double_list_##NAME##_t *this, size_t index)\
{\
    em_double_list_node_t *res = em_dll_get_node(this->_dll, index);\
    return res;\
}\
\
static void _insert_after_double_list_##NAME(em_double_list_##NAME##_t *this, em_double_list_node_t *node, TYPE value)\
{\
    TYPE *value_ptr = malloc(sizeof(TYPE));\
    *value_ptr = value;\
    em_dll_insert_after(this->_dll, node, value_ptr);\
    _update_double_list_##NAME(this);\
}\
static void _insert_ptr_after_double_list_##NAME(em_double_list_##NAME##_t *this, em_double_list_node_t *node, TYPE *value_ptr)\
{\
    em_dll_insert_after(this->_dll, node, value_ptr);\
    _update_double_list_##NAME(this);\
}\
\
static void _insert_before_double_list_##NAME(em_double_list_##NAME##_t *this, em_double_list_node_t *node, TYPE value)\
{\
    TYPE *value_ptr = malloc(sizeof(TYPE));\
    *value_ptr = value;\
    em_dll_insert_before(this->_dll, node, value_ptr);\
    _update_double_list_##NAME(this);\
}\
static void _insert_ptr_before_double_list_##NAME(em_double_list_##NAME##_t *this, em_double_list_node_t *node, TYPE *value_ptr)\
{\
    em_dll_insert_before(this->_dll, node, value_ptr);\
    _update_double_list_##NAME(this);\
}\
\
static void _insert_at_double_list_##NAME(em_double_list_##NAME##_t *this, size_t index, TYPE value)\
{\
    TYPE *value_ptr = malloc(sizeof(TYPE));\
    *value_ptr = value;\
    em_dll_insert_at(this->_dll, index, value_ptr);\
    _update_double_list_##NAME(this);\
}\
static void _insert_ptr_at_double_list_##NAME(em_double_list_##NAME##_t *this, size_t index, TYPE *value_ptr)\
{\
    em_dll_insert_at(this->_dll, index, value_ptr);\
    _update_double_list_##NAME(this);\
}\
\
static void _append_double_list_##NAME(em_double_list_##NAME##_t *this, TYPE value)\
{\
    TYPE *value_ptr = malloc(sizeof(TYPE));\
    *value_ptr = value;\
    em_dll_append(this->_dll, value_ptr);\
    _update_double_list_##NAME(this);\
}\
\
static void _append_ptr_double_list_##NAME(em_double_list_##NAME##_t *this, TYPE *value_ptr)\
{\
    em_dll_append(this->_dll, value_ptr);\
    _update_double_list_##NAME(this);\
}\
\
static TYPE _pop_double_list_##NAME(em_double_list_##NAME##_t *this, size_t idx)\
{\
    TYPE *pop = em_dll_pop(this->_dll, idx);\
    TYPE res = *pop;\
    this->_dll->_cln_func(pop);\
    _update_double_list_##NAME(this);\
    return res;\
}\
\
static TYPE *_pop_ptr_double_list_##NAME(em_double_list_##NAME##_t *this, size_t idx)\
{\
    TYPE *res = em_dll_pop(this->_dll, idx);\
    _update_double_list_##NAME(this);\
    return res;\
}\
\
static TYPE _pop_first_double_list_##NAME(em_double_list_##NAME##_t *this)\
{\
    TYPE *pop = em_dll_pop_first(this->_dll);\
    TYPE res = *pop;\
    this->_dll->_cln_func(pop);\
    _update_double_list_##NAME(this);\
    return res;\
}\
\
static TYPE *_pop_ptr_first_double_list_##NAME(em_double_list_##NAME##_t *this)\
{\
    TYPE *res = em_dll_pop_first(this->_dll);\
    _update_double_list_##NAME(this);\
    return res;\
}\
\
static TYPE _pop_last_double_list_##NAME(em_double_list_##NAME##_t *this)\
{\
    TYPE *pop = em_dll_pop_last(this->_dll);\
    TYPE res = *pop;\
    this->_dll->_cln_func(pop);\
    _update_double_list_##NAME(this);\
    return res;\
}\
\
static TYPE *_pop_ptr_last_double_list_##NAME(em_double_list_##NAME##_t *this)\
{\
    TYPE *res = em_dll_pop_last(this->_dll);\
    _update_double_list_##NAME(this);\
    return res;\
}\
\
static void _remove_node_double_list_##NAME(em_double_list_##NAME##_t *this, em_double_list_node_t *node)\
{\
    em_dll_remove_node(this->_dll, node);\
    _update_double_list_##NAME(this);\
}\
\
static void _remove_double_list_##NAME(em_double_list_##NAME##_t *this, size_t index)\
{\
    em_dll_remove_at(this->_dll, index);\
    _update_double_list_##NAME(this);\
}\
\
static void _destroy_double_list_##NAME(em_double_list_##NAME##_t *this)\
{\
    em_dll_destroy(this->_dll);\
    free(this);\
}\
\
em_double_list_##NAME##_t *em_double_list_##NAME##_new(const em_double_list_desc_t *desc)\
{\
    if (!desc->cln_func)\
    {\
        EM_LOG("Doubly linked list initialization params are invalid.\n", NULL);\
        exit(1);\
    }\
    \
    em_double_list_##NAME##_t *res = malloc(sizeof(em_double_list_##NAME##_t));\
    if (!res)\
    {\
        EM_LOG("Failed to allocate memory for doubly linked list.\n", NULL);\
        exit(1);\
    }\
    \
    res->_dll              = em_dll_new(desc);\
    \
    res->count             = 0;\
    \
    res->iterator          = _iterator_double_list_##NAME;\
    res->get_first         = _get_first_double_list_##NAME;\
    res->get_ptr_first     = _get_ptr_first_double_list_##NAME;\
    res->get_last          = _get_last_double_list_##NAME;\
    res->get_ptr_last      = _get_ptr_last_double_list_##NAME;\
    res->get               = _get_double_list_##NAME;\
    res->get_ptr           = _get_ptr_double_list_##NAME;\
    res->get_node          = _get_node_double_list_##NAME;\
    res->insert_after      = _insert_after_double_list_##NAME;\
    res->insert_ptr_after  = _insert_ptr_after_double_list_##NAME;\
    res->insert_before     = _insert_before_double_list_##NAME;\
    res->insert_ptr_before = _insert_ptr_before_double_list_##NAME;\
    res->insert_at         = _insert_at_double_list_##NAME;\
    res->insert_ptr_at     = _insert_ptr_at_double_list_##NAME;\
    res->append            = _append_double_list_##NAME;\
    res->append_ptr        = _append_ptr_double_list_##NAME;\
    res->pop               = _pop_double_list_##NAME;\
    res->pop_ptr           = _pop_ptr_double_list_##NAME;\
    res->pop_first         = _pop_first_double_list_##NAME;\
    res->pop_ptr_first     = _pop_ptr_first_double_list_##NAME;\
    res->pop_last          = _pop_last_double_list_##NAME;\
    res->pop_ptr_last      = _pop_ptr_last_double_list_##NAME;\
    res->remove_node       = _remove_node_double_list_##NAME;\
    res->remove            = _remove_double_list_##NAME;\
    res->destroy           = _destroy_double_list_##NAME;\
    \
    return res;\
}

typedef struct em_double_list_node {
    void *val;

    struct em_double_list_node *_next;
    struct em_double_list_node *_prev;
} em_double_list_node_t;

typedef struct em_double_list {
    size_t count;

    int32_t _flags;
    void_cln_func _cln_func;
    em_double_list_node_t *_head;
    em_double_list_node_t *_tail;
} em_double_list_t;

typedef struct em_double_list_iter {
    bool has_next;
    void (*next)(struct em_double_list_iter *this);
    em_double_list_node_t *(*get)(struct em_double_list_iter *this);

    em_double_list_node_t *_curr;
    em_double_list_t *_dll;
} em_double_list_iter_t;

typedef struct em_double_list_desc {
    void_cln_func cln_func;
    int32_t flags;
} em_double_list_desc_t;

em_double_list_node_t *_dll_iter_get(em_double_list_iter_t *iter);
void _dll_iter_next(em_double_list_iter_t *iter);
em_double_list_iter_t *em_dll_iterator(em_double_list_t *dll);

em_double_list_t *em_dll_new(const em_double_list_desc_t *desc);
void *em_dll_first(em_double_list_t *this);
void *em_dll_last(em_double_list_t *this);
void *em_dll_get(em_double_list_t *this, size_t index);
em_double_list_node_t *em_dll_get_node(em_double_list_t *this, size_t index);
void em_dll_insert_after(em_double_list_t *this, em_double_list_node_t *node, void *val);
void em_dll_insert_before(em_double_list_t *this, em_double_list_node_t *node, void *val);
void em_dll_insert_at(em_double_list_t *this, size_t index, void *val);
void em_dll_append(em_double_list_t *this, void *val);
void *em_dll_pop(em_double_list_t *this, size_t index);
void *em_dll_pop_first(em_double_list_t *this);
void *em_dll_pop_last(em_double_list_t *this);
void em_dll_remove_node(em_double_list_t *this, em_double_list_node_t *node);
void em_dll_remove_at(em_double_list_t *this, size_t index);

void em_dll_destroy(em_double_list_t *this);

#endif // EM_DOUBLE_LIST_INCLUDED

/*     ______  _______  __    ________  __________   ___________  ______________  _   __
 *    /   /  |/  / __ \/ /   / ____/  |/  / ____/ | / /_  __/   |/_  __/   / __ \/ | / /
 *    / // /|_/ / /_/ / /   / __/ / /|_/ / __/ /  |/ / / / / /| | / /  / // / / /  |/ /
 *   / // /  / / ____/ /___/ /___/ /  / / /___/ /|  / / / / ___ |/ /  / // /_/ / /|  /
 * /___/_/  /_/_/   /_____/_____/_/  /_/_____/_/ |_/ /_/ /_/  |_/_/ /___/\____/_/ |_/
 *
 */

#ifdef EM_DOUBLE_LIST_IMPL
#include <stdlib.h> // exit
#include <stdio.h> // fprintf, stderr

em_double_list_node_t *_dll_iter_get(em_double_list_iter_t *iter)
{
    return iter->_curr;
}

void _dll_iter_next(em_double_list_iter_t *iter)
{
    if (!iter->has_next)
        return;

    iter->_curr = iter->_curr->_next;
    iter->has_next = iter->_curr;
}

em_double_list_iter_t *em_dll_iterator(em_double_list_t *dll)
{
    em_double_list_iter_t *res = malloc(sizeof(em_double_list_iter_t));

    res->has_next = dll->count > 0;
    res->get = _dll_iter_get;
    res->next = _dll_iter_next;
    res->_curr = dll->_head;
    res->_dll = dll;

    return res;
}

em_double_list_t *em_dll_new(const em_double_list_desc_t *desc)
{
    em_double_list_t *res = malloc(sizeof(em_double_list_t));

    res->_cln_func = desc->cln_func;
    res->_flags = desc->flags;
    res->_head = NULL;
    res->_tail = res->_head;
    res->count = 0;

    return res;
}

void *em_dll_first(em_double_list_t *this)
{   
    if (this->count == 0)
    {
        EM_LOG("Attempting to access invalid element in dll get.\n", NULL);
        exit(1);
    }

    return this->_head;
}

void *em_dll_last(em_double_list_t *this)
{   
    if (this->count == 0)
    {
        EM_LOG("Attempting to access invalid element in dll get.\n", NULL);
        exit(1);
    }

    return this->_tail;
}

void *em_dll_get(em_double_list_t *this, size_t idx)
{
    if (idx >= this->count || this->count == 0)
    {
        EM_LOG("Attempting to access invalid element in dll get.\n", NULL);
        exit(1);
    }

    size_t ctr = 0;
    em_double_list_node_t *curr = this->_head;
    while (ctr < idx)
    {
        curr = curr->_next;
        ctr++;
    }

    return curr->val;
}

em_double_list_node_t *em_dll_get_node(em_double_list_t *this, size_t idx)
{
    if (idx >= this->count || this->count == 0)
    {
        EM_LOG("Attempting to access invalid element in dll get.\n", NULL);
        exit(1);
    }

    size_t ctr = 0;
    em_double_list_node_t *curr = this->_head;
    while (ctr < idx)
    {
        curr = curr->_next;
        ctr++;
    }

    return curr;
}

void em_dll_insert_after(em_double_list_t *this, em_double_list_node_t *node, void *val)
{
    if (!node)
    {
        EM_LOG("Attempting to access invalid element at dll insert after.\n", NULL);
        exit(1);
    }

    em_double_list_node_t *n = malloc(sizeof(em_double_list_node_t));
    n->val = val;
    n->_next = node->_next;
    n->_prev = node;
    n->_prev->_next = n;

    if (n->_next)
        n->_next->_prev = n;
    else 
        this->_tail = n;

    this->count++;
}

void em_dll_insert_before(em_double_list_t *this, em_double_list_node_t *node, void *val)
{
    if (!node)
    {
        EM_LOG("Attempting to access invalid element at dll insert before.\n", NULL);
        exit(1);
    }

    em_double_list_node_t *n = malloc(sizeof(em_double_list_node_t));
    n->val = val;
    n->_next = node;
    n->_prev = node->_prev;
    n->_next->_prev = n;

    if (n->_prev)
        n->_prev->_next = n;
    else 
        this->_head = n;

    this->count++;
}

void em_dll_insert_at(em_double_list_t *this, size_t idx, void *val)
{
    if (idx > this->count) // CAN insert at index == count as it is the same as appending.
    {
        EM_LOG("Attempting to access invalid element in dll get.\n", NULL);
        exit(1);
    }

    em_double_list_node_t *node = this->_head;
    for (size_t ctr = 0; ctr < idx; ctr++)
        node = node->_next;

    em_double_list_node_t *n = malloc(sizeof(em_double_list_node_t));
    n->val = val;
    n->_next = node;

    if (node) // Insert before end of list, same as insert before.
    {
        n->_prev = node->_prev;
        n->_next->_prev = n;

        if (n->_prev)
            n->_prev->_next = n;
        else 
            this->_head = n;
    }
    else // Insert at end of list, same as append.
    {
        if (this->count == 0) // First node, equal to head and tail.
        {
            n->_prev = NULL;
            this->_head = n;
            this->_tail = n;
        }
        else if (this->count == 1) // Second node, just head and tail.
        {
            n->_prev = this->_head;
            this->_head->_next = n;
            this->_tail = n;
        }
        else // Subsequent nodes, replaces tail, previous tail points to new node. 
        {
            n->_prev = this->_tail;
            n->_prev->_next = n;
            this->_tail = n;
        }
    }

    this->count++;
}

void em_dll_append(em_double_list_t *this, void *val)
{
    em_double_list_node_t *n = malloc(sizeof(em_double_list_node_t));
    n->val = val;
    n->_next = NULL;
    if (this->count == 0) // First node, equal to head and tail.
    {
        n->_prev = NULL;
        this->_head = n;
        this->_tail = n;
    }
    else if (this->count == 1) // Second node, just head and tail.
    {
        n->_prev = this->_head;
        this->_head->_next = n;
        this->_tail = n;
    }
    else // Subsequent nodes, replaces tail, previous tail points to new node. 
    {
        n->_prev = this->_tail;
        n->_prev->_next = n;
        this->_tail = n;
    }

    this->count++;
}

void *em_dll_pop(em_double_list_t *this, size_t idx)
{
    if (this->count == 0 || idx >= this->count)
    {
        EM_LOG("Attempting to access invalid element at dll remove at.\n", NULL);
        exit(1);
    }

    size_t ctr = 0;
    em_double_list_node_t *node = this->_head;
    while (ctr++ < idx)
        node = node->_next;

    if (idx == 0)
    {
        node->_next->_prev = NULL;
        this->_head = node->_next;
    }
    else if (idx == this->count - 1)
    {
        node->_prev->_next = NULL;
        this->_tail = node->_prev;
    }
    else 
    {
        node->_next->_prev = node->_prev;
        node->_prev->_next = node->_next;
    }

    void *res = node->val;
    free(node);
    this->count--;
    return res;
}

void *em_dll_pop_first(em_double_list_t *this)
{
    if (this->count == 0)
    {
        EM_LOG("Attempting to access invalid element at dll pop first.\n", NULL);
        exit(1);
    }

    em_double_list_node_t *prev_head = this->_head;
    this->_head = this->_head->_next;
    if (this->_head)
        this->_head->_prev = NULL;

    void *res = prev_head->val;
    free(prev_head);
    this->count--;
    return res;
}

void *em_dll_pop_last(em_double_list_t *this)
{
    if (this->count == 0)
    {
        EM_LOG("Attempting to access invalid element at dll pop last.\n", NULL);
        exit(1);
    }

    em_double_list_node_t *prev_tail = this->_tail;
    this->_tail = prev_tail->_prev;
    if (this->_tail)
        this->_tail->_next = NULL;

    void *res = prev_tail->val;
    free(prev_tail);
    this->count--;
    return res;
}

void em_dll_remove_node(em_double_list_t *this, em_double_list_node_t *node)
{
    if (!node || this->count == 0)
    {
        EM_LOG("Attempting to access invalid element at dll remove node.\n", NULL);
        exit(1);
    }

    if (this->_head == node && this->_tail == node)
    {
        this->_head = NULL;
        this->_tail = NULL;
    }
    else if (this->_head == node)
    {
        node->_next->_prev = NULL;
        this->_head = node->_next;
    }
    else if (this->_tail == node)
    {
        node->_prev->_next = NULL;
        this->_tail = node->_prev;
    }
    else 
    {
        node->_next->_prev = node->_prev;
        node->_prev->_next = node->_next;
    }

    this->_cln_func(node->val);
    free(node);
    this->count--;
}

void em_dll_remove_at(em_double_list_t *this, size_t idx)
{
    if (this->count == 0 || idx >= this->count)
    {
        EM_LOG("Attempting to access invalid element at dll remove at.\n", NULL);
        exit(1);
    }

    size_t ctr = 0;
    em_double_list_node_t *node = this->_head;
    while (ctr++ < idx)
        node = node->_next;

    if (idx == 0)
    {
        node->_next->_prev = NULL;
        this->_head = node->_next;
    }
    else if (idx == this->count - 1)
    {
        node->_prev->_next = NULL;
        this->_tail = node->_prev;
    }
    else 
    {
        node->_next->_prev = node->_prev;
        node->_prev->_next = node->_next;
    }

    this->_cln_func(node->val);
    free(node);
    this->count--;
}

void em_dll_destroy(em_double_list_t *this)
{
    if (this->count > 0)
    {
        em_double_list_node_t *curr = this->_head;
        em_double_list_node_t *next = NULL;
        while (curr)
        {
            next = curr->_next;
            this->_cln_func(curr->val);
            free(curr);
            curr = next;
        }
    }

    free(this);
}

#endif // EM_DLL_IMPL

#endif
