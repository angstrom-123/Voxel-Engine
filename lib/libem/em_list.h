#ifndef EM_LIST_H
#define EM_LIST_H 

#ifndef EM_DLL_INCLUDED
#define EM_DLL_INCLUDED

#include <stddef.h>
#include <stdbool.h>

/* Declaration template. */
#define DECLARE_DLL(TYPE, NAME)\
typedef struct em_dll_##NAME {\
    /* Members. */\
    size_t count;\
    em_dll_t *_dll;\
    /* Methods. */\
    /* Get a new iterator for this dll. */\
    em_dll_iter_t *(*iterator)(struct em_dll_##NAME *this);\
    /* Get a value at an index in the linked list. WARN: Requires traversal. */\
    TYPE (*get)(struct em_dll_##NAME *this, size_t index);\
    /* Get a pointer to the value at an index in the linked list. WARN: Requires traversal. */\
    TYPE *(*get_ptr)(struct em_dll_##NAME *this, size_t index);\
    /* Get a pointer to the node in the dll at an index in the linked list. WARN: Requires traversal. */\
    em_dll_node_t *(*get_node)(struct em_dll_##NAME *this, size_t index);\
    /* Add a new element after an existing element in the list. */\
    void (*insert_after)(struct em_dll_##NAME *this, em_dll_node_t *node, TYPE value);\
    /* Add a new element after an existing element in the list. */\
    void (*insert_ptr_after)(struct em_dll_##NAME *this, em_dll_node_t *node, TYPE *value_ptr);\
    /* Add a new element before an existing element in the list. */\
    void (*insert_before)(struct em_dll_##NAME *this, em_dll_node_t *node, TYPE value);\
    /* Add a new element before an existing element in the list. */\
    void (*insert_ptr_before)(struct em_dll_##NAME *this, em_dll_node_t *node, TYPE *value_ptr);\
    /* Add a new element at a specified index in the list. WARN: Requires traversal. */\
    void (*insert_at)(struct em_dll_##NAME *this, size_t index, TYPE value);\
    /* Add a new element at a specified index in the list. WARN: Requires traversal. */\
    void (*insert_ptr_at)(struct em_dll_##NAME *this, size_t index, TYPE *value_ptr);\
    /* Add an element onto the end of the linked list. */\
    void (*append)(struct em_dll_##NAME *this, TYPE value);\
    /* Add an element onto the end of the linked list. */\
    void (*append_ptr)(struct em_dll_##NAME *this, TYPE *value_ptr);\
    /* Removes and returns the element at index in the list. WARN: Requires traversal. */\
    TYPE (*pop)(struct em_dll_##NAME *this, size_t index);\
    /* Removes and returns the element at index in the list. WARN: Requires traversal. */\
    TYPE *(*pop_ptr)(struct em_dll_##NAME *this, size_t index);\
    /* Removes and returns the first element in this list. */\
    TYPE (*pop_first)(struct em_dll_##NAME *this);\
    /* Removes and returns a pointer to the first element in this list. */\
    TYPE *(*pop_ptr_first)(struct em_dll_##NAME *this);\
    /* Removes and returns the last element in this list. */\
    TYPE (*pop_last)(struct em_dll_##NAME *this);\
    /* Removes and returns a pointer to the last element in this list. */\
    TYPE *(*pop_ptr_last)(struct em_dll_##NAME *this);\
    /* Remove a given element from the linked list. */\
    void (*remove_node)(struct em_dll_##NAME *this, em_dll_node_t *node);\
    /* Remove an element at a specified index in the list. WARN: Requires traversal. */\
    void (*remove)(struct em_dll_##NAME *this, size_t index);\
    /* Frees all resources associated with the list. WARN: Requires traversal. */\
    void (*destroy)(struct em_dll_##NAME *this);\
} em_dll_##NAME##_t;\
\
em_dll_##NAME##_t *em_dll_##NAME##_new(void);

/* Definition template. */
#define DLL(NAME) em_dll_##NAME##_t
#define DLL_NEW(NAME) em_dll_##NAME##_new();
#define DEFINE_DLL(TYPE, NAME)\
\
static void _update_##NAME(em_dll_##NAME##_t *this)\
{\
    this->count = this->_dll->count;\
}\
\
static em_dll_iter_t *_iterator_##NAME(em_dll_##NAME##_t *this)\
{\
    return em_dll_iterator(this->_dll);\
}\
\
static TYPE _get_##NAME(em_dll_##NAME##_t *this, size_t index)\
{\
    TYPE *res = em_dll_get(this->_dll, index);\
    return *res;\
}\
\
static TYPE *_get_ptr_##NAME(em_dll_##NAME##_t *this, size_t index)\
{\
    TYPE *res = em_dll_get(this->_dll, index);\
    return res;\
}\
\
static em_dll_node_t *_get_node_##NAME(em_dll_##NAME##_t *this, size_t index)\
{\
    em_dll_node_t *res = em_dll_get_node(this->_dll, index);\
    return res;\
}\
\
static void _insert_after_##NAME(em_dll_##NAME##_t *this, em_dll_node_t *node, TYPE value)\
{\
    TYPE *value_ptr = malloc(sizeof(TYPE));\
    *value_ptr = value;\
    em_dll_insert_after(this->_dll, node, value_ptr);\
    _update_##NAME(this);\
}\
static void _insert_ptr_after_##NAME(em_dll_##NAME##_t *this, em_dll_node_t *node, TYPE *value_ptr)\
{\
    em_dll_insert_after(this->_dll, node, value_ptr);\
    _update_##NAME(this);\
}\
\
static void _insert_before_##NAME(em_dll_##NAME##_t *this, em_dll_node_t *node, TYPE value)\
{\
    TYPE *value_ptr = malloc(sizeof(TYPE));\
    *value_ptr = value;\
    em_dll_insert_before(this->_dll, node, value_ptr);\
    _update_##NAME(this);\
}\
static void _insert_ptr_before_##NAME(em_dll_##NAME##_t *this, em_dll_node_t *node, TYPE *value_ptr)\
{\
    em_dll_insert_before(this->_dll, node, value_ptr);\
    _update_##NAME(this);\
}\
\
static void _insert_at_##NAME(em_dll_##NAME##_t *this, size_t index, TYPE value)\
{\
    TYPE *value_ptr = malloc(sizeof(TYPE));\
    *value_ptr = value;\
    em_dll_insert_at(this->_dll, index, value_ptr);\
    _update_##NAME(this);\
}\
static void _insert_ptr_at_##NAME(em_dll_##NAME##_t *this, size_t index, TYPE *value_ptr)\
{\
    em_dll_insert_at(this->_dll, index, value_ptr);\
    _update_##NAME(this);\
}\
\
static void _append_##NAME(em_dll_##NAME##_t *this, TYPE value)\
{\
    TYPE *value_ptr = malloc(sizeof(TYPE));\
    *value_ptr = value;\
    em_dll_append(this->_dll, value_ptr);\
    _update_##NAME(this);\
}\
\
static void _append_ptr_##NAME(em_dll_##NAME##_t *this, TYPE *value_ptr)\
{\
    em_dll_append(this->_dll, value_ptr);\
    _update_##NAME(this);\
}\
\
static TYPE _pop_##NAME(em_dll_##NAME##_t *this, size_t idx)\
{\
    TYPE *pop = em_dll_pop(this->_dll, idx);\
    TYPE res = *pop;\
    free(pop);\
    _update_##NAME(this);\
    return res;\
}\
\
static TYPE *_pop_ptr_##NAME(em_dll_##NAME##_t *this, size_t idx)\
{\
    TYPE *res = em_dll_pop(this->_dll, idx);\
    _update_##NAME(this);\
    return res;\
}\
\
static TYPE _pop_first_##NAME(em_dll_##NAME##_t *this)\
{\
    TYPE *pop = em_dll_pop_first(this->_dll);\
    TYPE res = *pop;\
    free(pop);\
    _update_##NAME(this);\
    return res;\
}\
\
static TYPE *_pop_ptr_first_##NAME(em_dll_##NAME##_t *this)\
{\
    TYPE *res = em_dll_pop_first(this->_dll);\
    _update_##NAME(this);\
    return res;\
}\
\
static TYPE _pop_last_##NAME(em_dll_##NAME##_t *this)\
{\
    TYPE *pop = em_dll_pop_last(this->_dll);\
    TYPE res = *pop;\
    free(pop);\
    _update_##NAME(this);\
    return res;\
}\
\
static TYPE *_pop_ptr_last_##NAME(em_dll_##NAME##_t *this)\
{\
    TYPE *res = em_dll_pop_last(this->_dll);\
    _update_##NAME(this);\
    return res;\
}\
\
static void _remove_node_##NAME(em_dll_##NAME##_t *this, em_dll_node_t *node)\
{\
    em_dll_remove_node(this->_dll, node);\
    _update_##NAME(this);\
}\
\
static void _remove_##NAME(em_dll_##NAME##_t *this, size_t index)\
{\
    em_dll_remove(this->_dll, index);\
    _update_##NAME(this);\
}\
\
static void _destroy_##NAME(em_dll_##NAME##_t *this)\
{\
    em_dll_destroy(this->_dll);\
    free(this);\
}\
\
em_dll_##NAME##_t *em_dll_##NAME##_new(void)\
{\
    em_dll_##NAME##_t *res = malloc(sizeof(em_dll_##NAME##_t));\
    res->_dll              = em_dll_new();\
    \
    res->count             = 0;\
    \
    res->iterator          = _iterator_##NAME;\
    res->get               = _get_##NAME;\
    res->get_ptr           = _get_ptr_##NAME;\
    res->get_node          = _get_node_##NAME;\
    res->insert_after      = _insert_after_##NAME;\
    res->insert_ptr_after  = _insert_ptr_after_##NAME;\
    res->insert_before     = _insert_before_##NAME;\
    res->insert_ptr_before = _insert_ptr_before_##NAME;\
    res->insert_at         = _insert_at_##NAME;\
    res->insert_ptr_at     = _insert_ptr_at_##NAME;\
    res->append            = _append_##NAME;\
    res->append_ptr        = _append_ptr_##NAME;\
    res->pop               = _pop_##NAME;\
    res->pop_ptr           = _pop_ptr_##NAME;\
    res->pop_first         = _pop_first_##NAME;\
    res->pop_ptr_first     = _pop_ptr_first_##NAME;\
    res->pop_last          = _pop_last_##NAME;\
    res->pop_ptr_last      = _pop_ptr_last_##NAME;\
    res->remove_node       = _remove_node_##NAME;\
    res->remove            = _remove_##NAME;\
    res->destroy           = _destroy_##NAME;\
    \
    return res;\
}\

typedef struct em_dll_node {
    void *val;

    struct em_dll_node *_next;
    struct em_dll_node *_prev;
} em_dll_node_t;

typedef struct em_dll {
    size_t count;
    em_dll_node_t *head;
    em_dll_node_t *tail;
} em_dll_t;

typedef struct em_dll_iter {
    bool has_next;
    void *(*next)(struct em_dll_iter *this);

    em_dll_node_t *_curr;
    em_dll_t *_dll;
} em_dll_iter_t;

void *_dll_iter_next(em_dll_iter_t *iter);
em_dll_iter_t *em_dll_iterator(em_dll_t *dll);

em_dll_t *em_dll_new(void);
void *em_dll_get(em_dll_t *this, size_t index);
em_dll_node_t *em_dll_get_node(em_dll_t *this, size_t index);
void em_dll_insert_after(em_dll_t *this, em_dll_node_t *node, void *val);
void em_dll_insert_before(em_dll_t *this, em_dll_node_t *node, void *val);
void em_dll_insert_at(em_dll_t *this, size_t index, void *val);
void em_dll_append(em_dll_t *this, void *val);
void *em_dll_pop_first(em_dll_t *this);
void *em_dll_pop_last(em_dll_t *this);
void em_dll_remove_node(em_dll_t *this, em_dll_node_t *node);
void em_dll_remove_at(em_dll_t *this, size_t index);

void em_dll_destroy(em_dll_t *this);

#endif // EM_DLL_INCLUDED

/*     ______  _______  __    ________  __________   ___________  ______________  _   __
 *    /   /  |/  / __ \/ /   / ____/  |/  / ____/ | / /_  __/   |/_  __/   / __ \/ | / /
 *    / // /|_/ / /_/ / /   / __/ / /|_/ / __/ /  |/ / / / / /| | / /  / // / / /  |/ /
 *   / // /  / / ____/ /___/ /___/ /  / / /___/ /|  / / / / ___ |/ /  / // /_/ / /|  /
 * /___/_/  /_/_/   /_____/_____/_/  /_/_____/_/ |_/ /_/ /_/  |_/_/ /___/\____/_/ |_/
 *
 */

#ifdef EM_DLL_IMPL
#include <stdlib.h> // exit
#include <stdio.h> // fprintf, stderr

void *_dll_iter_next(em_dll_iter_t *iter)
{
    if (!iter->has_next)
        return NULL;

    if (!iter->_curr) // First call of iter next, from starting sentinel "NULL"
    {
        iter->_curr = iter->_dll->head;
        iter->has_next = (iter->_curr != iter->_dll->tail);
        if (iter->_curr)
            return iter->_curr->val;
        return NULL;
    }
    else 
    {
        iter->_curr = iter->_curr->_next;
        iter->has_next = (iter->_curr != iter->_dll->tail);
        return iter->_curr->val;
    }
}

em_dll_iter_t *em_dll_iterator(em_dll_t *dll)
{
    em_dll_iter_t *res = malloc(sizeof(em_dll_iter_t));
    res->has_next = dll->count > 0;
    res->next = _dll_iter_next;
    res->_curr = NULL;
    res->_dll = dll;
    return res;
}

em_dll_t *em_dll_new(void)
{
    em_dll_t *res = malloc(sizeof(em_dll_t));
    res->count = 0;
    res->head = NULL;
    res->tail = res->head;
    return res;
}

void *em_dll_get(em_dll_t *this, size_t idx)
{
    if (idx >= this->count || this->count == 0)
    {
        fprintf(stderr, "Attempting to access invalid element in dll get.\n");
        exit(1);
    }

    size_t ctr = 0;
    em_dll_node_t *curr = this->head;
    while (ctr < idx)
    {
        curr = curr->_next;
        ctr++;
    }

    return curr->val;
}

em_dll_node_t *em_dll_get_node(em_dll_t *this, size_t idx)
{
    if (idx >= this->count || this->count == 0)
    {
        fprintf(stderr, "Attempting to access invalid element in dll get.\n");
        exit(1);
    }

    size_t ctr = 0;
    em_dll_node_t *curr = this->head;
    while (ctr < idx)
    {
        curr = curr->_next;
        ctr++;
    }

    return curr;
}

void em_dll_insert_after(em_dll_t *this, em_dll_node_t *node, void *val)
{
    if (!node)
    {
        fprintf(stderr, "Attempting to access invalid element at dll insert after.\n");
        exit(1);
    }

    em_dll_node_t *n = malloc(sizeof(em_dll_node_t));
    n->val = val;
    n->_next = node->_next;
    n->_prev = node;
    n->_prev->_next = n;

    if (n->_next)
        n->_next->_prev = n;
    else 
        this->tail = n;

    this->count++;
}

void em_dll_insert_before(em_dll_t *this, em_dll_node_t *node, void *val)
{
    if (!node)
    {
        fprintf(stderr, "Attempting to access invalid element at dll insert before.\n");
        exit(1);
    }

    em_dll_node_t *n = malloc(sizeof(em_dll_node_t));
    n->val = val;
    n->_next = node;
    n->_prev = node->_prev;
    n->_next->_prev = n;

    if (n->_prev)
        n->_prev->_next = n;
    else 
        this->head = n;

    this->count++;
}

void em_dll_insert_at(em_dll_t *this, size_t idx, void *val)
{
    if (idx > this->count) // CAN insert at index == count as it is the same as appending.
    {
        fprintf(stderr, "Attempting to access invalid element in dll get.\n");
        exit(1);
    }

    em_dll_node_t *node = this->head;
    for (size_t ctr = 0; ctr < idx; ctr++)
        node = node->_next;

    em_dll_node_t *n = malloc(sizeof(em_dll_node_t));
    n->val = val;
    n->_next = node;

    if (node) // Insert before end of list, same as insert before.
    {
        n->_prev = node->_prev;
        n->_next->_prev = n;

        if (n->_prev)
            n->_prev->_next = n;
        else 
            this->head = n;
    }
    else // Insert at end of list, same as append.
    {
        if (this->count == 0) // First node, equal to head and tail.
        {
            n->_prev = NULL;
            this->head = n;
            this->tail = n;
        }
        else if (this->count == 1) // Second node, just head and tail.
        {
            n->_prev = this->head;
            this->head->_next = n;
            this->tail = n;
        }
        else // Subsequent nodes, replaces tail, previous tail points to new node. 
        {
            n->_prev = this->tail;
            n->_prev->_next = n;
            this->tail = n;
        }
    }

    this->count++;
}

void em_dll_append(em_dll_t *this, void *val)
{
    em_dll_node_t *n = malloc(sizeof(em_dll_node_t));
    n->val = val;
    n->_next = NULL;
    if (this->count == 0) // First node, equal to head and tail.
    {
        n->_prev = NULL;
        this->head = n;
        this->tail = n;
    }
    else if (this->count == 1) // Second node, just head and tail.
    {
        n->_prev = this->head;
        this->head->_next = n;
        this->tail = n;
    }
    else // Subsequent nodes, replaces tail, previous tail points to new node. 
    {
        n->_prev = this->tail;
        n->_prev->_next = n;
        this->tail = n;
    }

    this->count++;
}

void *em_dll_pop(em_dll_t *this, size_t idx)
{
    if (this->count == 0 || idx >= this->count)
    {
        fprintf(stderr, "Attempting to access invalid element at dll remove at.\n");
        exit(1);
    }

    size_t ctr = 0;
    em_dll_node_t *node = this->head;
    while (ctr++ < idx)
        node = node->_next;

    if (idx == 0)
    {
        node->_next->_prev = NULL;
        this->head = node->_next;
    }
    else if (idx == this->count - 1)
    {
        node->_prev->_next = NULL;
        this->tail = node->_prev;
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

void *em_dll_pop_first(em_dll_t *this)
{
    if (this->count == 0)
    {
        fprintf(stderr, "Attempting to access invalid element at dll pop first.\n");
        exit(1);
    }

    em_dll_node_t *prev_head = this->head;
    this->head = prev_head->_next;
    if (this->head)
        this->head->_prev = NULL;

    void *res = prev_head->val;
    free(prev_head);
    this->count--;
    return res;
}

void *em_dll_pop_last(em_dll_t *this)
{
    if (this->count == 0)
    {
        fprintf(stderr, "Attempting to access invalid element at dll pop last.\n");
        exit(1);
    }

    em_dll_node_t *prev_tail = this->tail;
    this->tail = prev_tail->_prev;
    if (this->tail)
        this->tail->_next = NULL;

    void *res = prev_tail->val;
    free(prev_tail);
    this->count--;
    return res;
}

void em_dll_remove_node(em_dll_t *this, em_dll_node_t *node)
{
    if (!node || this->count == 0)
    {
        fprintf(stderr, "Attempting to access invalid element at dll remove node.\n");
        exit(1);
    }

    if (this->head == node)
    {
        node->_next->_prev = NULL;
        this->head = node->_next;
    }
    else if (this->tail == node)
    {
        node->_prev->_next = NULL;
        this->tail = node->_prev;
    }
    else 
    {
        node->_next->_prev = node->_prev;
        node->_prev->_next = node->_next;
    }

    free(node->val);
    free(node);
    this->count--;
}

void em_dll_remove(em_dll_t *this, size_t idx)
{
    if (this->count == 0 || idx >= this->count)
    {
        fprintf(stderr, "Attempting to access invalid element at dll remove at.\n");
        exit(1);
    }

    size_t ctr = 0;
    em_dll_node_t *node = this->head;
    while (ctr++ < idx)
        node = node->_next;

    if (idx == 0)
    {
        node->_next->_prev = NULL;
        this->head = node->_next;
    }
    else if (idx == this->count - 1)
    {
        node->_prev->_next = NULL;
        this->tail = node->_prev;
    }
    else 
    {
        node->_next->_prev = node->_prev;
        node->_prev->_next = node->_next;
    }

    free(node->val);
    free(node);
    this->count--;
}

void em_dll_destroy(em_dll_t *this)
{
    if (this->count > 0)
    {
        em_dll_node_t *curr = this->head;
        em_dll_node_t *next = NULL;
        while (curr)
        {
            next = curr->_next;
            free(curr->val);
            free(curr);
            curr = next;
        }
    }

    free(this);
}

#endif // EM_DLL_IMPL

#endif
