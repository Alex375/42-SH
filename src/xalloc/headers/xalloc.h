#ifndef INC_42_SH_XALLOC_H
#define INC_42_SH_XALLOC_H

#include <stdlib.h>

/**
** @brief                   Represents a memory allocation.
** @param next              Next allocation.
** @param previous          Previous allocation.
*/
struct xmalloc_t
{
    struct xmalloc_t *next;
    struct xmalloc_t *previous;
    size_t allocation_size;
};

/**
** @brief                   Create a new_allocation.
** @param size              Size of allocation.
*/
void *xmalloc(size_t size);

/**
** @brief                   Create a new_allocation initialized at 0 with mult
*overflow check.
** @param n                 Number of allocation.
** @param size              Size allocation.
*/
void *xcalloc(size_t n, size_t size);

/**
** @brief                   Free a memory allocation.
** @param data              Allocation to free.
*/
void xfree(void *data);

/**
** @brief                   Modify the size of a memory allocation.
** @param data              Allocation to modify
** @param new_size          The new size of the allocation
*/
void *xrealloc(void *data, size_t new_size);

/**
** @brief                   Modify the size of a memory allocation and set every
*new element at 0.
** @param data              Allocation to modify
** @param new_size          The new size of the allocation
*/
void *xrecalloc(void *data, size_t new_size);

/**
** @brief                   Free every memory allocation known by xalloc.
*/
void xfree_all(void);

#endif // INC_42_SH_XALLOC_H
