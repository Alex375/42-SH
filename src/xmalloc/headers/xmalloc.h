#ifndef INC_42_SH_XMALLOC_H
#define INC_42_SH_XMALLOC_H

#include <stdlib.h>

struct xmalloc_t *head = NULL;


/**
** @brief                   Represents a memory allocation.
** @param next              Next allocation.
** @param previous          Previous allocation.
*/
struct xmalloc_t
{
    struct xmalloc_t *next;
    struct xmalloc_t *previous;
};

/**
** @brief                   Create a new_allocation.
** @param size              Size of allocation.
*/
void *xmalloc(size_t size);

/**
** @brief                   Create a new_allocation initialized at 0 with mult overflow check.
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
** @brief                   Free every memory allocation known by xmalloc.
*/
void xfree_all(void);


#endif //INC_42_SH_XMALLOC_H
